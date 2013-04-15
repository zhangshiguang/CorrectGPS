/*
 * log.h
 *
 *  Created on: 2012-12-6
 *      Author: zsg
 */

#ifndef LOG_H_
#define LOG_H_
#include <stdio.h>
#include <stdarg.h>
#include<fcntl.h>
#define  BUFFSIZE 4096
#define FLOG_MSG  log_msg
#define FLOG_ERR  log_sys

extern FILE *logfp;       /* Log file stream */
void
log_open(const char *ident, int option, int facility);

void
log_msg(const char *fmt, ...);

void
log_sys(const char *fmt, ...);

void
log_doit(int errnoflag, int priority, const char *fmt, va_list ap);

//////////////////////
void
logOpen(const char *logFilename);
void
logClose(void);
void
logMessage(const char *format, ...);

char* getcurrentdir(char * buf);

#endif /* LOG_H_ */
