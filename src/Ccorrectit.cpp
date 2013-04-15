/*
 * Ccorrectit.cpp
 *
 *  Created on: 2013-4-8
 *      Author: zsg
 */
#include <stdlib.h>
#include <string.h>
#include "CClient.h"
#include "CclientManager.h"
#include "CXArray.h"
#include "Ccorrectit.h"
#include "tools.h"
#include "log.h"

Ccorrectit::Ccorrectit()
    {
    // TODO Auto-generated constructor stub

    }

Ccorrectit::~Ccorrectit()
    {
    // TODO Auto-generated destructor stub
    }

//²ßÂÔ£º
//1.Ê¹ÓÃClientMapµÄÁÙœçÇø£º·¢ËÍÃŠÊ±£¬µŒÖÂÆäËüÉè±ž·ÃÎÊ±»ÔÝÊ±¹ÒÆð£¬žüœøÒ»²œ¿ÉÄÜµŒÖÂÌ«ÉîµÄ¶ÑÕ»£»
//2.²»Ê¹ÓÃClientMapµÄÁÙœçÇø£º·¢ËÍÊ±£¬¿ÉÄÜ·ÃÎÊÁËÒ»žöÒÑŸ­±»freeµÄclientÖžÕë£»µ«ÊÇ»ñµÃµÄ»Ø±šÊÇÃ»ÓÐ·ÃÎÊ±»¹ÒÆð£¬·ÃÎÊÊ±·¢ËÍÒ²²»»á±»¹ÒÆð£»
void Ccorrectit::correctgps_worker(CClient* pclient, INFO_MAP * pinfomap)
    {
    ////Èç¹û²»Ê¹ÓÃÁÙœçÇø£¬ÔòÐèÒªŽŠÀíÒì³£·ÃÎÊ£»
    CLIENT_MAP::iterator dev_it ;
    INFO_MAP::iterator infoit ;
    LNGMAP::iterator lngit ;

    CPOI gpsxy, baiduxy;
    unsigned int y1, x1;

    //ŸÀÆ«
    baiduxy.dy = atof(pclient->ybaidu);
    baiduxy.dx = atof(pclient->xbaidu);
    y1 = chang2uint(pclient->ybaidu);
    infoit = pinfomap->find(y1);
#ifdef _DEBUG
    //char ndate[24];
    //memset(ndate,0,sizeof(ndate));
    //getnowdate(ndate);
    FLOG_MSG("[log][correctgps_worker][finding %d]",y1);
#endif
    if (infoit != pinfomap->end())
	{
	x1 = chang2uint(pclient->xbaidu);
#ifdef USE_ARRAY
	gpsxy = infoit->second->getGpsxy(baiduxy, x1);//ŽÓarrayÕÒ
#else
	lngit = infoit->second->lngmap.find(x1);
	if (lngit != infoit->second->lngmap.end())
	    {
	    gpsxy.dx -= lngit->second->dx;
	    gpsxy.dy -= lngit->second->dy;
	    }
#endif
#ifdef _DEBUG
	FLOG_MSG("[log][correctgps_worker][found]");
#endif
	}
    else
	{ //Ã»ÕÒµœÔòÖ±œÓ·¢ËÍÔ­ÊŒŸ­Î³¶È
	gpsxy = baiduxy;
#ifdef _DEBUG
	FLOG_MSG("[log][correctgps_worker][not find]");
#endif
	}
    pclient->xgps = gpsxy.dx;
    pclient->ygps = gpsxy.dy;
    //·¢ËÍžøÏàÓŠµÄ·þÎñÆ÷(client·¢ËÍ²¢œÓÊÕ·µ»ØjsonÊýŸÝ£¬×ª·¢žø¿Í»§¶Ë)
    pclient->SetSend2CarServer();

    }

