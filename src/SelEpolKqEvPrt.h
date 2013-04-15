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
 * SelEpolKqEvPrt.h
 *
 *  Created on: 30-Dec-2012
 *      Author: sumeetc
 */

#ifndef SELEPOLKQEVPRT_H_
#define SELEPOLKQEVPRT_H_
#include <sys/types.h>
#include "g_api.h"
//#include "Logger.h"
#include "cstring"
#include <fcntl.h>
#include <sys/time.h>
#include <sys/epoll.h>
#define MAXDESCRIPTORS 1024

class SelEpolKqEvPrt {
	//Logger logger;
	int mode;
	int sockfd;
	int curfds;

	#ifdef USE_EPOLL
		struct epoll_event ev;
		struct epoll_event events[MAXDESCRIPTORS];
		int epoll_handle;
	#endif


public:
	SelEpolKqEvPrt();
	virtual ~SelEpolKqEvPrt();
	void initialize(int sockfd);
	int getEvents();
	int getDescriptor(int index);
	bool isListeningDescriptor(int descriptor);
	bool registerForEvent(int descriptor);
	bool unRegisterForEvent(int descriptor);
	void reRegisterServerSock();
	epoll_event* getEpollEvent(int index);
};

#endif /* SELEPOLKQEVPRT_H_ */
