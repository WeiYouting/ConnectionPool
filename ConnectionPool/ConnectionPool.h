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
	ʵ�����ӳع���ģ��
*/
class ConnectionPool {
public:
	// ��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();
	// �������ṩ���õ����ݿ�����
	// ʹ������ָ�룬�ض���ɾ���� ��������ʱ�Զ��������黹����
	std::shared_ptr<Connection> getConnection();

	
private:
	// �̳߳ز��õ���ģʽ��˽�л����캯��
	ConnectionPool();

	// �������ļ��м�������
    bool loadConfig(); 

	// �����ڶ������߳��У���������������
	void produceConnectionTask();

	// ip��ַ
	std::string _ip;
	//�˿ں�
	unsigned short _port;
	// �û���
	std::string _username;
	// ��¼����
	std::string _password;
	// ���ݿ���
	std::string _dbname;
	// ��ʼ������
	int _initSize;
	// ���������
	int _maxSize;
	// ������ʱ��
	int _maxIdleTime;
	// ���ӳ�ʱʱ��
	int _connectionTimeout;

	// ������ӵĶ���
	std::queue<Connection*> _connectionQueue;
	// ά���̰߳�ȫ������
	std::mutex _queueMutex;
	// ��¼������connection����������
	std::atomic_int _connectionCnt;
	// ���������������������������̺߳������̵߳�ͨ��
	std::condition_variable cv;
};