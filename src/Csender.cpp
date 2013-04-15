/*
 * Csender.cpp
 *
 *  Created on: 2013-4-8
 *      Author: zsg
 */
#include <errno.h>
#include <string.h>
#include "g_api.h"
#include "api.h"
#include "log.h"
#include "Csender.h"

Csender::Csender() {
	// TODO Auto-generated constructor stub

}

Csender::~Csender() {
	// TODO Auto-generated destructor stub
}
// Sender1.cpp: implementation of the CSender class.
// Œì²éclientManagerµÄËùÓÐClient£¬Èç¹ûÒÑŸ­ŸÀÆ«¹ý£¬
//
//////////////////////////////////////////////////////////////////////
void Csender::Close()
{
	if(m_sockfd != INVALID_SOCKET)
	{
		close(m_sockfd);
		m_sockfd = INVALID_SOCKET;
	}
}
bool Csender::Connect()
{
	DEBUGOUT("Csender::Connect\n");
	try
	{
		struct sockaddr_in servaddr;

		m_sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(INVALID_SOCKET ==m_sockfd)
		{
		//#ifdef _DEBUG
			perror("Csender::Connect socket error!");
			FLOG_MSG("Csender::Connect socket error!");
		//#endif
			//setallthreadexitflag(500);
			return false;
		}

#ifdef NONEBLOCK_SOCKET
		fcntl(m_sockfd, F_SETFL, fcntl(m_sockfd, F_GETFD, 0) | O_NONBLOCK);
#else
		SetRecvTimeOut(5000);
		SetSendTimeOut(5000);
#endif

		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr =inet_addr( g_cfg.carserver_ip.c_str() );// Ïà·ŽÔòÊÇinet_ntoa(ina.sin_addr)
		servaddr.sin_port = htons( g_cfg.carserver_port );


#ifdef _DEBUG
		FLOG_MSG("connecting...%s:%d",g_cfg.carserver_ip.c_str(),g_cfg.carserver_port);
#endif
#ifdef NONEBLOCK_SOCKET
		/////
		fd_set fdw,fdr,fde;



		struct timeval tv;

		// block for 10 ms to keep from eating up all cpu time
		tv.tv_sec = 0;
		tv.tv_usec = 350000;

		int retsoc=0;

		int err = connect(m_sockfd, (struct sockaddr*) &servaddr,sizeof(servaddr));

		if(SOCKET_ERROR == err)
		{
			switch(	errno)
			{
			case EINPROGRESS: // //windows :WSAEWOULDBLOCK:
				//getpeername
				while(1)
				{
				    FD_ZERO( &fdr );
				    FD_ZERO( &fdw );
				    FD_ZERO( &fde );
				    FD_SET( m_sockfd, &fdr );
				    FD_SET( m_sockfd, &fdw );
				    FD_SET( m_sockfd, &fde );

					retsoc=select( m_sockfd+1,&fdr,&fdw, &fde, &tv );

					if(0==retsoc)    //TIMEOUT
					{
						continue;
					}
					else if(0<retsoc)
					{
						int error = 0;
						socklen_t sz = sizeof(error); /* even if we get -1 */
						int code =0;
						if( FD_ISSET( m_sockfd, &fdw ) ) ////ÅÐ¶ÏŸä±ú¿ÉÐŽ£¬²»ÄÜŽú±íœšÁ¢Á¬œÓ³É¹Š¡£
						{
							code = getsockopt(m_sockfd,SOL_SOCKET,SO_ERROR,&error, &sz);
							if ((code < 0) || error)
							{
								return false;
							}
							else
							{
								return true;
							}
						}
						else if (FD_ISSET( m_sockfd, &fde ))//exception
						{
							return false;
						}
						else
						{
							continue;
						}

					}
					else
					{
						return false;
					}
				}
				break;
			default:
				return false;
			}


		}
#else
		int err = connect(m_sockfd, (struct sockaddr*) &servaddr,sizeof(servaddr));

		if(SOCKET_ERROR == err)
		{
			FLOG_MSG("[err][connectToServer]connect Error\n");
			return false;
		}
		else
		{
		#ifdef _DEBUG
			//FLOG_MSG("Csender::Connect socket ok!\n");
		#endif

		}
#endif

	}
	catch(...)
	{
		DEBUGOUT("Csender::Connect except\n");
		FLOG_MSG("Csender::Connect except!");
	}

	return true;
}
bool Csender::SetRecvTimeOut(int t)
{
	int timeout = t; //ms
	int res = setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout));
	if(res == 0) return true;
	return false;
}
bool Csender::SetSendTimeOut(int t)
{
	int timeout = t; //ms
	int res = setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout));
	if(res == 0) return true;
	return false;
}
//GET http://58.61.160.95:81/api/navigate.api.php?serial= &md5= &xgps= &ygps=
//static const char *  h ="HTTP/1.1 200 OK\r\nContent-Type: text/html;Charset=UTF-8\r\nContent-Length: ";
//static const char *  x ="\r\n\r\n";

