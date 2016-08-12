/*************************************************************************
	> File Name: fileTrans.cpp
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月22日 星期五 12时55分04秒
 ************************************************************************/

#include "md5.h"
#include "MCache.h"
#include "MyDB.h"
#include "fileTrans.h"

FileTrans::FileTrans(int port,const char *addr)
{
    if((trans_fd=socket(PF_INET,SOCK_STREAM,0)) < 0)
        ERR_EXIT("socket");
    memset(&seraddr,0,sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(port);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int on = 1;
    if(setsockopt(trans_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0 )
        ERR_EXIT("setsockopt");
    if(::bind(trans_fd,(struct sockaddr*)&seraddr,sizeof(seraddr)) < 0 )
        ERR_EXIT("bind");
    if(listen(trans_fd,SOMAXCONN) < 0)
        ERR_EXIT("listen");
}

FileTrans::~FileTrans()
{
    close(trans_fd);
    close(connfd);
}

void FileTrans::Run()
{
    struct epoll_event ev,events[EPOLL_SIZE];
    int epfd = epoll_create(EPOLL_SIZE);
    ev.events = EPOLLIN;
    ev.data.fd = trans_fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,trans_fd,&ev);
    
    while(1)
    {
        int events_count = epoll_wait(epfd,events,EPOLL_SIZE,-1);
        int i=0;
        
        for(;i<events_count;++i)
        {
            if(events[i].data.fd == trans_fd)
            {
                socklen_t peerlen = sizeof(peeraddr);
                pid_t pid;

                while((connfd = accept(trans_fd,(struct sockaddr *)&peeraddr,&peerlen)) > 0)
                {
                    cout<<"---------FileTrans-Server-----------"<<endl;

                    pid = fork();
                    if(pid == -1)
                        ERR_EXIT("FileTrans fork");
                    if(pid == 0)
                    {
                        close(trans_fd);
                        cout<<"Start FileTrans server"<<endl;
                        
                        int ret = read(connfd,(void*)msg.c_str(),1024);
                        if(ret == -1)
                            ERR_EXIT("read");
                        
                        getCmd();

                        write(connfd,(void*)msg.c_str(),1024); 
                        cout<<"end of FileTrans server"<<endl;
                        exit(EXIT_SUCCESS);
                    }
                    else
                        close(connfd);
                }
            }
        }

    }
}

void FileTrans::getCmd()
{
    //"OP_TYPE user_id file_name file_MD5 file_path file_size file_chunk_size chunk_size trans_status"
    char *ptr = (char*)msg.c_str();
    char *cmd = strtok(ptr," ");
    while(ptr=strtok(NULL," "))
    {
        //cout<<ptr<<endl;
        sql.push_back(ptr);
    }
    user_id = *(sql.begin());
    sql.pop_front();
    int select = atoi(cmd);
    switch(select)
    {
        case UPLODE:
            getFileInfo();
            recvFile();
            break;
        case DOWNLOAD:
            sendFileInfo();
            sendFile();
            break;
        case RM:
            removeFile(m_fileinfo.file_path);
            break;
        case LS:
            showFileList();
            break;
    }
}

void FileTrans::getFileInfo()
{
    m_fileinfo.file_name = *(sql.begin());
    sql.pop_front();
    m_fileinfo.file_MD5 = *(sql.begin());
    sql.pop_front();
    m_fileinfo.file_path=*(sql.begin());
    sql.pop_front();
    const char *cmd = (*(sql.begin())).c_str();
    m_fileinfo.file_size = atoi(cmd);
    sql.pop_front(); 
    cmd = (*(sql.begin())).c_str();
    m_fileinfo.file_chunk_size = atoi(cmd);
    sql.pop_front();
    cmd = (*(sql.begin())).c_str();
    m_fileinfo.chunk_size = atoi(cmd);
    sql.pop_front();
    m_fileinfo.trans_status = false;
}
void FileTrans::sendFileInfo()
{
    MyDB *db = new MyDB();
    if(!db->initDB(SERADDR,DBUSER,DBPSSW,DBNAME))
    {
        msg = "ser error(mysql connecting)";
        return;
    }
    string selectSql = "select *from files where user_id="+user_id+" and file_path="+m_fileinfo.file_path; 
    db->execSQL(selectSql);
    sql = db->getResult();
    getFileInfo();
    write(connfd,&m_fileinfo,sizeof(m_fileinfo));
}
bool FileTrans::recvFile()
{
    //秒传的实现：检查以文件MD5为key的value是否存在mcached中
    MCache mem(SERADDR,MCA);
    MyDB *db = new MyDB();
    if(mem.getValue(m_fileinfo.file_MD5.c_str()) == NULL)
    {
        if(createFile() == false)
            ERR_EXIT("open file");
        //创建文件成功，匹配MD5，需要给出存储文件的全路径名
        if(strncmp((m_fileinfo.file_MD5).c_str(),(MD5(m_fileinfo.file_path).toString()).c_str(),32) != 0)
        {
            ERR_EXIT("File md5 error");
        }
        m_fileinfo.trans_status = true;
        //文件真正传输成功，存入memcached
        //以文件MD5值作为key,文件名作为value存入memcached
        mem.insertValue((m_fileinfo.file_MD5).c_str(),(m_fileinfo.file_name).c_str(),180);
    }
    //存入mysql
    if(!db->initDB(SERADDR,DBUSER,DBPSSW,DBNAME))
    {
        msg = "ser error(mysql connecting)";
    }
    stringstream ss;
    string tmp;
    string status;
    ss<<m_fileinfo.file_size;
    ss>>tmp;
    if(m_fileinfo.trans_status)
        status = "ok";
    else
        status = "er";
    string insertSql = "insert into files values("+m_fileinfo.file_name+","+user_id+","+m_fileinfo.file_MD5+","+m_fileinfo.file_path+","+tmp+","+status+");";
    db->execSQL(insertSql);
    
    msg=*(db->getResult()).begin();
    if(strncmp("1",msg.c_str(),1) == 0)
    {
        msg = "file insert to mysql error!";
    }
    return true;
}

bool FileTrans::sendFile()
{
    int fd = open((m_fileinfo.file_path).c_str(), O_RDONLY);
    if(fd < 0)
    {
        //ERR_EXIT("open file");
        cout<<"open file false"<<endl;
        return false;
    }
    struct stat statBuf;
    fstat(fd, &statBuf);
    sendfile(connfd, fd, NULL, statBuf.st_size);
    return true;
}

bool FileTrans::removeFile(string path)
{
    MyDB *db = new MyDB();
    if(!db->initDB(SERADDR,DBUSER,DBPSSW,DBNAME))
    {
        msg = "ser error(mysql connecting)";
        return false;
    }
    //查询user_id是否拥有path文件
    string selectSql="select count(*) from files where user_id="+user_id+"and file_path="+path ;
    db->execSQL(selectSql);
    msg = *(db->getResult()).begin();
    if(strncmp("1",msg.c_str(),1) != 0)
    {
        msg = "The user does not have this file";
        return false;
    }
    //删除memcached记录,需要从mysql查询key值，即file_MD5
    selectSql = "select file_MD5 from files where user_id="+user_id+" and file_path="+path;
    db->execSQL(selectSql);

    MCache mem(SERADDR,MCA);
    mem.deleteKey((*(db->getResult()).begin()).c_str());
    //删除mysql记录
    selectSql = "delete from files where user_id="+user_id+"and file_path="+path;
    db->execSQL(selectSql);
    
    return true;
}

bool FileTrans::createFile()
{
    int nbytes = -1;
    //while (true)
    //{
        int fd = open((m_fileinfo.file_path).c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0666);
        if (fd == -1)
        {
            ERR_EXIT("open file error");
            return false;
        }
        //recive file
        char recvBuf[BUFFSIZE]={0};
        while((nbytes=recv(trans_fd,recvBuf,64,0)) > 0)
        {
            int writeBytes = write(fd,recvBuf,nbytes);
            if(writeBytes != nbytes)
            {
                ERR_EXIT("write file error");
                return false;
            }
            if(nbytes < 64)
            {
                break;
            }
        }
    //}
    return true;
}

void FileTrans::showFileList()
{
    MyDB *db = new MyDB();
    if(!db->initDB(SERADDR,DBUSER,DBPSSW,DBNAME))
    {
        msg = "ser error(mysql connecting)";
        return;
    }
    //查询user_id的所有文件名
    string selectSql="select file_name from files where user_id="+user_id;
    db->execSQL(selectSql);
    list<string> res=db->getResult();
    //msg保存查询出来的文件名，以" "分开，传回客户端
    list<string>::iterator it;
    for(it=res.begin();it!=res.end();++it)
    {
        msg += *it;
        msg += " ";
    }
}

int main()
{
    FileTrans ft(TPT,SERADDR);
    ft.Run();
    return 0;
}
