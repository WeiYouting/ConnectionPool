#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>
#include "Connection.h"

/*
	实现连接池功能模块
*/
class ConnectionPool {
public:
	// 获取连接池对象实例
	static ConnectionPool* getConnectionPool();
	// 给外面提供可用的数据库连接
	// 使用智能指针，重定义删除器 出作用域时自动析构，归还连接
	std::shared_ptr<Connection> getConnection();

	
private:
	// 线程池采用单例模式，私有化构造函数
	ConnectionPool();

	// 从配置文件中加载配置
    bool loadConfig(); 

	// 运行在独立的线程中，负责生成新连接
	void produceConnectionTask();

	// ip地址
	std::string _ip;
	//端口号
	unsigned short _port;
	// 用户名
	std::string _username;
	// 登录密码
	std::string _password;
	// 数据库名
	std::string _dbname;
	// 初始连接量
	int _initSize;
	// 最大连接量
	int _maxSize;
	// 最大空闲时间
	int _maxIdleTime;
	// 连接超时时间
	int _connectionTimeout;

	// 存放连接的队列
	std::queue<Connection*> _connectionQueue;
	// 维护线程安全互斥锁
	std::mutex _queueMutex;
	// 记录创建的connection连接总数量
	std::atomic_int _connectionCnt;
	// 设置条件变量，用于连接生产线程和消费线程的通信
	std::condition_variable cv;
};