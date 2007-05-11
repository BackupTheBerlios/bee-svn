/*
 * Copyright (C) 2006, 2007 Free Software Foundation, Inc.
 * Written by Negreanu Marius <groleo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING.  If not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 *   \brief  execute a given parameter, either on local machine, or remote
 */
#include "config.h"
#include "dbg.h"
#include "sock.h"
#include "strop.h"
#include "sut.h"

static bool
rexec_remote( char *host, int port, char *c );

#define TOOL_NAME rexec
#define HAVE_CMD_ARG 1
#define TOOL_DESCRIPTION "Start the product under test.\n"
#define CMD_FUNC\
        ret  = rexec_remote( cfg.hostname, cfg.rawport, cmd )

#include "template.c"

static bool
rexec_remote( char *host, int port, char *c )
{
    int     ret, sockfd;
    char    *cmd=NULL;

    dbg_verbose("rexec\n");
    cmd = (char*)malloc( strlen(c) + 10);
    sockfd = sock_connectTo( host, port );
    if(sockfd==-1)
        return false;
    sprintf( cmd, "EXECUTE %s", c );
    dbg_verbose( "[%s]\n", cmd );
    sock_sendLine( sockfd, cmd );
    ret = sock_getStatus( sockfd );
    if( ret < 0 )
    {   dbg_error( "rexec: No remote confirmation!\n");
        free(cmd);
        return false;
    }

    if( ret > 0 )
    {   dbg_error("rexec: Error: '%s'\n", strerror( ret ) );
        dbg_error("rexec: Can't execute [%s].\n", c );
        free(cmd);
        shutdown( sockfd, 2);
        close( sockfd );
        return false;
    }
    free(cmd);
    shutdown( sockfd, 2);
    close( sockfd );
    return true;
}
