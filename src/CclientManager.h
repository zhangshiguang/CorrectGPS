/*
 * CclientManager.h
 *
 *  Created on: 2013-4-8
 *      Author: zr
 */

#ifndef CCLIENTMANAGER_H_
#define CCLIENTMANAGER_H_
#include <semaphore.h>
#include <list>
//#include <sys/types>
//#include <sys/ipc.h>
//#include <sys/msg.h>
//#include <sys/stat.h>
#include "api.h"

class Ccfg;
class CClient;

class Thread;

class CclientManager
    {
public:
    CclientManager();
    virtual ~CclientManager();

    void AddToQueue(int fd);

    long GetActiveSize();
    CClient*GetNewClient(int fd);
    //CClient* GetClient();
    void PushNewIn(CClient *p);


    void start();
    void stop();
    void DoRead(int fd);
    void DoKill(int fd);
    void notificationthreads(int fd);
    void killTimeoutClient();
    CLIENT_MAP* getClientMap();
    void setinfomap(INFO_MAP * pinfomap);

private:

    static pthread_mutex_t mmutex; //for clientqueue
    std::list<Thread *> threadlist; //工作线程，个数由配置文件定义

public:
    INFO_MAP * infomap;

    CLIENT_MAP clientqueue;
    //static sem_t sem_notification; //notificate from request to correcting
    //各个线程见通过此条件变量来获得处理权
    static bool    test_cond;
    static  pthread_cond_t cond;
    static  pthread_mutex_t mtx4cond;
    //获得处理权的线程通过此互斥访问信息
    static  pthread_mutex_t mtx4msgqueue;
    std::list<int> msglist;      //完整请求的socket队列 //int okfd; //完整请求的socket fd

    static void* run(void * param);
    };

#endif /* CCLIENTMANAGER_H_ */
