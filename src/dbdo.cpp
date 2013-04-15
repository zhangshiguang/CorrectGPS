/*
 * dbdo.cpp
 *
 *  Created on: 2012-12-6
 *      Author: zsg
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <string>
#include <map>

#include "Thread.h"
#include "g_api.h"
#include "api.h"

#undef max
#undef min
#ifdef INVALID_SOCKET
#undef INVALID_SOCKET
#endif
#include <my_global.h>
#include <mysql.h>
#include <errmsg.h>

#include "CXArray.h"
#include "dbdo.h"
#include "log.h"
#include "tools.h"
using namespace std;

//MYSQL *Cdbdo::conn = NULL;
MYSQL_RES * Cdbdo::getResult(const char* sqlstr, bool *boolresult)
    {
    try
	{
	switch (mysql_query(this->conn, sqlstr))
	    {
	case CR_COMMANDS_OUT_OF_SYNC: ///因为上一个sql执行返回了多个结果集，但没有把结果都取出来，就执行了新的sql语句，就会产生这个错误。
	    FLOG_ERR("getResult>>CR_COMMANDS_OUT_OF_SYNC.");
	    //break;
	case CR_SERVER_GONE_ERROR:
	    FLOG_ERR("getResult>>CR_SERVER_GONE_ERROR.");
	    //break;
	case CR_SERVER_LOST:
	    FLOG_ERR("getResult>>CR_SERVER_LOST.");
	    //break;
	case CR_UNKNOWN_ERROR:
	    FLOG_ERR("getResult>>CR_SERVER_LOST.");
	    *boolresult = false;
	    break;
	default:
	    break;
	    }

	return mysql_store_result(Cdbdo::conn);
	}
    catch (...)
	{
	*boolresult = false;
	return NULL;
	}
    }

bool Cdbdo::checklink()
    {
    int n = 0;

    while (0 != mysql_ping(Cdbdo::conn))
	{
	string pstr("[error][SQL]");
	pstr += mysql_error(Cdbdo::conn);
	pstr += ".";
	FLOG_ERR(pstr.c_str());
	if (3 < ++n)
	    return false;
	}
    return true;

    }

////////////////////////////////
//¹Ù·œ£ºÔÚÏàÍ¬µÄÁ¬œÓÉÏ£¬ÁœžöÏß³Ì²»ÄÜÍ¬Ê±œ«²éÑ¯·¢ËÍµœMySQL·þÎñÆ÷¡£ÓÈÆäÊÇ£¬±ØÐëÈ·±£ÔÚmysql_query()ºÍmysql_store_result()Ö®Œä£¬Ã»ÓÐÊ¹ÓÃÏàÍ¬Á¬œÓµÄÆäËûÏß³Ì¡£
//Ò»žöÏß³ÌÖ»ŒÓÔØÒ»²¿·ÖÊýŸÝ£¬
//////////////////////////
void Cdbdo::getdb(void * arg)
    {
    Cdbdo* pdbdo = (Cdbdo*) arg;
    TDB *ptdb = &pdbdo->pdb;
    bool retn;

    ///select min(lat) ,max(lat) from map_shenzhen_copy where lng=137.70 order by lat ;
    //10   ,48.80
    ////Ÿ«¶È0.01 µÝœø ¹²3880Ìõ
    //Ä¬ÈÏ Î³¶ÈŽÓ17.00 µœ 53.00;
    //Ê±ŒäÖ÷Òª»š·ÑÔÚ²éÑ¯·µ»ØÊýŸÝÉÏ£šŒÓorderbyµŒÖÂÊ±ŒäŸÃ£»È¥µôorderby ÊýŸÝ1ÃëÄÚÄÜ²éÑ¯ŽøŽŠÀíÍê£©
    //ŽÓÄÚÍø·ÃÎÊ1ÃëÄÜ²éÑ¯2ÍòÌõ£»ŽÓÍâÍø·ÃÎÊ20Ãë²éÑ¯1ÍòÌõ£»ÊýŸÝ¿âŸ¡Á¿²¿ÊðµœÒ»žöŸÖÓòÍø£¬»òÕßŸ¡Á¿²ÉÈ¡Ò»Ð©°ì·šÍ¬²œ»òÕßŒõÉÙŽóÊýŸÝ²éÑ¯¡£

    string sqlstr("SELECT lat,lng,offsetlng,offsetlat FROM baidumaps  limit ");
    string str;
    char offindex[30];

    unsigned long m = 0;
    unsigned long max = MAX_RECORD;

    double dx = 0.00;
    double dy = 0.00;
    unsigned int key = 0;
    unsigned int lngkey = 0;
#ifdef USE_ARRAY
    CXArray *info = NULL;
#else
    CLNGMAP *info = NULL;
    CPOI *poi = NULL;
#endif

    //char  tmp[255];

    //_ini(p->m);
    INFO_MAP::iterator it;
    LNGMAP::iterator lngit;
    //      if (!mysql_thread_safe())
    //      {
    //              return;
    //      }
    //      if(0!= mysql_thread_init()) return;

    if (!checklink())
	return;

    while (!allExitFlag && m < max)
	{
	FLOG_MSG("[log][loaddb]....");
	str = sqlstr;
	sprintf(offindex, "%d,%d ;", (int) m, (int) COUNT_PER_READ);
	m += COUNT_PER_READ;
	str.append(offindex);
	FLOG_MSG(str.c_str());
	retn = true;
	ptdb->result = getResult(str.c_str(), &retn);
	if (!retn || NULL == ptdb->result)
	    {
	    FLOG_MSG("[error][loaddb]getresult!.");
	    setallthreadexitflag(400);
	    return;
	    }
	//


	dx = 0.00;
	dy = 0.00;

	while (NULL != (ptdb->row = mysql_fetch_row(ptdb->result)))
	    {

	    if (NULL != ptdb->row[0]) //lat Î³¶È
		{
		key = chang2uint(ptdb->row[0]); //y
		}
	    else
		{
		FLOG_MSG("[error][loaddb]data error!");
		//setallthreadexitflag(400);
		break;
		}
	    if (NULL != ptdb->row[1])
		{
		lngkey = chang2uint(ptdb->row[1]); //x
		}
	    else
		{
		FLOG_MSG("[error][loaddb]data error!.");
		break;
		}
	    if (NULL != ptdb->row[2])
		{
		dx = atof(ptdb->row[2]);
		}
	    else
		{
		FLOG_MSG("[error][loaddb]data error!.");
		break;
		}
	    if (NULL != ptdb->row[3])
		{
		dy = atof(ptdb->row[3]);
		}
	    else
		{
		FLOG_MSG("[error][loaddb]data error!.");
		break;
		}

	    //²éÕÒÊÇ·ñÒÑŸ­ÓÐŽËÎ³¶ÈµÄmap£š¶ÔÓŠŸ­¶Èarray£©
	    try
		{
#ifdef MULTI_THREAD
		CAutoLock lock(ptdb->mut);
#endif

		it = this->m_mapinfo->find(key);

		if (it == this->m_mapinfo->end())
		    {
#ifdef _DEBUG
		    FLOG_MSG("[log][loaddb]x=%d,y=%d,dx=%f,dy=%f.", lngkey,
			    key, dx, dy);

#endif

		    try
			{

#ifdef USE_ARRAY
			if (NULL != (info = new CXArray()))
			    {
			    m_mapinfo->insert(
				    pair<unsigned int, CXArray*> (key, info)); //<id,info>     //±ØÐëÓÐÐò
			    info->append(lngkey, dx, dy);

#ifdef _DEBUG
			    FLOG_MSG("[log][loaddb]mapinfo->insert(%d).", key);

#endif
			    }
#else
			if (NULL != (info = new CLNGMAP()) && NULL != (poi
					= new CPOI()))
			    {
			    poi->dx = dx;
			    poi->dy = dy;
			    m_mapinfo->insert(
				    pair<unsigned int, CLNGMAP*> (key, info));
			    //lngkey = x;
			    info->lngmap.insert(
				    pair<unsigned int, CPOI*> (lngkey, poi));
			    }
#endif
			}
		    catch (...)
			{
			FLOG_MSG("[exception][loaddb]newÒì³£..");
			setallthreadexitflag(406);
			}

		    }
		else
		    {
#ifdef USE_ARRAY
#if 0//def _DEBUG
		    FLOG_MSG("[log][loaddb]%d=>array(%d)--%d.", key,lngkey,it->second->getcount()+1);

#endif

		    it->second->append(lngkey, dx, dy);
#else

		    //lngkey = x;
		    lngit = it->second->lngmap.find(lngkey);
		    if (lngit == it->second->lngmap.end())
			{
			if (NULL != (poi = new CPOI()))
			    {
			    poi->dx = dx;
			    poi->dy = dy;
			    it->second->lngmap.insert(
				    pair<unsigned int, CPOI*> (lngkey, poi));

			    //sprintf(tmp,"[log][loaddb]%d=>%d=>(%f,%f)--%d.", key,lngkey,dx,dy,it->second->lngmap.size());
			    //FLOG_MSG(tmp);
			    }
			}
		    else
			{
			sprintf(tmp,
				"[error][loaddb]%d=>%d=>(%f,%f)--%d ÖØžŽ.",
				key, lngkey, dx, dy);
			FLOG_MSG( tmp);
			}

#endif
		    }
		}//lock
	    catch (...)
		{
		FLOG_MSG("[exception][loaddb].");
		setallthreadexitflag(407);
		break;
		}

	    }
	mysql_free_result(ptdb->result);

	}//while

    //mysql_thread_end();
    FLOG_MSG("[log][loaddb] ok..");
    BBT_DisConn();

    }
void * service(void *arg)
    {
    Cdbdo* pdb = (Cdbdo*) arg;

    pdb->getdb(pdb);
    return NULL;
    }
void Cdbdo::loaddb(INFO_MAP * mapinfo)
    {

    this->m_mapinfo = mapinfo;
    if (!checklink())
	{
	return;
	}
#ifdef _DEBUG
    getdb(this);
#else
    Thread pthread(&service, this);
    pthread.execute();
#endif
    }

bool Cdbdo::BBT_DOSQL(const char *psql)
    {

    if (0 != mysql_query(Cdbdo::conn, psql))
	{
	char buf[16];
	memset(buf, 0, sizeof(buf));

	string pstr("[error][SQL][");
	sprintf(buf, "%d", (int) time(NULL));
	pstr += buf;
	pstr += "][";
	pstr += psql;
	pstr += "].";
	FLOG_ERR(pstr.c_str());

	string errstr("[error][EXEC SQL]err:");
	errstr += mysql_error(Cdbdo::conn);
	errstr += ".";
	FLOG_ERR(errstr.c_str());
	return false;
	}
    else
	{
	FLOG_MSG("[log][EXEC SQL]ok.");
	return true;
	}
    }

void Cdbdo::BBT_DisConn()
    {
    FLOG_MSG("[SQL]mysql close connect.");
    if (Cdbdo::conn)
	{
	mysql_close(Cdbdo::conn);
	Cdbdo::conn = NULL;
	}
    FLOG_MSG("[SQL]mysql close connect ok.");
    }

bool Cdbdo::readConfigFile()
    {

    if (configFilename.empty())
	{
	FLOG_ERR("[cksettling] no config file for loading..");
	setallthreadexitflag(408);
	return false;
	}

    file_to_items(configFilename.c_str(), &this->kvmap);

    std::map<string, string>::iterator it = kvmap.find(string("db_host_ip"));
    if (it != kvmap.end())
	{
	this->m_strServerName = it->second;
	}
    it = kvmap.find(string("db_name"));
    if (it != kvmap.end())
	{
	this->m_strDBName = it->second;
	}
    it = kvmap.find(string("db_user"));
    if (it != kvmap.end())
	{
	this->m_strUserName = it->second;
	}
    it = kvmap.find(string("db_psw"));
    if (it != kvmap.end())
	{
	this->m_strPassword = it->second;
	}
    it = kvmap.find(string("db_port"));
    if (it != kvmap.end())
	{
	this->m_port = atoi(it->second.c_str());
	}
    /*db_host_ip=183.61.183.120
     db_name=cking_center
     db_user=test
     db_psw=test
     db_port=3306*/
