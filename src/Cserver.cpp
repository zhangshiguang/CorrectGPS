/*
 * Cserver.cpp
 *
 *  Created on: 2013-4-10
 *      Author: zsg
 */
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <string.h>
#include "g_api.h"
#include "api.h"
#include "log.h"
#include "Cserver.h"

Cserver::Cserver()
{
	// TODO Auto-generated constructor stub

}

Cserver::~Cserver()
{
	// TODO Auto-generated destructor stub
}
int Cserver::createListener(string ipAddress, string port, bool block)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int yes = 1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	const char *ip_addr = NULL;
	if (!ipAddress.empty())
	{
		ip_addr = ipAddress.c_str();
	}

	if ((rv = getaddrinfo(ip_addr, port.c_str(), &hints, &servinfo)) != 0)
	{
		FLOG_MSG("getaddrinfo: %s.", gai_strerror(rv));
		setallthreadexitflag(201);
		return CKERROR;
	}
	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))

				== -1)
		{
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1)
		{
			perror("1 setsockopt");
			setallthreadexitflag(202);
			return CKERROR;
		}
		//defer accept，从字面上理解是推迟accept，实际上是当接收到第一个数据之后，才会创建连接，三次握手完成，连接还没有建立。
		//对于像HTTP等非交互式的服务器，这个很有意义，可以用来防御空连接攻击（只是建立连接，但是不发送任何数据）。
		//里面 val 的单位是秒，注意如果打开这个功能，kernel 在 val 秒之内还没有收到数据，不会继续唤醒进程，而是直接丢弃连接。如果服务器设置TCP_DEFER_ACCEPT选项后，服务器受到一个CONNECT请求后，三次握手之后，新的socket状态依然为SYN_RECV，而不是ESTABLISHED，操作系统不会Accept。
		//由于设置TCP_DEFER_ACCEPT选项之后，三次握手后状态没有达到ESTABLISHED，而是SYN_RECV。这个时候，如果客户端一直没有发送"数据"报文，服务器将重传SYN/ACK报文，重传次数受net.ipv4.tcp_synack_retries参数控制，达到重传次数之后，才会再次进行setsockopt中设置的超时值，因此会出现SYN_RECV生存时间比设置值大一些的情况。

		int optval = 2;
		if (setsockopt(sockfd, SOL_TCP, TCP_DEFER_ACCEPT, &optval,sizeof(optval)) < 0)
		{
		    perror("2 setsockopt");
		    setallthreadexitflag(203);
		    return CKERROR;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			sockfd = SOCKET_ERROR; //-1
			perror("server: bind");
			continue;
		}
		break;
	}
	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		setallthreadexitflag(204);
		return CKERROR;
	}
	freeaddrinfo(servinfo); // all done with this structure
	if (listen(sockfd, BACKLOGM) == -1)
	{
		perror("listen");
		close(sockfd);
		setallthreadexitflag(205);
		return CKERROR;
	}

	if (!block)
	{
		fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
	}
	return sockfd;
}
