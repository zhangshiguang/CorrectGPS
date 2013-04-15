/*
 * Ccfg.h
 *
 *  Created on: 2013-4-9
 *      Author: zsg
 */

#ifndef CCFG_H_
#define CCFG_H_
#include <map>
//#include <vector>
#include <string>
class Ccfg
{
public:
  Ccfg();
  virtual
  ~Ccfg();

  bool readConfigFile();

private:
  string configFilename;
  std::map<string,string> kvmap;
public:
  unsigned int carserver_port; //CARSERVER_PORT
  string carserver_ip;// //CARSERVER_IP
  string carserv_domainname;//CARSERVER_DOMAINNAME
  string local_port;	//LOCAL_PORT
  unsigned int max_clients;//MAX_CONNECTION_NUM
  unsigned int outtime_s;  //LINK_TIMEOUT
  unsigned int max_worker;

public:
  static const char *LOG_FILE;
  static const char *CONFIG_FILE;
};

#endif /* CCFG_H_ */
