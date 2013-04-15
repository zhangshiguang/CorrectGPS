/*
 * dbdo.h
 *
 *  Created on: 2012-12-6
 *      Author: zsg
 */

#ifndef DBDO_H_
#define DBDO_H_

#undef max
#undef min


#include <string>
using namespace std;
#include "api.h"
typedef struct
{
        MYSQL *conn;
        MYSQL_ROW row;
        MYSQL_RES *result;
        unsigned long m;
        pthread_cond_t *cond;
        pthread_mutex_t *mut;
}TDB;

class Cdbdo
    {
public:
  Cdbdo();
    virtual ~Cdbdo();

    bool DataBaseIni();
    void loaddb(INFO_MAP * mapinfo);
    bool BBT_DOSQL(const char *psql);
    void BBT_DisConn();
    void getdb (void * t);
public:
    TDB pdb;
    MYSQL *conn;

#ifdef MULTI_THREAD
    pthread_mutex_t mut;
    pthread_cond_t cond;
#endif

private:
    bool readConfigFile();
    MYSQL_RES * getResult(const char* sqlstr, bool *boolresult);

    bool checklink();


private:
    INFO_MAP *m_mapinfo;


    string m_strServerName;
    string m_strUserName;
    string m_strPassword;
    string m_strDBName;
    int m_port;

    string configFilename;
    std::map<string,string> kvmap;

    };

#endif /* DBDO_H_ */
