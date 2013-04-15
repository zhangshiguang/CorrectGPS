/*
 * CClient.cpp
 *
 *  Created on: 2013-4-8
 *      Author: zr
 */

/******************************************************************************
 ** ¿Í»§¶ËÀà
 ** 1. ŽŠÀíœÓÊÕµœµÄÊýŸÝ
 ** 2. »ØžŽ
 ** 3. ÖØÓÃ
 ** 4. ·¢ËÍžø³µÔØ²¢°Ñ·µ»ØÊýŸÝ×ªžø¿Í»§¶Ë
 ** author£ºÕÅÊÀ¹â 2012.7
 **
 ******************************************************************************/
#include <string.h>
#include <assert.h>
#include "g_api.h"
#include "api.h"
#include "CClient.h"
#include "log.h"
#include "CclientManager.h"
#include "Csender.h"
#include "tools.h"
extern const char * h;
extern const char * x;

void ini(CClient * client, int fd)
    {
    socklen_t len;
    if (0 != getpeername(fd, (struct sockaddr *) &client->m_sinaddr, &len))
	{

	}

    client->status = 0;
    client->delay_time = time(NULL);

    memset(client->serial, 0, sizeof(client->serial));
    memset(&(client->iobuf), 0, sizeof(client->iobuf));
    client->offset = 0;
    client->fd = fd;

    if (0 != fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK))
	{
#ifdef _DEBUG
	FLOG_MSG("[error][ini]fcnt.");
#endif
	}

    }

CClient::CClient(int fd, CclientManager* cman)
    {
    m_cman = cman;
    ini(this, fd);
    }

void CClient::ReInit(int fd)
    {
    ini(this, fd);
    }

void CClient::Close()
    {
    this->status = 0;
    if (INVALID_SOCKET != fd)
	{
	close(fd);
	fd = INVALID_SOCKET;
	}
    }

CClient::~CClient()
    {
    Close();
    }

void CClient::resetbuf(char * buf)
    {
    offset = 0;
    }
void CClient::receive()
    {
    FLOG_MSG("CClient::receive");
    int len = recv(fd, iobuf + offset, MAXBUF - offset, 0);
    if (len == -1) //error
	{
    	FLOG_MSG("CClient::receive len=-1");
	this->m_cman->DoKill(fd);
	}
    else if (len == 0) //peer close
	{
	FLOG_MSG("CClient::receive len=0");
	this->m_cman->DoKill(fd);
	}
    else
	{
	parse(len);
	}
    }
//////http://www.domain.com/index.php/api/car/navto?md5= &serial= &xbaidu= &ybaidu=
//http://www.domain.com/index.php/api/car/navto?serial=356823032255122&md5=2eb399b9588b1ac2ccc35dce6f266033&xbaidu=11.222&ybaidu=33.333
//GET /index.php/api/car/navto?serial=356823032255122&md5=2eb399b9588b1ac2ccc35dce6f266033&xbaidu=114.73427&ybaidu=10.640487 HTTP/1.1