#ifdef _DEBUG
    FLOG_MSG("Read config file:.");
    it = kvmap.begin();
    while (it != kvmap.end())
	{
	FLOG_MSG("%s = %s.", it->first.c_str(), it->second.c_str());
	++it;
	}

#endif

    return true;

    }
bool Cdbdo::DataBaseIni()
    {
    configFilename = getexedir();
    configFilename.append(Ccfg::CONFIG_FILE);

    this->readConfigFile();

    if (Cdbdo::conn)
	{
	//mysql_close(conn);
	}
    else
	{
	Cdbdo::conn = mysql_init(NULL);
	if (!Cdbdo::conn)
	    {
	    FLOG_ERR("mysql_init fail! %s.", mysql_error(Cdbdo::conn));
	    }
	//mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8");
	//int reconnect = 1;
	//mysql_options(conn, MYSQL_OPT_RECONNECT, (int *)&reconnect);
	if (!mysql_real_connect(Cdbdo::conn, m_strServerName.c_str(),
		m_strUserName.c_str(), m_strPassword.c_str(),
		m_strDBName.c_str(), m_port, NULL, 0))
	    {
	    FLOG_ERR("Failed to connect to database: Error: %s.",
		    mysql_error(Cdbdo::conn));
	    return false;
	    }

	mysql_set_character_set(Cdbdo::conn, "utf8"); //
	Cdbdo::conn->reconnect = 1;
	}
#ifdef _DEBUG
    FLOG_MSG("connect to database ok.");
#endif

    return true;

    }

Cdbdo::Cdbdo() :
    conn(NULL), m_strServerName("localhost"), m_strUserName("test"),
	    m_strPassword("test"), m_strDBName("baidumaps"), m_port(3306)
    {
    // TODO Auto-generated constructor stub
#ifdef MULTI_THREAD
    pthread_mutex_init(&mut, NULL);
    pthread_cond_init(&cond, NULL);
#endif

    }

Cdbdo::~Cdbdo()
    {
    BBT_DisConn();
#ifdef MULTI_THREAD
    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);
#endif

#ifdef _DEBUG
    FLOG_MSG("[exit]~dbdo().");
#endif
    }

