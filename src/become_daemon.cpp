/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2010.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

/* become_daemon.c

   A function encapsulating the steps in becoming a daemon.
*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include "become_daemon.h"
#include "log.h"
//#include "tlpi_hdr.h"

int                                     /* Returns 0 on success, -1 on error */
becomeDaemon(int flags)
{
    int maxfd, fd;
    pid_t pid = fork();
    FLOG_MSG("[cksettling>> becomeDaemon begin...]");
    if(pid<0)/* while fail  */
    {
    	FLOG_ERR("[cksettling>> fork fail");
    	return(-1);
    }
	else if(pid!=0)  /* while parent  */
	{
		FLOG_MSG("[cksettling>> while parent]");

		_exit(0);

	}
    /* Child falls through... */


    if (setsid() == -1)                 /* Become leader of new session */
    {
    	FLOG_ERR("[cksettling>> setsid() fail]");
    	return -1;
    }

    /* Ensure we are not session leader */
    pid=fork();
	if(pid<0)/* while fail  */
	{
		FLOG_ERR("[cksettling>> fork(2) fail]");
		return(-1);
	}
	else if(pid!=0)  /* while parent  */
	{
		FLOG_MSG("[cksettling>> while parent2]");
		_exit(0);
	}
	/* Child falls through... */

    if (!(flags & BD_NO_UMASK0))
        umask(0);                       /* Clear file mode creation mask */

    if (!(flags & BD_NO_CHDIR))
        chdir("/");                     /* Change to root directory */

    if (!(flags & BD_NO_CLOSE_FILES)) { /* Close all open files */
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)                /* Limit is indeterminate... */
            maxfd = BD_MAX_CLOSE;       /* so take a guess */

        for (fd = 0; fd < maxfd; fd++)
            close(fd);
    }

    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);            /* Reopen standard fd's to /dev/null */

        fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO)         /* 'fd' should be 0 */
        {
        	FLOG_ERR("[cksettling>> fd != STDIN_FILENO]");
        	return -1;
        }
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
        {
        	FLOG_ERR("[cksettling>> dup2 >>!= STDIN_FILENO]");
            return -1;
        }
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
        {
        	FLOG_ERR("[cksettling>> dup2 2 >>!= STDIN_FILENO]");
        	return -1;
        }
    }
    FLOG_MSG("[cksettling>> becomeDaemon ok]");
    return 0;
}