bool CClient::parse(int ulen)
    {
#define GPS_LEN  11
    char *recvbuf = this->iobuf;
#ifdef LOGLOG
    string log("[get][");
    char ndate[24];
    memset(ndate,0,sizeof(ndate));
    getnowdate(ndate);
    log.append(ndate);
    log.append("][");
    log.append((char*)recvbuf);
    log.append("].");
    FLOG_MSG(log.c_str());
#endif

    if (ulen > 0)
	{
	int len = ulen + offset;

	recvbuf[len] = 0;
	char *p = (char *) recvbuf;

	int i = 0, j = 0;

	//navto
	while (i < len)
	    {
	    if (('N' == p[i] || 'n' == p[i]) && ('a' == p[i + 1] || 'A' == p[i
		    + 1]) && ('V' == p[i + 2] || 'v' == p[i + 2]) && ('T'
		    == p[i + 3] || 't' == p[i + 3]) && ('O' == p[i + 4] || 'o'
		    == p[i + 4]))
		{
		j = i + 6; //j=> serial
		if (j + IN_DATA_LEN > len)///ÓÐ<1žöÍêÕûÖžÁî
		    {
			DEBUGOUT("not enough len.\n");
		    offset = len - i;
		    memcpy(p, p + i, offset);
		    p[offset] = 0;
		    break;
		    }
		else //enough one
		    {

		    int x = j;
		    //get serial
		    while (x < len)
			{
			if (('S' == p[x] || 's' == p[x]) && ('e' == p[x + 1]
				|| 'E' == p[x + 1]) && ('R' == p[x + 2] || 'r'
				== p[x + 2]) && ('I' == p[x + 3] || 'i' == p[x
				+ 3]) && ('A' == p[x + 4] || 'a' == p[x + 4])
				&& ('L' == p[x + 5] || 'l' == p[x + 5]))
			    {
				DEBUGOUT("get serial.");
			    memcpy(this->serial, p + x + 7, SERIAL_LEN);
			    this->serial[SERIAL_LEN] = '\0';
			    break;
			    }
			else
			    {
			    ++x;
			    }

			}
		    assert(x < len);
		    //get md5
		    x = j;
		    while (x < len)
			{
			if (('M' == p[x] || 'm' == p[x]) && ('d' == p[x + 1]
				|| 'D' == p[x + 1]) && '5' == p[x + 2])
			    {
				DEBUGOUT("get md5.\n");
			    memcpy(this->md5, p + x + 4, MD5_LEN);
			    this->md5[MD5_LEN] = '\0';
			    break;
			    }
			else
			    {
			    ++x;
			    }

			}
		    assert(x < len);
		    //get xbaidu;
		    x = j;
		    while (x < len)
			{
			if (('X' == p[x] || 'x' == p[x]) && ('B' == p[x + 1]
				|| 'b' == p[x + 1]) && ('a' == p[x + 2] || 'A'
				== p[x + 2]) && ('I' == p[x + 3] || 'i' == p[x
				+ 3]) && ('D' == p[x + 4] || 'd' == p[x + 4])
				&& ('U' == p[x + 5] || 'u' == p[x + 5]))
			    {
				DEBUGOUT("get xbaidu.\n");
			    int dot = x + 7;
			    while (p[dot] != ' ' && p[dot] != '&' && dot < x
				    + 7 + GPS_LEN) //xbaidu=111.333333& or 11.333
				{
				++dot;
				}
			    if (dot < x + 7 + GPS_LEN)
				{
				memcpy(this->xbaidu, p + x + 7, dot - (x + 7));
				this->xbaidu[dot - (x + 7)] = '\0';
				break;
				}
			    else
				{
				FLOG_MSG("[log][CCLient]xbaidu  data error");
				break;
				}

			    }
			else
			    {
			    ++x;
			    }

			}
		    assert(x < len);
		    //get ybaidu
		    x = j;
		    while (x < len)
			{
			if (('Y' == p[x] || 'y' == p[x]) && ('B' == p[x + 1]
				|| 'b' == p[x + 1]) && ('a' == p[x + 2] || 'A'
				== p[x + 2]) && ('I' == p[x + 3] || 'i' == p[x
				+ 3]) && ('D' == p[x + 4] || 'd' == p[x + 4])
				&& ('U' == p[x + 5] || 'u' == p[x + 5]))
			    {
				DEBUGOUT("get ybaidu.\n");
			    int dot = x + 7;
			    while (p[dot] != ' ' && p[dot] != '&' && dot < x
				    + 7 + GPS_LEN) ////ybaidu=11.333 or 1.333
				{
				++dot;
				}
			    if (dot < x + 7 + GPS_LEN)
				{
				memcpy(this->ybaidu, p + x + 7, dot - (x + 7));
				this->ybaidu[dot - (x + 7)] = '\0';
				break;
				}
			    else
				{
			    	DEBUGOUT("ybaidu data error\n");
				FLOG_MSG("[log][CCLient]ybaidu data error");
				break;
				}
			    }
			else
			    {
			    ++x;
			    }
			}
		    if((x >= len)){
		    	perror("parse error");
		    }
		    this->status = 1;
		    this->m_cman->notificationthreads(this->fd);
		    break;
		    }

		}
	    else
		{
		++i;
		}
	    }//while

	}

    return true;
    }
void CClient::SetSend2CarServer()
    {
    status = 2;
    }
bool CClient::GetSend2CarServer()
    {
    return  (2==status);
    }
void CClient::Send2CarServer()
    {
#ifdef _DEBUG
    //char ndate[24];
    //memset(ndate,0,sizeof(ndate));
    //getnowdate(ndate);
    FLOG_MSG("[log][CCLient]Send2CarServer()");
#endif
    // send 2 car server
    Csender *client = new Csender();
    if (client->Connect())
	{
	client->Send(this);

	//get feedback
	client->Recv();

	//forward to client
	this->sendback(client->fb, strlen(client->fb));

	}
    else
	{
	FLOG_MSG("fail to connecting car server!.");
	}

    }
int CClient::sendback(const char* data, int len)
    {

    try
	{
	int ret = send(fd, data, len, 0);

#ifdef LOGLOG
	string log("[log]to[");
	log.append(this->serial);
	log.append("]"); ///[ret

#ifdef _DEBUG
	log.append(data);
#endif
	log.append(".");
	FLOG_MSG(log.c_str());
#endif
	return ret;
	}
    catch (...)
	{
	}
    return 0;
    }
