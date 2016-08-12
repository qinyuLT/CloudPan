/*************************************************************************
	> File Name: conSer.cpp
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月20日 星期三 22时38分48秒
 ************************************************************************/
#include "MCache.h"
#include "conSer.h"
    conSer::conSer(int port,const char *addr)
    {
        if((listenfd=socket(PF_INET,SOCK_STREAM,0)) < 0)
            ERR_EXIT("socket");
        memset(&seraddr,0,sizeof(seraddr));
        seraddr.sin_family = AF_INET;
        seraddr.sin_port = htons(port);
        seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    conSer::~conSer(){}//delete db;}
    
    void conSer::Socket()
    {
	    int on = 1;
        if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0 )
            ERR_EXIT("setsockopt");
    }
    
    void conSer::Bind()
    {
        if(::bind(listenfd,(struct sockaddr*)&seraddr,sizeof(seraddr)) < 0 )
            ERR_EXIT("bind");
    }
    
    void conSer::Listen()
    {
        if(listen(listenfd,SOMAXCONN) < 0)
            ERR_EXIT("listen");
    }

    //采用select方法:
    /*int conSer::accept_timeout(unsigned int wait_seconds)
    {
        int ret;
	    socklen_t peerlen = sizeof(peeraddr);
        
        if(wait_seconds > 0)
        {
            fd_set accept_fdset;
            struct timeval timeout;
            FD_ZERO(&accept_fdset);
            FD_SET(listenfd,&accept_fdset);
            timeout.tv_sec = wait_seconds;
            timeout.tv_usec = 0;

            do{
                ret = select(listenfd+1,&accept_fdset,NULL,NULL,&timeout);
            }while(ret<0 && errno == EINTR);
            
            if(ret == -1)
            {
                return -1;
            }
            else if(ret == 0)
            {
                errno = ETIMEDOUT;
                return -1;
            }
        }

        if(&peeraddr != NULL)
            ret = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen);
        else
            ret = accept(listenfd,NULL,NULL);

        return ret;
    }
    */

    //int conSer::accept_timeout(unsigned int wait_seconds)
    //采用epoll方法:
    void conSer::epoll_work()
    {
        struct epoll_event ev,events[EPOLL_SIZE];
        int epfd = epoll_create(EPOLL_SIZE);
        ev.events = EPOLLIN;
        ev.data.fd = listenfd;
        epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

        while(1)
        {
            int events_count = epoll_wait(epfd,events,EPOLL_SIZE,-1);
            int i=0;

            for(;i<events_count;++i)
            {
                if(events[i].data.fd == listenfd)
                {
                    int conn;
	                socklen_t peerlen = sizeof(peeraddr);
                    pid_t pid;
                    
                    system("clear");
                    while((conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen))>0)
                    {
                        cout<<endl<<"=============== Service ================"<<endl;
                        cout<<" EPOLL:Received New Connection Request "<<endl;
                        cout<<"  confd="<<conn;
                        cout<<"  ip="<<inet_ntoa(peeraddr.sin_addr);
                        cout<<"  port="<<ntohs(peeraddr.sin_port)<<endl;
                        cout<<"========================================"<<endl;
                        
                        pid = fork();
                        if(pid == -1)
                            ERR_EXIT("fork");
           
                        if(pid == 0)
                        {
                            close(listenfd);
                            cout<<"Start do_service"<<endl;
                
                            do_service(conn);

                            cout<<"End of do_service"<<endl;
                            cout<<"========================================"<<endl;
                            exit(EXIT_SUCCESS);
                        }else
                            close(conn);    
                    }
                }
            }
        }
    }

    void conSer::Run()
    {
        Socket();
        Bind();
	    Listen();
        epoll_work();
	    /*
        //socklen_t peerlen = sizeof(peeraddr);
        int conn;
        pid_t pid;

        while(1)
        {
            //if((conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen)) < 0)
            //    ERR_EXIT("accept");
	        //cout<<"ip="<<inet_ntoa(peeraddr.sin_addr)<<" port="<<ntohs(peeraddr.sin_port)<<endl;
            
            conn = accept_timeout(0);
            if(conn == -1)
                ERR_EXIT("accept_timeout");
	        cout<<"ip="<<inet_ntoa(peeraddr.sin_addr)<<" port="<<ntohs(peeraddr.sin_port)<<endl;

            pid = fork();
            if(pid == -1)
                ERR_EXIT("fork");
           
            if(pid == 0)
            {
                close(listenfd);
                cout<<"enter do_service"<<endl;
                
                do_service(conn);

                cout<<"end of do_service"<<endl;
                exit(EXIT_SUCCESS);
            }else
                close(conn);
        }*/
    }

    void conSer::do_service(int conn)
    {
        while(1)
        {
            int ret = read(conn,(void *)msg.c_str(),1024);
            //cout<<"ret:"<<ret<<" msg:"<<msg.c_str()<<endl;
            get_cmd();
            //msg = "get it!";
            //cout<<"the msg= "<<msg<<endl;
            write(conn,(void*)msg.c_str(),msg.size());
            if(ret != -1)
                break;
            //while(1){}
        }
    }

    /*
    void sendToDB()
    {
        MyDB db;
        list<string>::iterator it=sql.pop_back();
        char *user = *it;
        db.initDB("localhost","root","qinyu","FTP");
        db.execSQL("select password from user where user_id = 'qinyu'");
    }*/

    void conSer::selectFTServer()
    {
        string FTS_IP = SERADDR;
        int FTS_PORT = TPT;
        stringstream ss;
        string port;
        ss<<FTS_PORT;
        ss>>port;
        msg += FTS_IP+" "+port;
    }
    
    void conSer::get_cmd()
    {
        char *ptr = (char*)msg.c_str();
        char *cmd = strtok(ptr," ");
        string sqlExec;
        int select;
        //cout<<cmd<<endl;
        //sql.push_back(cmd);
        while(ptr=strtok(NULL," "))
        {
            //cout<<ptr<<endl;
            sql.push_back(ptr);
        }

        MyDB *db = new MyDB();
        if(!db->initDB(SERADDR,DBUSER,DBPSSW,DBNAME))
        {
            msg = "ser error(mysql connecting)";
            select = -1;
        }else
        {
            select = atoi(cmd);
        }

	    string userid=*(sql.begin());
	    sql.pop_front();
	    string passwd=*(sql.begin());
	    sql.pop_front();

        MCache mem(SERADDR,MCA);
        //mem.insertValue(userid.c_str(),passwd.c_str(),180);

        //cout<<"user:"<<userid<<" "<<"password:"<<passwd<<endl;
        switch(select)
        {
        //登录：加入memcached后，不需要从数据库中查询
        case ENTER:
            /*//cout<<select<<" enter"<<endl;
	        //select exists(select *from user where user_id='用户名' and password='密码');
	        sqlExec="select exists(select *from user where user_id='" +userid + "' and password='"+passwd+"')";
            db->execSQL(sqlExec);
            msg=*(db->getResult()).begin(); 
            */
            //MCache mem(SERADDR,MCA);
            if(strncmp(mem.getValue(userid.c_str()),passwd.c_str(),32) == 0)
            {
                msg = "1";
            }else
            {
                msg = "userid or passwd error!";
            }
            break;

        //注册：插入mysql数据库并且插入到memcached缓存中
        case REGISTER:
	        //insert into user(user_id,password) select '用户名','密码' from dual where not exists(select *from user where user.user_id='用户名');
            sqlExec="select exists (select *from user where user_id='"+userid+"')";
            db->execSQL(sqlExec);
            msg=*(db->getResult()).begin();
            if(strncmp("1",msg.c_str(),1) == 0)
            {
                msg = "The user existed!";
            }else{

                sqlExec="insert into user(user_id,password) select '"+userid+"','"+passwd+"' from dual where not exists(select *from user where user.user_id='"+userid+"')";
                db->execSQL(sqlExec);
                msg = "register success!";
            }
            //插入memcached
            //MCache mem(SERADDR,MCA);
            mem.insertValue(userid.c_str(),passwd.c_str(),180);
            break;

        case UPLODE:
            msg = "3 ";
            selectFTServer();
            break;

        case DOWNLOAD:
            msg = "4 ";
            selectFTServer();
            break;

        case CHART:
            //msg = "5 ";
            //selectCServer();
            break;

            break;
        }
    }
    

int main()
{
    conSer ser(CPT,SERADDR);
    ser.Run();

    return 0;
}
