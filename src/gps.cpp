//============================================================================
// Name        : gps.cpp
// Author      : zhangshiguang
// Version     :
// Copyright   : All right reserved.
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include <locale.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>

#include<sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>
#include <syslog.h>
#include <semaphore.h>
#include <my_global.h>
#include <mysql.h>
using namespace std;
#include "g_api.h"

#include "become_daemon.h"
#include "log.h"
#include "dbdo.h"
#include "tools.h"
#include "CclientManager.h"
#include "SelEpolKqEvPrt.h"
#include "Cserver.h"
#include "CastUtil.h"
#include "api.h"
static volatile sig_atomic_t termReceived = 0;
static volatile sig_atomic_t hupmReceived = 0;
bool volatile allExitFlag = false; //exception exit
Ccfg g_cfg;

void sig_term(int signo)
{
	if (SIGTERM == signo) /* catched signal sent by kill(1) command */
	{
		//FLOG_MSG("correctgpsd get SIGTERM.");
		termReceived = 1;
		allExitFlag = true;
		DEBUGOUT("sig_term-SIGTERM-allExitFlag\n");
	}
	else if (SIGHUP == signo)
	{
		//FLOG_MSG("correctgpsd get SIGHUP.");
		hupmReceived = 1;
		allExitFlag = true;
		DEBUGOUT("sig_term-SIGHUP-allExitFlag\n");
	}
}
void setallthreadexitflag(int index)
{
	DEBUGOUT("setallthreadexitflag\n");
	FLOG_MSG("[setallthreadexitflag]%d.", index);
	allExitFlag = true;
}
int main(int argc, char *argv[])
{
	//const int SLEEP_TIME = 30; /*second*/

	log_open("[correctgps]", LOG_PID, LOG_USER);
	DEBUGOUT("[correctgps starting...]\n");
#ifdef MAKE_DAEMON
	if (becomeDaemon(BD_NO_CHDIR) == -1)
	{
		FLOG_ERR("[can't fork self]");
		closelog();
		exit(0);
	}
#endif
	//
	FLOG_MSG("correctgps started.");

	signal(SIGTERM, sig_term); /* arrange to catch the signal */
	signal(SIGHUP, sig_term);
	//signal(SIGPIPE,SIG_IGN);

	g_cfg.readConfigFile();

	//
	INFO_MAP mapinfo;

	//load db start/////////
	Cdbdo* doit = new Cdbdo();
	doit->DataBaseIni();
	doit->loaddb(&mapinfo);

	//load db end.//


	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int nfds;

	CclientManager clientmanager;
	clientmanager.setinfomap(&mapinfo);
	clientmanager.start();

	if (-1 != (sockfd = Cserver::createListener("", g_cfg.local_port, false)))
	{
		FLOG_MSG("listen %s ok", g_cfg.local_port.c_str());
		SelEpolKqEvPrt selEpolKqEvPrtHandler;
		selEpolKqEvPrtHandler.initialize(sockfd);

		while (!termReceived && !allExitFlag)
		{
			RESTART: nfds = selEpolKqEvPrtHandler.getEvents();
			if (-1 == nfds)
			{
				DEBUGOUT("poll_wait main process\n");
				FLOG_MSG("Interruption Signal Received.");
				if (errno == EBADF)
				{
					FLOG_MSG(".Inavlid fd.");
				}
				else if (errno == EFAULT)
				{
					FLOG_MSG(":The memory area pointed to by events is not accessible.");
				}
				else if (errno == EINVAL)
				{
					FLOG_MSG(":not a poll file descriptor, or maxevents is less than or equal to zero.");
				}
				else
				{
					FLOG_MSG(":not an epoll file descriptor.");
				}
			}
			////
			for (int n = 0; n < nfds; n++)
			{
				int descriptor = selEpolKqEvPrtHandler.getDescriptor(n);
				if (descriptor == sockfd)
				{
					new_fd = -1;
					sin_size = sizeof(their_addr);
					new_fd = accept(sockfd, (struct sockaddr *) &their_addr,
							&sin_size);
					if (new_fd == -1)
					{
						//FLOG_MSG("accept error[%d],continue.", new_fd);
						continue;
					}
					else
					{
						//FLOG_MSG("accept ok[%d].", new_fd);
						clientmanager.AddToQueue(new_fd);
						selEpolKqEvPrtHandler.reRegisterServerSock();
						if (!selEpolKqEvPrtHandler.registerForEvent(new_fd))
						{
							FLOG_MSG("registerForEvent(%d) fail", new_fd);
						}
					}
				}
				else if (descriptor != -1)
				{
					//FLOG_MSG("got new connection %s" , CastUtil::lexical_cast<string>(descriptor).c_str());
					selEpolKqEvPrtHandler.unRegisterForEvent(descriptor);
					epoll_event *ev = selEpolKqEvPrtHandler.getEpollEvent(n);
					if (ev->events & EPOLLIN) // read event
					{
						//FLOG_MSG("EPOLLIN==>[%d]==[%d]", ev->data.fd,descriptor);
						//assert(ev->data.fd == descriptor);
						clientmanager.DoRead(descriptor);
						selEpolKqEvPrtHandler.reRegisterServerSock();
						if (!selEpolKqEvPrtHandler.registerForEvent(descriptor))
						{
							FLOG_MSG("registerForEvent(%d) fail", descriptor);
						}
					}
					if (ev->events & EPOLLRDHUP || ev->events & EPOLLERR)
					{
						DEBUGOUT("EPOLLRDHUP|EPOLLERR\n");
						//FLOG_MSG("EPOLLRDHUP | EPOLLERR");
						selEpolKqEvPrtHandler.unRegisterForEvent(descriptor);
						clientmanager.DoKill(descriptor);
					}

				}
			}

			//sleep(SLEEP_TIME);
			if (hupmReceived)
			{
				hupmReceived = 0;

				sleep(1);//s
				goto RESTART;
			}
		}
	}
	DEBUGOUT("gps main end\n");
	FLOG_MSG("[correctgps]ended.");

	//logClose();

	return 0;
}

