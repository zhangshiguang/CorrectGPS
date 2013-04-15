/*
 * Csender.h
 *
 *  Created on: 2013-4-8
 *      Author: zr
 */

#ifndef CSENDER_H_
#define CSENDER_H_

class Csender
{
public:
	Csender();
	virtual ~Csender();
	int m_sockfd;

	void Close();
	bool Connect();
	bool SetRecvTimeOut(int t);
	bool SetSendTimeOut(int t);

	int Send(CClient* p);
	int Recv();

	char fb[FEEDBACK_MSG_LEN];
private:
	int m_sendtimeout;
	int m_recvtimeout;

	bool initedflag;
};

#endif /* CSENDER_H_ */
