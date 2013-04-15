/*
 * CClient.h
 *
 *  Created on: 2013-4-8
 *      Author: zr
 */

#ifndef CCLIENT_H_
#define CCLIENT_H_

//#ifdef __linux__

#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "api.h"

using namespace std;
#define MAXBUF (1024*4)
class CclientManager;
class CClient
    {
public:

    CClient(int fd, CclientManager* cman);
    virtual ~CClient();

    void ReInit(int fd);
    void Close();
    void receive();
    bool parse(int ulen);
    void resetbuf(char *buf);

    void SetSend2CarServer();
    bool GetSend2CarServer();

    void Send2CarServer();//·¢ËÍÊýŸÝžø³µÔØ·þÎñÆ÷²¢ÇåÀíÊµÀýµÄÊýŸÝ

    int fd;

    int offset; ///buffer offset
    char iobuf[MAXBUF + 1];


    char serial[SERIAL_LEN + 1];
    char md5[MD5_LEN + 1];
    char xbaidu[16];
    char ybaidu[16];
    double xgps;
    double ygps;

    int status;// 1: get all data ;2: gps corrected ;


    CclientManager* m_cman;
    time_t delay_time;//delay for kill
    sockaddr_in m_sinaddr;
private:
    int sendback(const char* data, int len);
    };

#endif /* CCLIENT_H_ */
