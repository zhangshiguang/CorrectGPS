/*
 * CclientManager.cpp
 *
 *  Created on: 2013-4-8
 *      Author: zr
 */
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "g_api.h"
#include "api.h"
#include "Thread.h"
#include "log.h"
#include "CClient.h"

#include "CclientManager.h"
#include "Ccorrectit.h"

//sem_t CclientManager::sem_notification;
pthread_mutex_t CclientManager::mmutex;

pthread_mutex_t CclientManager::mtx4cond;
bool CclientManager::test_cond = false;
pthread_cond_t CclientManager::cond;

pthread_mutex_t CclientManager::mtx4msgqueue;

CclientManager::CclientManager() :
	infomap(NULL)
{
	pthread_mutex_init(&CclientManager::mmutex, NULL);
	//sem_init(&CclientManager::sem_notification, 0, 0);

	pthread_mutex_init(&CclientManager::mtx4cond, NULL);
	pthread_cond_init(&CclientManager::cond, NULL);

	pthread_mutex_init(&CclientManager::mtx4msgqueue, NULL);

}

CclientManager::~CclientManager()
{

	CLIENT_MAP::iterator it = clientqueue.begin();
	for (it = clientqueue.begin(); it != clientqueue.end(); ++it)
	{
		delete (CClient*) it->second;
	}

	clientqueue.clear();

	msglist.clear();

	pthread_mutex_destroy(&CclientManager::mmutex);
	//sem_destroy(&CclientManager::sem_notification);

	pthread_mutex_destroy(&CclientManager::mtx4cond);
	pthread_cond_destroy(&CclientManager::cond);

	pthread_mutex_destroy(&CclientManager::mtx4msgqueue);
}
void CclientManager::setinfomap(INFO_MAP * pinfomap)
{
	this->infomap = pinfomap;
}
//when get imei ,add to map.
void CclientManager::AddToQueue(int fd)
{
	CClient* pclient = GetNewClient(fd);

	FLOG_MSG("[log][CClientManager::AddToQueue(%d)", fd);
	if (pclient)
	{//auto lock block
		CAutoLock Lock(&mmutex);

		CLIENT_MAP::iterator it = clientqueue.find(fd);
		if (it != clientqueue.end())
		{
			if (it->second)
			{
				delete it->second;
			}
			it->second = pclient;
		}
		else
		{
			clientqueue.insert(pair<int, CClient*> (fd, pclient));
			FLOG_MSG("[log][CClientManager::AddToQueue(%d) ok", fd);
		}

	}

}

//
void CclientManager::DoKill(int fd)
{
	FLOG_MSG("CclientManager::DoKill(%d)", fd);
	CAutoLock Lock(&mmutex);
	CLIENT_MAP::iterator it = clientqueue.find(fd);
	if (it != clientqueue.end())
	{
		delete it->second;
		it->second = NULL;
		clientqueue.erase(it);
	}

}

long CclientManager::GetActiveSize()
{
	return clientqueue.size();
}

CClient* CclientManager::GetNewClient(int fd)
{

	if (clientqueue.size() < g_cfg.max_clients)
	{
		CClient *client = new CClient(fd, this);
		if (!client)
		{
			setallthreadexitflag(4);
		}
		return client;
	}
	FLOG_MSG("[execption]CclientManager::GetNewClient(%d) fail", fd);
	return NULL;

}
/*
 CClient* CclientManager::GetClient(int fd)
 {
 CCLient * p = NULL;
 {
 CAutoLock Lock(&mutex);
 CLIENT_MAP::iterator it = clientqueue.find(fd);
 if (it != clientqueue.end())
 {
 p= it->second ;
 }
 }
 return p;
 }

 //±©Â¶žøÍâ²¿µ÷ÓÃµÄ¹Ø±ÕœÓ¿Ú


 void CclientManager::KillClient(CClient * p)
 {

 try
 {

 if (p == NULL)
 return;

 #if defined(_DEBUG)
 //              char debuginfo[200]={0};
 //              sprintf(debuginfo,"[log][CClientManager]KillClient(%s,%d)\n",p->serial,temp->_fd);
 //              DEBUGOUT(debuginfo);

 FLOG_MSG("[log][CClientManager]KillClient(%s,%d)\n",p->serial,p->_fd);
 #endif
 if (strlen(p->serial) > 0)
 {
 DeleteFromQueue(p->serial); ///
 }
 memset(p->serial, 0, sizeof(p->serial));
 memset(p->md5, 0, sizeof(p->md5));
 memset(p->xbaidu, 0, sizeof(p->xbaidu));
 memset(p->ybaidu, 0, sizeof(p->ybaidu));

 p->Close();

 } catch (...)
 {
 setallthreadexitflag(2);
 FLOG_MSG("[exception][CClientManager]KillClient!\n");
 }

 }

 */

CLIENT_MAP* CclientManager::getClientMap()
{
	return &clientqueue;
}

