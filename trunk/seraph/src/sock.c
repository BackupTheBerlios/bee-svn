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
 *   \brief    Socket operations.
 *   \see      util.c 
 *   \author   
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c)  
 */
#include "socket.h"
#include "config.h"
#include <limits.h>


int sock_connectTo( const char *host, const int port )
{
        struct sockaddr_in serv_addr;   /* structure used for connection */
        int sockfd;             /* socket descriptor */
        struct hostent *server;


        if( !host || !port ) {
                printf( "SOCKET:Cant connect to host(%s) port(%d)\n", host, port );
                return -1;
        }
        /* socket create */
        sockfd = socket( AF_INET, SOCK_STREAM, 0 );
        if( sockfd == -1 ) {
                perror( "ERR: can't create socket" );
                return -1;
        }
        /* connected with server */
        server = gethostbyname( host );
        if( server == NULL ) {
                fprintf( stderr, "Error, no such host\n" );
                return -1;
        }

        memset( ( char * )&serv_addr, 0, sizeof( serv_addr ) );
        serv_addr.sin_family = AF_INET;
        bcopy( ( char * )server->h_addr, ( char * )&serv_addr.sin_addr.s_addr,
               server->h_length );
        serv_addr.sin_port = htons( port );

        if( connect
            ( sockfd, ( const struct sockaddr * )&serv_addr,
              sizeof( serv_addr ) )
            < 0 ) {
                perror( "Socket Error" );
                return -1;
        }
        return sockfd;
}


/**
 * send on socket ERR: errno
 * or OK: Command successful*/
int sock_sendStatus( const int sock, const int cod )
{
        char str[PATH_MAX] = { 0 };
        ssize_t t = 0;

        debug("cod(%d)\n", cod);
        if( !cod ) {
                sprintf( str, "O:\r\n" );
        } else {
                sprintf( str, "E:%d\r\n", cod );
        }
        t = write( sock, str, strlen( str ) );
        if( t < 0 || t < strlen( str ) ) {
                fprintf( stderr, "ERROR writing on socket : %s\n",
                         strerror( errno ) );
                exit( -1 );
        }
        return cod;
}

int sock_getStatus( const int sock )
{
    char buf[PATH_MAX + 1] = { 0 };
    int status = 0;
    if( read( sock, buf, PATH_MAX ) ==-1)
        return -1;

    if( buf[0] == 'O' )
        return 0;
    if( buf[0] == 'E' )
        sscanf( buf, "E:%d\r\n", &status );
    return status;
}
/* TODO: revise this */
int sock_sendLine( const int sock, const char *line )
{
        int t;
        char tr[PATH_MAX] = { 0 };

        sprintf( tr, "%s\r\n", line );
        t = write( sock, tr, strlen( tr ) );
        if( t < 0 || t < strlen( tr ) ) {
                fprintf( stderr, "ERROR writing on socket : %s\n",
                         strerror( errno ) );
                return false;
        }
        return true;
}
