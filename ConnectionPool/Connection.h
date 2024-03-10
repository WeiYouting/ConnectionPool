#pragma once

#include "mysql.h"
#include <string>
#include <ctime>

/*
	实现Mysql数据库操作
*/
class Connection {
public:
	// 初始化数据库连接
	Connection();
	
	// 释放数据库连接资源
	~Connection();

	// 连接数据库
	bool connect(std::string ip,unsigned short port,std::string username, std::string password, std::string dbname);

	// 更新操作 insert、delete、update
	bool update(std::string sql);

	// 查询操作 select
	MYSQL_RES* query(std::string sql);

	// 刷新连接存活时间
	void refreshAliveTime() {
		this->_alivetime = clock();
	}

	// 获得空闲时间
	clock_t getAliveTime() const{
		return clock() - this->_alivetime;
	}

private:
	MYSQL* _conn;
	// 记录进入空闲状态后的存活时间
	clock_t _alivetime;
};
