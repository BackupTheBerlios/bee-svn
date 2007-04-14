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

/*
 * Installs seraph on remote machines
 */

static bool parse( const char *machine, int *os, char **scpArg )
{
        char *p = 0;
        p = strchr( machine, '#' );
        if( !p )
                return false;
        *p = '\0';
        p++;
        *scpArg = p;
        if( stricmp( machine, "openbsd" ) )
                *os = ST_OPENBSD;
        else if( stricmp( machine, "freebsd" ) )
                *os = ST_FREEBSD;
        else if( stricmp( machine, "netbsd" ) )
                *os = ST_NETBSD;
        else if( stricmp( machine, "linux" ) )
                *os = ST_LINUX;
        return true;
}

int setup( char *machine )
{
        int os = 0;
        char *scpDst = 0, scpSrc = 0;
        if( !parse( machines, &os, &scpDst ) ) {
                printf( "Invalid -S parameters\n" );
                return false;
        }
        switch ( os ) {
        case ST_OPENBSD:
                scpSrc = "./lib/OpenBSD/";
                break;
        }

        // scp -r ./lib/OpenBSD/ localhost:/home/groleo/seraph/
        // ln -sf /home/groleo/seraph/lib/OpenBSD/seraph /home/groleo/seraph/seraph
        execlp( "scp", "scp", "-r", scpSrc, scpDst );
        return true;

}
