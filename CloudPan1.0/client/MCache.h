/*************************************************************************
	> File Name: MCache.h
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年05月19日 星期四 20时12分57秒
 ************************************************************************/
#ifndef _MCACHED_H
#define _MCACHED_H

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <libmemcached/memcached.h>
using namespace std;

class MCache
{
public:
    MCache(const char *ip,int port)
    {
        memc = memcached_create(NULL);
        servers = memcached_server_list_append(NULL,ip,port,&rc);
        rc = memcached_server_push(memc,servers);
        memcached_server_free(servers);
    }
    ~MCache()
    {
        memcached_free(memc);
    }
    bool insertValue(const char *key,const char *value,int timeout)
    {
        rc = memcached_set(memc,key,strlen(key),value,strlen(value),(time_t)timeout,(uint32_t)0);
        if(rc == MEMCACHED_SUCCESS)
        {
            cout<<"Save key: "<<key<<"  data: "<<value<<endl;
        }else
        {
            return false;
        }
        return true;
    }
    char *getValue(const char *key)
    {
        if(NULL == key)
        {
            cout<<"key in empty"<<endl;
            return NULL;
        }
        time_t expiration = 3;
        size_t value_length;
        uint32_t flags;
        char *value = NULL;

        value = memcached_get(memc,key,strlen(key),&value_length,&flags,&rc);
        if(rc == MEMCACHED_SUCCESS)
        {
            cout<<"Get key: "<<key<<"  data:"<<value<<endl;
            return value;
        }
        return NULL;
    }
    bool deleteKey(const char *key)
    {
        rc = memcached_delete(memc,key,strlen(key),time_t(0));
        if(rc == MEMCACHED_SUCCESS)
        {
            cout<<"Delete key: "<<key<<endl;
            return true;
        }
        return false;
    }
private:
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *servers;
};

#endif
/*
int main(int argc, char *argv[]) 
{
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *servers;
    char value[8191];
//connect multi server
    memc = memcached_create(NULL);
    servers = memcached_server_list_append(NULL, "localhost", 11211, &rc);
    //servers = memcached_server_list_append(servers, "localhost", 11212, &rc);
    rc = memcached_server_push(memc, servers);
    memcached_server_free(servers);
//Save multi data
    size_t i;
    const char*keys[]= {"key1", "key2", "key3"};
    size_t key_length[]= {4, 4, 4};
    
    char*values[] = {"This is a first value", "This is a second value", "This is a third value"};
    size_t val_length[]= {21, 22, 21};
    for (i=0; i <3; i++) {
        rc = memcached_set(memc, keys[i],key_length[i], values[i], val_length[i], (time_t)180,(uint32_t)0);
        if (rc == MEMCACHED_SUCCESS) {
            printf("Save key:%s data:\"%s\" success.\n",keys[i], values[i]);
        }
    }
//Fetch multi data
    char return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    char *return_value;
    size_t return_value_length;
    uint32_t flags;
    rc = memcached_mget(memc, keys, key_length, 3);
    while ((return_value = memcached_fetch(memc, return_key,&return_key_length, &return_value_length, &flags, &rc))) {
        if (rc == MEMCACHED_SUCCESS) {
            printf("Fetch key:%s data:%s\n", return_key, return_value);
        }
    }
//Delete multi data
    for (i=0; i <3; i++) {
        rc = memcached_set(memc, keys[i], key_length[i],values[i], val_length[i], (time_t)180, (uint32_t)0);
        rc = memcached_delete(memc, keys[i],key_length[i], (time_t)0);
        if (rc == MEMCACHED_SUCCESS) {
            printf("Delete %s success\n", keys[i], values[i]);
        }
    }
//free
    memcached_free(memc);
    return 0;
}*/
