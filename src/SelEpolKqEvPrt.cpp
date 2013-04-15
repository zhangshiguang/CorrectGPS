/*
 Copyright 2009-2013, Sumeet Chhetri

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */
/*
 * SelEpolKqEvPrt.cpp
 *
 *  Created on: 30-Dec-2012
 *      Author: sumeetc
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <errno.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include "api.h"
#include "log.h"
#include "SelEpolKqEvPrt.h"

SelEpolKqEvPrt::SelEpolKqEvPrt()
    {
    //logger = Logger::getLogger("SelEpolKqEvPrt");
    }

SelEpolKqEvPrt::~SelEpolKqEvPrt()
    {
    }

void SelEpolKqEvPrt::initialize(int sockfd)
    {
    this->sockfd = sockfd;
    curfds = 1;

#ifdef USE_EPOLL
    if (-1 == (epoll_handle = epoll_create(MAXDESCRIPTORS)))
	{
	perror("epoll_create");
	FLOG_MSG("epoll create error.");
	setallthreadexitflag(106);
	}
    else
	{
	FLOG_MSG("epoll create ok.");
	}
#endif

    if (registerForEvent(this->sockfd))
	{
	FLOG_MSG("registerForEvent(%d) ok.", this->sockfd);
	}
    }

int SelEpolKqEvPrt::getEvents()
    {
    int numEvents = -1;

#ifdef USE_EPOLL
    while (!allExitFlag)
	{
	if (0 > (numEvents = epoll_wait(epoll_handle, events, curfds, 1000)))
	    {
	    if (errno == EINTR)
		{
		continue;
		}
	    else
		{
		perror("epoll_wait");
		FLOG_MSG("epoll wait error.");
		}
	    }

	break;
	}
#endif

    return numEvents;
    }
epoll_event* SelEpolKqEvPrt::getEpollEvent(int index)
    {
    if (index > -1 && index < MAXDESCRIPTORS)
	{
	return &events[index];
	}
    return NULL;
    }
int SelEpolKqEvPrt::getDescriptor(int index)
    {

#ifdef USE_EPOLL
    if (index > -1 && index < MAXDESCRIPTORS)
	{
	return events[index].data.fd;
	}
#endif

    return -1;
    }

bool SelEpolKqEvPrt::isListeningDescriptor(int descriptor)
    {
    if (descriptor == sockfd)
	{
	return true;
	}
    return false;
    }

bool SelEpolKqEvPrt::registerForEvent(int descriptor)
    {

    fcntl(descriptor, F_SETFL, fcntl(descriptor, F_GETFD, 0) | O_NONBLOCK);

    linger lin;
    unsigned int y = sizeof(lin);
    lin.l_onoff = 1;
    lin.l_linger = 2;

    // linger option  //windows µ ;linger time, POSIX specifies units as seconds//

    if (setsockopt(descriptor, SOL_SOCKET, SO_LINGER, (void*) (&lin), y) < 0)
	{
	perror("SelEpolKqEvPrt 1 setsockopt()");
	close(descriptor);
	//setallthreadexitflag(100);
	return false;
	}

    struct timeval optval;
    optval.tv_sec = 1000;
    optval.tv_usec = 0;
    if (setsockopt(descriptor, SOL_SOCKET, SO_RCVTIMEO, &optval, sizeof(optval))
	    < 0)
	{
	perror("SelEpolKqEvPrt 2 setsockopt()");
	close(descriptor);
	//setallthreadexitflag(101);
	return false;
	}

    /*
     * optval = 1;
     if (setsockopt(sckClient, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval))< 0)
     {
     perror("setsockopt()");
     close( sckClient);
     return false;
     }
     if (setsockopt(sckClient, SOL_TCP, TCP_KEEPINTVL, &optval, sizeof(optval))< 0)
     {
     perror("setsockopt()");
     close( sckClient);
     return false;
     }
     */
#ifdef USE_EPOLL
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET;
    ev.data.fd = descriptor;
    if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, descriptor, &ev) < 0)
	{
	perror("epoll");
	FLOG_MSG("Error adding to epoll cntl list.");
	//setallthreadexitflag(103);
	return false;
	}
    else
	{
	++curfds;
	}
#endif

    return true;
    }

bool SelEpolKqEvPrt::unRegisterForEvent(int descriptor)
    {
    if (descriptor <= 0)
	return false;

#ifdef USE_EPOLL
    if (0 == epoll_ctl(epoll_handle, EPOLL_CTL_DEL, descriptor, &ev))
	{
	--curfds;
	}
#endif

    return true;
    }

void SelEpolKqEvPrt::reRegisterServerSock()
    {
#ifdef USE_EVPORT
    if (port_associate(port, PORT_SOURCE_FD, sockfd, POLLIN, NULL) < 0)
	{
	perror("port_associate");
	setallthreadexitflag(111);
	}
#endif
    }