void CclientManager::start()
{
	Thread *pthread = NULL;
	for (unsigned int i = 0; i < g_cfg.max_worker; ++i)
	{
		pthread = new Thread(CclientManager::run, this);
		if (!pthread)
		{
			setallthreadexitflag(3);
		}
		else
		{
			pthread->execute();
			this->threadlist.push_back(pthread);
		}
	}
}
void CclientManager::stop()
{
	//sem_post(&CclientManager::sem_notification);
	FLOG_MSG("[log][CClientManager]stop.");
	Thread *pthread = NULL;
	while (!this->threadlist.empty())
	{
		pthread = this->threadlist.front();
		delete pthread;
		pthread = NULL;
		this->threadlist.pop_front();
	}
}
//不建议使用
void CclientManager::killTimeoutClient()
{
	//CAutoLock Lock(&mmutex);//when large clients ,会降低容量
	try
	{

		CLIENT_MAP::iterator it = clientqueue.begin();

		int ret = 0;
		//unsigned int op = 0;
		//int oplen = sizeof(int);

		while (it != clientqueue.end())
		{

			if (it->second && 0 == strlen(it->second->serial))
			{

				//ret = getsockopt(it->second->_fd, SOL_SOCKET, SO_CONNECT_TIME,reinterpret_cast<char*> (&op), &oplen);
				if (0 == ret)
				{
					if (g_cfg.outtime_s || ((unsigned int) (time(NULL)
							- it->second->delay_time) > g_cfg.outtime_s)) //Ãë
					{
#ifdef _DEBUG
						FLOG_MSG("[log][CClientManager]remove socket[%s]>%d.",
								it->second->serial, g_cfg.outtime_s);
#endif
						DoKill(it->first);

					}
				}
			}
			++it;
		}
	} catch (...)
	{
		setallthreadexitflag(301);
	}
}
//
//and notificate thread (to getgps and send to carserver and backfeed)
void CclientManager::notificationthreads(int fd)
{

	//notification
	//sem_post(&CclientManager::sem_notification);
	//add to list
	FLOG_MSG("CclientManager::notificationthreads(%d)", fd);
	pthread_mutex_lock(&CclientManager::mtx4msgqueue);
	msglist.push_back(fd);
	pthread_mutex_unlock(&CclientManager::mtx4msgqueue);
	//notification
	pthread_mutex_lock(&CclientManager::mtx4cond);
	CclientManager::test_cond = true;
	pthread_cond_signal(&CclientManager::cond);
	pthread_mutex_unlock(&CclientManager::mtx4cond);
	FLOG_MSG("CclientManager::notificationthreads ok");
}

void CclientManager::DoRead(int fd)
{
	FLOG_MSG("CclientManager::DoRead(%d)", fd);
	CLIENT_MAP::iterator it = clientqueue.find(fd);
	if (it != clientqueue.end())
	{
		CClient * client = it->second;
		if (client)
			client->receive();
	}

}
//
void* CclientManager::run(void *lpParam)
{

	int rc = ETIMEDOUT;
	int fd = INVALID_SOCKET;
	CclientManager* p = static_cast<CclientManager*> (lpParam);
	struct timespec ts;
	FLOG_MSG("CclientManager::run [%d] ok", pthread_self());
	while (!allExitFlag)
	{

		pthread_mutex_lock(&CclientManager::mtx4cond);
		while (!test_cond && (ETIMEDOUT == rc) && !allExitFlag)
		{
			ts.tv_sec = time(NULL) + 10;
			rc = pthread_cond_timedwait(&CclientManager::cond,
					&CclientManager::mtx4cond, &ts);
		}
		test_cond = false;
		pthread_mutex_unlock(&CclientManager::mtx4cond);
		FLOG_MSG("CclientManager::run %d", rc);
		rc = ETIMEDOUT;
		//get fd
		pthread_mutex_lock(&CclientManager::mtx4msgqueue);
		if (!p->msglist.empty())
		{
			fd = p->msglist.front();
			p->msglist.pop_front();
		}
		else
		{
			DEBUGOUT("msglist empty\n");
		}
		pthread_mutex_unlock(&CclientManager::mtx4msgqueue);
		FLOG_MSG("CclientManager::run get one");
		if (!allExitFlag)
		{

			try
			{
				CLIENT_MAP::iterator dev_it;
				CClient *pc = NULL;
				{
					CAutoLock Lock(&mmutex);

					dev_it = p->getClientMap()->find(fd);

					//FLOG_MSG("%d =p->getClientMap()->find(%d)", dev_it, fd);

					if (dev_it != p->getClientMap()->end()) //²éÕÒÉè±ž±í
					{

						Ccorrectit::correctgps_worker(dev_it->second,
								p->infomap);

						if (dev_it->second
								&& dev_it->second->GetSend2CarServer()
								&& strlen(dev_it->second->serial)) //client ŽæÔÚ,serialÓÐÖµ//¿ÉÄÜÒÑŸ­killed,»áÒì³£

						{
							//·¢ËÍžøÏàÓŠµÄ·þÎñÆ÷(client·¢ËÍ²¢œÓÊÕ·µ»ØjsonÊýŸÝ£¬×ª·¢žø¿Í»§¶Ë)
							dev_it->second->Send2CarServer();
							pc = dev_it->second;
						}//all client over

					}
				}
				if (pc)
					p->DoKill(pc->fd);
			} catch (...)
			{
				FLOG_MSG("[CclientManager run][exception 31].");
				setallthreadexitflag(300);
			}

		}
		else //if (!allExitFlag)
		{
			break;
		}

	}//while

	FLOG_MSG("[CClientManager]run exit.");
	return NULL;
}
