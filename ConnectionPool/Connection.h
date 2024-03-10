#pragma once

#include "mysql.h"
#include <string>
#include <ctime>

/*
	ʵ��Mysql���ݿ����
*/
class Connection {
public:
	// ��ʼ�����ݿ�����
	Connection();
	
	// �ͷ����ݿ�������Դ
	~Connection();

	// �������ݿ�
	bool connect(std::string ip,unsigned short port,std::string username, std::string password, std::string dbname);

	// ���²��� insert��delete��update
	bool update(std::string sql);

	// ��ѯ���� select
	MYSQL_RES* query(std::string sql);

	// ˢ�����Ӵ��ʱ��
	void refreshAliveTime() {
		this->_alivetime = clock();
	}

	// ��ÿ���ʱ��
	clock_t getAliveTime() const{
		return clock() - this->_alivetime;
	}

private:
	MYSQL* _conn;
	// ��¼�������״̬��Ĵ��ʱ��
	clock_t _alivetime;
};
