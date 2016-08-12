/*************************************************************************
	> File Name: sql_conn_pool.h
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月23日 星期六 15时02分26秒
 ************************************************************************/
#ifndef CONN_POOL_H
#define CONN_POOL_H

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <boost/thread.hpp>
#include <deque>
#include "mysql/mysql.h"
using namespace std;

typedef struct sqlConnInfo_
{
	int max_conn_num;
	std::string ip;
	std::string port;
	std::string user_name;
	std::string password;
} SqlConnInfo;

class CSqlConnPool
{
public:
	CSqlConnPool();
	~CSqlConnPool();

	void Init(const SqlConnInfo& sqlConnInfo);
	void Destroy();

	void SetDBName(const char* DBName);

	sql::Connection*GetConnection();
	void ReleaseConnection(sql::Connection * conn);

private:

	SqlConnInfo sqlConnInfo_;

	boost::mutex mutex_;
	sql::Driver* driver_;
	const char*  DBName_;
	deque<sql::Connection *> queue_conn_;

	sql::Connection * CreateConnection();
	void TerminateConnection(sql::Connection * conn);
	bool IsValidConnection(sql::Connection *conn); //判断数据库连接是否有效（为了解决8小时连接失效问题)
};

#endif
