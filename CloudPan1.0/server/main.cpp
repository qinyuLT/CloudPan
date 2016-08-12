/*************************************************************************
	> File Name: main.cpp
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月19日 星期二 22时59分09秒
 ************************************************************************/
//#include "conSer.h"
//#include "conCli.h"
//#include "MyDB.h"
//#include "md5.h"
//#include "tPool.h"
#include "MCache.h"
#include <iostream>
using namespace std;

int main()
{
    MCache mem("127.0.0.1",11211);
    mem.insertValue("qinyu","123123",180);
    cout<<mem.getValue("qinyu")<<endl;
    mem.deleteKey("qinyu");

    /*
    if(createPool(4) != 0)
    {
        cout<<"tpool_create failed"<<endl;
        return -1;
    }
    cout<<"-- Thread Pool start --"<<endl;
    */

    //cout<<MD5("111111").toString()<<endl;
    //conSer ser(22222,"127.0.0.1");
    //ser.Run();

    //conCli cli(22222,"127.0.0.1");
    //cli.Run();
    //cli.do_work();
/*
    MyDB db;
    db.initDB("localhost","root","qinyu","FTP");
    string userid="qinyu";
    string password="222222";
    //string sqlExec="select *from user";
    string sqlExec="insert into user(user_id,password) select '"+userid+"','"+password+"' from dual where not exists(select *from user where user.user_id='"+userid+"')";        
    db.execSQL(sqlExec);
    cout<<"============================="<<endl;
    db.showResult();
*/
    return 0;
}
