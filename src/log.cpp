/*
 * log.cpp
 *
 *  Created on: 2012-12-6
 *      Author: zsg
 */
#include <stdio.h>
#include <stdarg.h>
#include<string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/times.h>
#include <locale.h>
#include<time.h>
#include"log.h"

FILE *logfp;

void log_open(const char *ident, int option, int facility)
    {

    openlog(ident, option, facility);
    }

void log_msg(const char *fmt, ...)
    {
    va_list ap;

    va_start(ap, fmt);
    log_doit(0, LOG_INFO, fmt, ap);
    va_end(ap);
    return;
    }

void log_sys(const char *fmt, ...)
    {
    va_list ap;

    va_start(ap, fmt);
    log_doit(1, LOG_ERR, fmt, ap);
    va_end(ap);

    }

void log_doit(int errnoflag, int priority, const char *fmt, va_list ap)
    {
    int errno_save;
    char buf[BUFFSIZE];

    errno_save = errno;
    vsprintf(buf, fmt, ap);
    if (errnoflag)
	sprintf(buf + strlen(buf), ":%s", strerror(errno_save));
    strcat(buf, "\n");
    syslog(priority, "%s", buf);
    return;
    }

////////////////////////////////////////////////////////////////////////

/* Write a message to the log file. Handle variable length argument
 lists, with an initial format string (like printf(3), but without
 a trailing newline). Precede each message with a timestamp. */

void logMessage(const char *format, ...)
    {
    va_list argList;
    const char *TIMESTAMP_FMT = "%F %X"; /* = YYYY-MM-DD HH:MM:SS */
#define TS_BUF_SIZE sizeof("YYYY-MM-DD HH:MM:SS")       /* Includes '\0' */
    char timestamp[TS_BUF_SIZE];
    time_t t;
    struct tm *loc;

    t = time(NULL);
    loc = localtime(&t);
    if (loc == NULL || strftime(timestamp, TS_BUF_SIZE, TIMESTAMP_FMT, loc)
	    == 0)
	fprintf(logfp, "???Unknown time????: ");
    else
	fprintf(logfp, "%s: ", timestamp);

    va_start(argList, format);
    vfprintf(logfp, format, argList);
    fprintf(logfp, "\n");
    va_end(argList);
    }

/* Open the log file 'logFilename' */

void logOpen(const char *logFilename)
    {
    mode_t m;

    m = umask(077);
    logfp = fopen(logFilename, "a+");
    umask(m);

    /* If opening the log fails we can't display a message... */

    if (logfp == NULL)
	{
	log_sys("[cksettling][logOpen <%s> fail]\n", logFilename);
	_exit(-1);
	}

    setbuf(logfp, NULL); /* Disable stdio buffering */

    logMessage("Opened log file");
    }

/* Close the log file */

void logClose(void)
    {
    logMessage("Closing log file");
    fclose(logfp);
    }

