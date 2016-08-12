/*************************************************************************
	> File Name: testser.cpp
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月19日 星期二 23时11分23秒
 ************************************************************************/
#include "conSer.h"
#include "MyDB.h"
#include <iostream>
using namespace std;

void do_cmd()
{
    //ser.sql.p    
    int select = 1;//atoi(cmd);
        
    switch(select)
    {
    case ENTER:
        //cout<<select<<" enter"<<endl;
        //sendToDB();
        break;

    case REGISTER:
        cout<<select<<" register"<<endl;
        break;

    case UPLODE:
        break;
    case DOWNLOAD:
        break;

    case CHART:
        break;
        
    default:
        break;
    }
}


int main()
{
    conSer ser(22222,"127.0.0.1");
    ser.Run();
    
    /*list<string>::iterator it;
    for(it=ser.sql.begin();it!=ser.sql.end();++it)
    {
        cout<<*it<<" ";
    }
    cout<<endl;

    MyDB db;
    db.initDB("localhost","root","qinyu","FTP");
    db.execSQL("select *from user");
    */

    //conCli cli(22222,"127.0.0.1");
    //cli.Run();
    //cli.do_work();

    return 0;
}
