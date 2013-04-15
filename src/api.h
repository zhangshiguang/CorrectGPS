/*
 * api.h
 *
 *  Created on: 2013-4-9
 *      Author: zsg
 */

#ifndef API_H_
#define API_H_


/*
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include <locale.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>

#include<sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <syslog.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
//#include <my_global.h>
//#include <mysql.h>
*/
#include <list>
#include <map>
#include <vector>
#include <string>

using namespace std;

#include "g_api.h"

//Ä¬ÈÏ Î³¶ÈŽÓ17.00 µœ 53.00; Ÿ«¶È0.01 ;Ã¿žöŸ­¶È¶È¶Œ¶ÔÓŠ3600ÌõÎ³¶ÈŒÇÂŒ¡£
//Ä¬ÈÏ Ÿ­¶ÈŽÓ72.00 µœ 137.99£»Ÿ­¶È0.01£»Ã¿žöÎ³¶È¶ÔÓŠ6600ÌõŸ­¶ÈŒÇÂŒ¡£
//select lat,count(lng) from baidumaps group by lat;


#ifdef USE_ARRAY

  typedef struct
  {
          double dx;
          double dy;
  }CPOI;
#else
  class CPOI
  {
  public:
          double dx;
          double dy;
  };
#endif

typedef std::map<unsigned int,CPOI*> LNGMAP;

class CClient;
class CXArray;

#include "CClient.h"
//ÖÕ¶Ë¶ÓÁÐ socket is key
typedef std::map<int, CClient*> CLIENT_MAP;

//ŸÀÆ«ÐÅÏ¢ÁÐ±í  Î³¶È is key
#ifdef USE_ARRAY
typedef std::map<unsigned int,CXArray*> INFO_MAP;
#else
typedef std::map<unsigned int,CLNGMAP*> INFO_MAP;
#endif




//
typedef vector< HANDLE > HL;  //Ïß³ÌŸä±úŒ¯ºÏ
typedef HL::iterator HLI;     //Ïß³ÌŸä±úÖžÕë
//
////
class CAutoLock
{
public:
        CAutoLock(pthread_mutex_t *pmutex):m_pmutex(pmutex)
        {
          if(0!=pthread_mutex_lock(m_pmutex))
            {

            }
        }
        ~CAutoLock(){
          if(0!=pthread_mutex_unlock(m_pmutex))
            {

            }
        }

private:
        pthread_mutex_t * m_pmutex;
};

//


/////////////////////////////////////////

///ÈÕÆÚ¡¢Ê±ŒäÊýŸÝÀàÐÍ
typedef struct
{
  BYTE year;                      ///Äê
  BYTE month;                     ///ÔÂ
  BYTE day;                       ///ÈÕ
  BYTE hour;                      ///Ê±
  BYTE min;                       ///·Ö
  BYTE sec;                       ///Ãë
}GC_Time_Struct;


extern void LogExt(int loglevel,const char* lpszFormat,...);

///////////
char *getYYYYMMDD(char *nowdate);
void stopFrommyInter();
void setallthreadexitflag(int index);
//void exceptiontolog (CSeException *e);
extern bool volatile allExitFlag;


#ifdef _DEBUG
#ifdef WIN
#define DEBUGOUT  OutputDebugString
#else
        #define DEBUGOUT  FLOG_MSG
#endif
#else
        #define DEBUGOUT
#endif


#include "Ccfg.h"
extern Ccfg g_cfg;

//
struct tcp_keepalive_in
{
    ULONG onoff;
    ULONG keepalivetime;
    ULONG keepaliveinterval;
};
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR, 4)
#endif

int writelogimmediatly(const char *msg);
unsigned int getAccuracy01(double x); //°ŽŸ«¶È0.01È¡Öµ(*100È¡Õû)  //²»×ŒÈ·
unsigned int chang2uint(const char*str);////°ŽŸ«¶È0.01È¡Öµ(*100È¡Õû)
///////////////////////


#endif /* API_H_ */
