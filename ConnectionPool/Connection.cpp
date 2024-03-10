#include "public.h"
#include "Connection.h"

Connection::Connection() {
	// ��ʼ�����ݿ�����
	_conn = mysql_init(nullptr);
}


Connection::~Connection() {
	// �ͷ����ݿ�������Դ
	mysql_close(_conn);
}


bool Connection::connect(std::string ip,unsigned short port,std::string username, std::string password, std::string dbname) {
	// �������ݿ�
	MYSQL *p = mysql_real_connect(_conn, ip.c_str(),username.c_str(), password.c_str(), dbname.c_str(),port,nullptr,0);
	return p != nullptr;
}

bool Connection::update(std::string sql) {
	// ���²��� insert��delete��update
	if (mysql_query(_conn, sql.c_str())) {
		LOG("����ʧ��:" + sql);
		return false;
	}
	return true;
}

MYSQL_RES* Connection::query(std::string sql) {
	// ��ѯ���� select
	if (mysql_query(_conn, sql.c_str())) {
		LOG("��ѯʧ��:" + sql);
		return nullptr;
	}
	return mysql_use_result(_conn);
}
	
