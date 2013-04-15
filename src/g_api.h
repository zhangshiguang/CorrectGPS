/*
 * g_api.h
 *
 *  Created on: 2013-4-9
 *      Author: zsg
 */

#ifndef G_API_H_
#define G_API_H_

#define MAKE_DAEMON

#define LOGLOG
#ifdef _DEBUG
#define DEBUGOUT printf
#else
#define DEBUGOUT
#endif
#define  NONEBLOCK_SOCKET
#define USE_EPOLL
#define USE_ARRAY
#define ARRAY_COUNT 6600
#ifdef _DEBUG
#define MAX_RECORD   300
#define COUNT_PER_READ  100
#else
#define COUNT_PER_READ  10000000
#define MAX_RECORD   30000000
#endif

#define FEEDBACK_MSG_LEN  1024
#ifndef SOCKET_ERROR
#define SOCKET_ERROR  (-1)
#endif

#ifndef BACKLOGM
#define BACKLOGM  1024
#endif

#define HANDLE int    /////__linux__
typedef unsigned char BYTE;
typedef unsigned long  ULONG;

//////////////////////////////
#define IN_DATA_LEN   87    //strlen(serial=356823032255122&md5=2eb399b9588b1ac2ccc35dce6f266033&xbaidu=111.222&ybaidu=33.333)
#define SERIAL_LEN    15
#define MD5_LEN       32
//#define IMEI_LEN     15
//#define       MAX_REQ_BUF_LEN 80     //×îŽóÇëÇó×ÖŽ®<80

//#define  CK_MAX_INFO_LEN 600
//#define  CK_MAX_TITLE_LEN 100
//#define  CK_CMD_MAX_LEN 4

///////////////////////////////
// buffer to int
#define CK_BUF2INT(p)           (((p)[3]<<24)+((p)[2]<<16)+((p)[1]<<8)+(p)[0])
#define CK_BUF2ULONG(p) ((((unsigned char*)p)[3]<<24)+(((unsigned char*)p)[2]<<16)+(((unsigned char*)p)[1]<<8)+((unsigned char*)p)[0])
// int to buffer
#define CK_INT2BUF(n,p) {(p)[0]=((n)>>24)&0xff;(p)[1]=((n)>>16)&0xff;(p)[2]=((n)>>8)&0xff;(p)[3]=n&0xff;}
#define CK_ULONG2BUF(n,p)       {(p)[3]=(unsigned char)(((unsigned long)n)>>24)&0xff;(p)[2]=(unsigned char)(((unsigned long)n)>>16)&0xff;(p)[1]=(unsigned char)(((unsigned long)n)>>8)&0xff;(p)[0]=(unsigned char)(unsigned long)n&0xff;}
// buffer to short
#define CK_BUF2SHT(p)           (((p)[1]<<8)+(p)[0])
// short to buffer
#define CK_SHT2BUF(n,p) {(p)[1]=((n)>>8)&0xff;(p)[0]=(n)&0xff;}


typedef enum
{
        DEBUG_ONLY_LEVEL,
        LOG_LOG_LEVEL,
        WARNNING_LEVEL, ///Ÿ¯žæ
        ERROR_LEVEL,    ///ÓÐŽí£¬²»Ó°ÏìºóÐøÕýÈ·ÔËÐÐ
        CRITICAL_LEVEL,   ///ÖØÒª£ºÈç¹ûÊÇŽíÎóÔòÐèÒªÖØÆô£¬Èç¹ûÊÇÕý³££¬ÔòÊÇÖØÒªÐÅÏ¢
}LOG_LEVEL;
/*
#define  LOG_EMERG   "0" //ÓÃÓÚœôŒ±ÏûÏ¢, ³£³£ÊÇÄÇÐ©±ÀÀ£Ç°µÄÏûÏ¢.
#define  LOG_ALERT   "1" //ÐèÒªÁ¢¿Ì¶¯×÷µÄÇéÐÎ
#define  LOG_CRIT    "2" //ÑÏÖØÇé¿ö, ³£³£ÓëÑÏÖØµÄÓ²Œþ»òÕßÈíŒþÊ§Ð§ÓÐ¹Ø.
#define  LOG_ERR     "3" //ÓÃÀŽ±šžæŽíÎóÇé¿ö;
#define  LOG_WARNING "4" //ÓÐÎÊÌâµÄÇé¿öµÄŸ¯žæ, ÕâÐ©Çé¿ö×ÔŒº²»»áÒýÆðÏµÍ³µÄÑÏÖØÎÊÌâ
#define  LOG_NOTICE  "5" //Õý³£Çé¿ö, µ«ÊÇÈÔÈ»ÖµµÃ×¢Òâ. ÔÚÕâžöŒ¶±ðÒ»Ð©°²È«Ïà¹ØµÄÇé¿ö»á±šžæ.
#define  LOG_INFO    "6" //ÐÅÏ¢ÐÍÏûÏ¢. ÔÚÕâžöŒ¶±ð, ºÜ¶àÇý¶¯ÔÚÆô¶¯Ê±ŽòÓ¡ËüÃÇ·¢ÏÖµÄÓ²ŒþµÄÐÅÏ¢.
#define  LOG_DEBUG   "7"  //ÓÃ×÷µ÷ÊÔÏûÏ¢
*/

#define CKMIN(a,b) ((a)>(b)?(b):(a))
#define CKERROR (-1)
#define SOCKET_ERROR (-1)
#ifndef INVALID_SOCKET
#define INVALID_SOCKET  (-1)
#endif
// ·Öžô×Ö·û

#define SEPARATE_COMMA          0x2C    // (',')
#define SEPARATE_SEMICOLON      0x3B    // (';')

#endif /* G_API_H_ */
