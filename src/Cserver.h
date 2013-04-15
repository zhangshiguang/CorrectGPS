/*
 * Cserver.h
 *
 *  Created on: 2013-4-10
 *      Author: zsg
 */

#ifndef CSERVER_H_
#define CSERVER_H_
#include "g_api.h"
#include "api.h"

class Cserver
    {
public:
    Cserver();
    virtual ~Cserver();

    static int createListener(string ipAddress,string port,bool block);
    };



#endif /* CSERVER_H_ */
