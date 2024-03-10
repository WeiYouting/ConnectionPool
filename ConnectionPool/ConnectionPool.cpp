#include "ConnectionPool.h"
#include "public.h"

// 线程安全的懒汉单例模式接口
ConnectionPool* ConnectionPool::getConnectionPool() {
	static ConnectionPool pool;
	return &pool;
}

std::shared_ptr<Connection> ConnectionPool::getConnection(){
	std::unique_lock<std::mutex> lock(this->_queueMutex);
	while (this->_connectionQueue.empty()) {
		if (this->_connectionQueue.empty()) {
			// 等待空闲连接
			if (std::cv_status::timeout == cv.wait_for(lock, std::chrono::milliseconds(this->_connectionTimeout))) {
				if (this->_connectionQueue.empty()) {
					LOG("获取连接超时");
					return nullptr;
				}
			}
		}
	}

	// 自定义删除器，不需要释放连接，只需要将连接入队
	std::shared_ptr<Connection> sp(this->_connectionQueue.front(), 
		[&](Connection *pcon) {
			std::unique_lock<std::mutex> lock(this->_queueMutex);
			//归还线程
			this->_connectionQueue.push(pcon);
			++this->_connectionCnt;
			// 刷新空闲时间
			pcon->refreshAliveTime();
		});

	this->_connectionQueue.pop();
	// 通知生产者线程检查队列状态
	cv.notify_all();
	
	return sp;
}

/*
	线程池使用单例模式，构造函数只执行一次
*/
ConnectionPool::ConnectionPool() {
	// 加载配置项
	if (!loadConfig()) {
		LOG("load config error");
		return;
	}

	// 创建初始数量的连接
	for (int i = 0; i < _initSize; ++i) {
		Connection* con = new Connection();
		con->connect(this->_ip, this->_port, this->_username,
			this->_password,this->_dbname); 
		// 刷新空闲时间
		con->refreshAliveTime();

		// 添加至连接池队列，创建连接数自增
		this->_connectionQueue.push(con);
		++this->_connectionCnt;

	}


	// 启动新线程作为连接的生产者
	std::thread produce(std::bind(&ConnectionPool::produceConnectionTask,this));
	produce.detach();


	// 启动新的定时线程，扫描超过最大空闲时间的连接，进行回收；
	std::thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

void ConnectionPool::scannerConnectionTask(){

	while (TRUE) {
		std::this_thread::sleep_for(std::chrono::seconds(this->_maxIdleTime));

		// 扫描整个队列，释放多余连接
		std::unique_lock<std::mutex> lock(this->_queueMutex);
		while (this->_connectionCnt > this->_initSize) {
			Connection* p = this->_connectionQueue.front();
			if (p->getAliveTime() >= (this->_maxIdleTime * 1000)) {
				// 出队，调用析构函数释放连接
				this->_connectionQueue.pop();
				--this->_connectionCnt;
				delete p;
			}
			else {
				break;
			}
		}
	}

}

void ConnectionPool::produceConnectionTask() {

	// 运行在独立的线程中，负责生成新连接
	while (TRUE) {
		std::unique_lock<std::mutex> lock(this->_queueMutex);
		while (!this->_connectionQueue.empty()) {
			// 连接队列不为空，生产线程进入等待状态
			cv.wait(lock);
		}
		// 连接数量没有达到上限，继续创建新连接
		if (this->_connectionCnt < this->_maxSize) {
			Connection* con = new Connection();
			con->connect(this->_ip, this->_port, this->_username,
				this->_password, this->_dbname);
			// 刷新空闲时间
			con->refreshAliveTime();
			// 添加至连接池队列，创建连接数自增
			this->_connectionQueue.push(con);
			++this->_connectionCnt;
		}
		// 通知消费者线程
		cv.notify_all();
	}

}





bool ConnectionPool::loadConfig() {
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr) {
		LOG("mysql.ini file is not exists");
		return false;
	}

	while (!feof(pf)) {
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		std::string str(line);
		
		//排除无效项
		int idx = str.find('=', 0);
		if (idx == -1) {
			continue;
		}

		int endidx = str.find('\n', idx);
		std::string key = str.substr(0, idx);
		std::string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip") {
			this->_ip = value;
		}
		else if (key == "port") {
			this->_port = atoi(value.c_str());
		}
		else if (key == "username") {
			this->_username = value;
		}
		else if (key == "password") {
			this->_password = value;
		}
		else if (key == "dbname") {
			this->_dbname = value;
		}
		else if (key == "initSize") {
			this->_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize") {
			this->_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime") {
			this->_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeout") {
			this->_connectionTimeout = atoi(value.c_str());
		}

	}

	return true;
}