int  Csender::Send(CClient* obj)
{
	DEBUGOUT("Csender::Send\n");
	//ÓŠŽðµÄÐŽ·š
	 // 	char temp[8]={0};
	//
	// 	string bd(h);
	//
	// 	bd.append(itoa(len,temp ,10));
	// 	bd.append(x);
	// 	bd.append();

	//ÇëÇóµÄÐŽ·š
	char data[1024];
sprintf(data,"GET http://%s:%d/api/navigate.api.php?serial=%s&md5=%s&xgps=%.8f&ygps=%.8f HTTP/1.1\r\n\
Host: %s:%d\r\nConnection: keep-alive\r\nAccept: text/html;charset=utf-8\r\n \
User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.22 (KHTML, like Gecko) Chrome/25.0.1364.172 Safari/537.22 \
Accept-Encoding: gzip,deflate \
Accept-Language: zh;q=0.8,en-US;q=0.6,en;q=0.4 \
Accept-Charset: utf-8;q=0.7,*;q=0.3\r\n\r\n",
	g_cfg.carserv_domainname.c_str(),g_cfg.carserver_port,obj->serial,obj->md5,obj->xgps,obj->ygps,
		g_cfg.carserv_domainname.c_str(),g_cfg.carserver_port);

	int ret = send(m_sockfd, data, strlen(data), 0);

// 	string log;
// 	if(ret !=  SOCKET_ERROR)
// 	{
// 		log=("[log][CUpSender]tocarserver[");
// 	}
// 	else if (ret >0 && ret < strlen(data))
// 	{
// 		log=("[log][CUpSender]tocarserver[fail][");
// 	}
// 	log.append("[r=");
// 	log.append(itoa(ret,temp ,10));
// 	log.append("]");
// 	log.append(data);
// 	g_log.log(log.c_str(),LOG_LOG_LEVEL);
#ifdef _DEBUG
	FLOG_MSG("[log][Csender]·¢%d[%s]\n",ret,data);
#endif
	return ret;
}
int Csender::Recv()
{
	int strlen =0;
	int len =FEEDBACK_MSG_LEN-1;
	int ret    =0;
	char *p =this->fb;

	time_t timer=time(NULL);

	if(m_sockfd != INVALID_SOCKET)
	do
	{
		ret = recv(m_sockfd, p , len, 0);
		if (ret>0)
		{
			strlen += ret;
			len -= strlen;
			p +=ret;
		}
// 		else if ( ret == 0 )
//             printf("Connection closed\n");
//         else
//             printf("recv failed: %d\n", WSAGetLastError());

		if ((unsigned int)(time(NULL)-timer)>g_cfg.outtime_s)
		{
			DEBUGOUT("接收超时\n");
			FLOG_MSG("接收超时");
			break;
		}
		if (strstr(fb,"{\"succ\":\"") && strstr(fb,"}"))
		{
			break;
		}
	} while (1);

	this->fb[strlen]='\0';

#ifdef _DEBUG
	FLOG_MSG("[log][Csender][收%d[%s]].",strlen,this->fb);
#endif
	if(ret ==  SOCKET_ERROR)
	{
		DEBUGOUT("remote close\n");
		FLOG_MSG("remote close %d .",m_sockfd);
		ret=0;
	}
	else
	{
	}
	Close();
	return ret;
}

