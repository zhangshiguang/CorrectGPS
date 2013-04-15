/*
 * tools.cpp
 *
 *  Created on: 2012-12-8
 *      Author: zsg
 */

#include "tools.h"
#include <string.h>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include <sys/socket.h>
#include <netinet/in.h>
#define  LINE_MAX_LEN 400
/*
 *去除字符串右端空格
 */
char *strtrimr(char *pstr)
{
	int i = strlen(pstr) - 1;
	while ((isspace(pstr[i]) || '\n' == pstr[i]) && (i >= 0))
		pstr[i--] = '\0';

	return pstr;
}
/*
 *去除字符串左端空格
 */
char *strtriml(char *pstr)
{
	int i = 0, j;

	j = strlen(pstr);
	while (isspace(pstr[i]) && (i < j))
		++i;
	if (0 < i)
		strcpy(pstr, &pstr[i]);

	return pstr;
}
/*
 *去除字符串两端空格
 */
char *strtrim(char *pstr)
{
	return strtriml(strtrimr(pstr));
}

/*
 *从配置文件的一行读出key && value
 *line--从配置文件读出的一行
 */
int get_item_from_line(char *line, KEY_VAL_MAP *kvmap)
{
	char *p = strtrim(line);
	int len = strlen(p);
	if (len <= 0)
	{
		return 1;//空行
	}
	else if (p[0] == '#')
	{
		return 2;
	}
	else
	{
		char *v = strchr(p, '=');
		*v++ = '\0';
		kvmap->insert(pair<string, string> (p, v));
	}

	return 0;
}
/*
 * 读取整个配置文件到key value map 中*/
int file_to_items(const char *file, KEY_VAL_MAP *kvmap)
{
	char line[LINE_MAX_LEN];
	FILE *fp;

	fp = fopen(file, "r");
	if (fp == NULL)
		return 1;

	while (fgets(line, LINE_MAX_LEN - 1, fp))
	{
		get_item_from_line(line, kvmap);
	}

	fclose(fp);

	return 0;
}

/*
 *读取value
 */
int read_conf_value(const char *key, char *value, const char *file,
		KEY_VAL_MAP *kvmap)
{

	file_to_items(file, kvmap);
	KEY_VAL_MAP::iterator it = kvmap->find(string(key));
	if (it != kvmap->end())
	{
		strcpy(value, it->second.c_str());
	}

	kvmap->clear();

	return 0;
}
int write_conf_value(const char *key, const char *value, const char *file,
		KEY_VAL_MAP *kvmap)
{
	file_to_items(file, kvmap);

	KEY_VAL_MAP::iterator it = kvmap->find(string(key));
	if (it != kvmap->end())
	{
		it->second = value;
	}

	// 更新配置文件，下面的操作会将文件内容清除
	FILE *fp;
	fp = fopen(file, "w");
	if (fp == NULL)
		return 1;

	it = kvmap->begin();
	while (it != kvmap->end())
	{
		fprintf(fp, "%s=%s\n", it->first.c_str(), it->second.c_str());
		++it;
	}
	fclose(fp);

	kvmap->clear();

	return 0;

}

