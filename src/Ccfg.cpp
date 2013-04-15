/*
 * Ccfg.cpp
 *
 *  Created on: 2013-4-9
 *      Author: zsg
 */
#include <math.h>
#include <string.h>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include "g_api.h"
#include "api.h"
#include "log.h"
#include "tools.h"
#include "Ccfg.h"
 const char *Ccfg::LOG_FILE = "/correctgps.log";
 const char *Ccfg::CONFIG_FILE = "/correctgps.conf";
Ccfg::Ccfg()
{
  // TODO Auto-generated constructor stub

}

Ccfg::~Ccfg()
{
  // TODO Auto-generated destructor stub
}

bool Ccfg::readConfigFile()
{
    FLOG_MSG("[correctgps][get logfile]");

    	string sf(getexedir());
    	//FLOG_MSG("[correctgps]getcwd[%s].", sf.c_str());

    	sf.append(Ccfg::LOG_FILE);
    	//FLOG_MSG("[correctgps][%s].", sf.c_str());
    	//logOpen(sf.c_str());
    	string sf2(getexedir());
    	sf2.append(Ccfg::CONFIG_FILE);
    	configFilename =sf2;

        if (configFilename.empty())
        {
                FLOG_ERR("[correctgps] no config file for loading.\n");
                return false;
        }

        file_to_items(configFilename.c_str(), &this->kvmap);

        std::map<string, string>::iterator it = kvmap.find(string("CARSERVER_PORT"));
        if (it != kvmap.end())
        {
            this->carserver_port = atoi(it->second.c_str());
        }else{
        	this->carserver_port =81;
        }

        it = kvmap.find(string("CARSERVER_IP"));
        if (it != kvmap.end())
        {
            this->carserver_ip = it->second;
        }else{
        	this->carserver_ip ="0.0.0.0";
        }
        it = kvmap.find(string("CARSERVER_DOMAINNAME"));
        if (it != kvmap.end())
        {
            this->carserv_domainname = it->second;
        }else{
        	this->carserv_domainname="localhost";
        }
        it = kvmap.find(string("LOCAL_PORT"));
        if (it != kvmap.end())
        {
            this->local_port = it->second;
        }else{
        	this->local_port="8080";
        }
        it = kvmap.find(string("MAX_CONNECTION_NUM"));
        if (it != kvmap.end())
        {
            this->max_clients = atoi(it->second.c_str());
        }else{
        	this->max_clients=50000;
        }
        it = kvmap.find(string("LINK_TIMEOUT"));
        if (it != kvmap.end())
        {
            this->outtime_s = atoi(it->second.c_str());
        }else{
        	this->outtime_s=30;
        }
//max_worker
        it = kvmap.find(string("MAX_WORKER"));
                if (it != kvmap.end())
                {
                    this->max_worker = atoi(it->second.c_str());
                }else{
                	this->max_worker=1;
                }
#ifdef _DEBUG
        FLOG_MSG("Read config file:");
        it = kvmap.begin();
        while(it != kvmap.end())
        {
                FLOG_MSG("%s = %s.", it->first.c_str(),it->second.c_str());
                ++it;
        }

#endif

        return true;

}
