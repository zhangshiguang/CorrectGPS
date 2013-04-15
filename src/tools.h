/*
 * tools.h
 *
 *  Created on: 2012-12-8
 *      Author: zsg
 */

#ifndef TOOLS_H_
#define TOOLS_H_
#include <map>
#include <string>
using namespace std;

typedef std::map<string,string> KEY_VAL_MAP;
/*
 *去除字符串右端空格
 */
char *strtrimr(char *pstr);
/*
 *去除字符串左端空格
 */
char *strtriml(char *pstr);

/*
 *去除字符串两端空格
 */
char *strtrim(char *pstr);

/*
 *从配置文件的一行读出key或value,返回item指针
 *line--从配置文件读出的一行
 */
int  get_item_from_line(char *line, KEY_VAL_MAP *item);

int file_to_items(const char *file,  KEY_VAL_MAP *items);

/*
 *读取value
 */
int read_conf_value(const char *key, char *value,const char *file);

int write_conf_value(const char *key,char *value,const char *file);

const char* getexedir();

void getMB(float fv, int *intPart, int *fractionalPartToint);
char *getnowdate(char *nowdate);

#endif /* TOOLS_H_ */