const char* getexedir()
{
	char buff[500] =
	{ 0 };
	memset(buff, 0, sizeof(buff));
	int n = readlink("/proc/self/exe", buff, 499);
	if (n < 0)
	{
		return NULL;
	}
	string path = buff;
	int nLen = path.rfind('/');
	path.erase(nLen, path.length() - nLen);

	return path.c_str();
}
#include <math.h>
//把浮点数 fv的整数部分转成整数M,小数部分转成整数B
void getMB(float fv, int *intPart, int *fractionalPartToint)
{
        float i;
        float fractional = modff(fv, &i);
        *intPart = (int) i;
        char str[4] =
        { 0 };
        sprintf(str, "%.2f", fractional);
        strcpy(str, str + 2);//去掉0.
        *fractionalPartToint = atoi(str);

}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/*È¡µ±Ç°Ê±Œä žñÊœ:YYYY-MM-DD HH:MI:SS:mmm*/
char *getnowdate(char *nowdate)
{

#ifdef WIN32
	SYSTEMTIME   st;
	GetLocalTime(&st);
	char ndate[24];
	memset(ndate,0,sizeof(ndate));
	sprintf(ndate,"%04d-%02d-%02d %02d:%02d:%02d:%03d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);
#else
	time_t timer;
	struct tm *ltm;
	char ndate[20];
	memset(ndate,0,sizeof(ndate));
	timer = time(NULL);
	ltm = localtime(&timer);
	sprintf(ndate,"%04d-%02d-%02d %02d:%02d:%02d",
		ltm->tm_year + 1900,
		ltm->tm_mon + 1,
		ltm->tm_mday,
		ltm->tm_hour,
		ltm->tm_min,
		ltm->tm_sec);


#endif
	strcpy(nowdate,ndate);

	return nowdate;
}

int myUTF8_to_UNICODE(unsigned short* unicode, unsigned char* utf8, int len)
{
    int length;
    unsigned char* t = utf8;

    length = 0;
    while (utf8 - t < len){
        //one byte.ASCII as a, b, c, 1, 2, 3 ect
        if ( *(unsigned char *) utf8 <= 0x7f ) {
            //expand with 0s.
            *unicode++ = *utf8++;
        }
        //2 byte.
        else if ( *(unsigned char *) utf8 <= 0xdf ) {
            *unicode++ = ((*(unsigned char *) utf8 & 0x1f) << 6) + ((*(unsigned char *) (utf8 + 1)) & 0x3f);
            utf8 += 2;
        }
        //3 byte.Chinese may use 3 byte.
        else {
            *unicode++ = ((int) (*(unsigned char *) utf8 & 0x0f) << 12) +
                ((*(unsigned char *) (utf8 + 1) & 0x3f) << 6) +
                (*(unsigned char *) (utf8 + 2) & 0x3f);
            utf8 += 3;
        }
        length++;
    }

    *unicode = 0;

    return (length);
}
#include <wchar.h>
void UTF_8ToUnicode(wchar_t* pOut,char *pText)
{
    char* uchar = (char *)pOut;

    uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
    uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

    return;
}

void UnicodeToUTF_8(char* pOut,wchar_t* pText)
{
    // ×¢Òâ WCHARžßµÍ×ÖµÄË³Ðò,µÍ×ÖœÚÔÚÇ°£¬žß×ÖœÚÔÚºó
    char* pchar = (char *)pText;

    pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
    pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
    pOut[2] = (0x80 | (pchar[0] & 0x3F));

    return;
}
//ŒÆËãÐ£ÑéºÍ
unsigned short checksum(unsigned short *buffer,int size)
{
	unsigned long cksum=0;
	while(size>1)
	{
		cksum +=  *buffer++;
		size   -=  sizeof(unsigned short);
	}
	if(size)
	{
		cksum  +=  *(unsigned char *)buffer;
	}
	//œ«32Î»Êý×ª»»³É16
	while (cksum>>16)
		cksum=(cksum>>16)+(cksum & 0xffff);

	return (unsigned short) (~cksum);
}


//°ŽŸ«¶È0.01È¡Öµ;*100È¡Õû
unsigned int getAccuracy01(double x)
{
	return 100*x;
}

unsigned int chang2uint(const char*str)
{
	char ui[10];
	unsigned int i =0;
	bool dot=false;
	int j =0;

	memset(ui,0,sizeof(ui));
	while(*str != '\0' && j<3)
	{
		if (*str != '.')
		{
			ui[i++]=*str++;
			if (dot)
			{
				++j;
			}

		}
		else
		{
			j=1;
			dot =true;
			++str;
		}
	}
	return atoi(ui);
}
