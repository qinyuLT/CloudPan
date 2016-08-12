/*************************************************************************
	> File Name: mysql.cpp
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月19日 星期二 23时10分07秒
 ************************************************************************/

#include "mysql.h"
#include <iostream>
using namespace std;

MyDB::MyDB()
{
    connection = mysql_init(NULL);
    if(connection == NULL)
    {
        cout<<"Errorr"<<mysql_error(connection)<<endl;
        exit(1);
    }
}
MyDB::~MyDB()
{
    if(connection != NULL)
    {
        mysql_close(connection);
    }
}

bool MyDB::initDB(string host,string user,string password,string db_name)
{
    connection = mysql_real_connect(connection,host.c_str(),user.c_str(),password.c_str(),db_name.c_str(),0,NULL,0);
    if(connection == NULL)
    {
        cout<<"Error"<<mysql_error(connection)<<endl;
        exit(1);
    }
    return true;
}

bool MyDB::execSQL(string sql)
{
    int i,j;
    if(mysql_query(connection,sql.c_str()))
    {
        cout<<"Query Error"<<mysql_error(connection)<<endl;
        exit(1);
    }else
    {
        result = mysql_use_result(connection);//获取结果集
        //result = mysql_store_result(connection);
        /*while(row = mysql_fetch_row(result))
        {
            for(i=0;i<mysql_num_fields(result);++i)
            {
                cout<< row[i]<<" ";
            }
            cout<<endl;
        }*/
        for(i=0;i<mysql_field_count(connection);++i)
        {
            //获取下一行
            row = mysql_fetch_row(result);
            if(row <= 0)
                break;
            for(j=0;j<mysql_num_fields(result);++j)
            {
                cout<<row[j]<<" ";
            }
            cout<<endl;
        }
        //释放结果集的内存
        mysql_free_result(result);
    }
    return true;
}

int main()
{
    MyDB db;
    db.initDB("localhost","root","qinyu","FTP");
    cout<<"查询表user的信息"<<endl;
    db.execSQL("select * from user;");
    cout<<"查找用户‘qinyu’的文件列表"<<endl;
    db.execSQL("select user_id,filename,size,type from user,files where user.user_id='qinyu' and user.file_id=files.id;");
    cout<<"查找用户‘qiny’的好友列表"<<endl;
    db.execSQL("select user.user_id,friends.user_id from user,friends where user.user_id='qinyu' and user.friends_id=friends.id;");
    return 0;
}
