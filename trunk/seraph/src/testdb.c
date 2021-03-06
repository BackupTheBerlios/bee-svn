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

#include "config.h"
#include "dbg.h"
#include "sut.h"
#include "sock.h"
#include "scan.h"
#include "strop.h"
#include "strop.h"
#include "fileop.h"
#include "wall.h"
#include "testdb.h"

extern struct config_s cfg;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    char**
testdb_listTests(const char* td, int* sz)
{
    DIR* dir;
    struct dirent* ent;
    int i=0;
    struct stat stbuf;
    char f[PATH_MAX]={0};

    /* opendir segfaults if parameter is NULL */
    if(!td)
    {   *sz =0;
        return NULL;
    }
    char** buf =(char**)calloc(8192, sizeof(char*)); //TODO
    if( !( dir = opendir( td ) ) ) {
        dbg_error( "1: Can't open test directory [%s] : %s\n",
                td, strerror( errno ) );
	*sz = 0;
        return NULL;
    }
    while( ( ent = readdir( dir ) ) )
    {   if( !strcmp(ent->d_name,".")
        ||  !strcmp(ent->d_name,"..")
        ||  !strcmp(ent->d_name, "CVS") )
            continue;
        sprintf(f, "%s/%s", td, ent->d_name);
        if(stat( f, &stbuf))
            dbg_error("Unable to stat [%s]: [%s]\n" , f, strerror(errno));

        if( !S_ISDIR(stbuf.st_mode) )
            continue;

        buf[i] = (char*)calloc(512, sizeof(char));
        strcpy(buf[i++], ent->d_name);
    }
    *sz = i;
    closedir(dir);
    return buf;
}/*------------------------------------------------------------------*/

#warning implement libtar
bool
testdb_addUserTest( const char* uName, int fSize, const char* fData)
{
    char cmd[PATH_MAX]={0};
    sprintf( cmd, "tar xzvf - -C %s/%s/tests/", USERDB, uName);
    debug("sz[%d]\n", fSize);
    FILE* fh = popen( cmd, "w");
    fwrite( fData, fSize, 1, fh);
    debug( "%s", fData);
    fclose(fh);
    return true;/* TODO: make use of this */
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#warning Sprintf
#warning directory traversal

bool
testdb_clearRunnedJobs( const char* uName)
{
    char cmd[PATH_MAX]={0};
    sprintf( cmd, "%s/%s/jobs/running", USERDB, uName);
    fop_rm( TEST_LOCAL, cmd, 0,0);
    mkdir( cmd, 0755); /*check return val*/
    return true;
}/*------------------------------------------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    char**
testdb_listUserTests(const char* uName, int* sz)
{
    DIR* dir;
    struct dirent* ent;
    int i=0;
    struct stat stbuf;
    char f[PATH_MAX]={0};

    /* opendir segfaults if parameter is NULL */
    if(!uName)
    {   *sz =0;
        debug("Null username\n");
        return NULL;
    }
    char** buf =(char**)calloc(8192, sizeof(char*)); //TODO
    sprintf(f, "%s/%s/tests/", USERDB, uName);

    debug("listUserTests: [%s]\n", f);

    if( !( dir = opendir(f) ) ) {
        dbg_error( "1: Can't open test directory [%s] : %s\n",
                f, strerror( errno ) );
	*sz = 0;
        return NULL;
    }
    while( ( ent = readdir( dir ) ) )
    {   if( !strcmp(ent->d_name,".")
        ||  !strcmp(ent->d_name,"..")
        ||  !strcmp(ent->d_name, "CVS") )
            continue;
        sprintf(f, "%s/%s/tests/%s", USERDB, uName, ent->d_name);
        if(stat( f, &stbuf))
            dbg_error("Unable to stat [%s]: [%s]\n" , f, strerror(errno));

        if( !S_ISDIR(stbuf.st_mode) )
        {   debug("continuing [%s]\n", ent->d_name);
            continue;
        }

        buf[i] = (char*)calloc(512, sizeof(char));
        strcpy(buf[i++], ent->d_name);
    }
    *sz = i;
    closedir(dir);
    return buf;
}/*------------------------------------------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    char**
testdb_listMachines(const char* uName, int* sz)
{
    DIR* dir;
    struct dirent* ent;
    int i=0;
    char **buf, fname[PATH_MAX]={0};

    sprintf(fname,"%s/%s/machines", USERDB, uName);
    debug("LISTMACHINES: [%s]\n", fname);
    buf =(char**)calloc(8192, sizeof(char*)); /*TODO use linked list */

    if( !( dir = opendir(fname) ) ) {
        debug( "1: Can't open directory [%s] : %s\n",
                fname, strerror( errno ) );
        *sz = 0;
        return NULL ;
    }
    while( ( ent = readdir( dir ) ) ) {
        if( !strcmp(ent->d_name,".") || !strcmp(ent->d_name,"..") || !strcmp(ent->d_name, "cfg_template") ) continue;
        buf[i] = (char*)calloc(512, sizeof(char));
        strcpy(buf[i++], ent->d_name);
    }
    *sz = i;
    closedir(dir);
    return buf;
}/*------------------------------------------------------------------*/



int onLineParsed(const char *name, const char *value, void* arg)
{
    GSList** a = (GSList**)arg;
    ConfigEntry* el = calloc(1, sizeof(ConfigEntry));

    debug("%s -- %s\n", name, value);
    el->symbol = calloc(1, strlen(name)+1);
    el->value = calloc(1, strlen(value)+1);

    strcpy(el->symbol, name);
    strcpy(el->value, value);
    *a = g_slist_append (*a, el);
    return 0;
}

/*
 * TODO: figure out where to alloc the list( inside or out of the function)*/
GSList* testdb_getConfig( const char* uName, const char* machine, unsigned int* sz)
{
    char path[PATH_MAX]={0};
    GSList*   cfgTable =  g_slist_alloc() ;
    cfg.takeAction = onLineParsed ;
    sprintf(path,"%s/%s/machines/%s", USERDB, uName, machine);
    debug( "\nGETCONFIG [%s]\n", path );
    scan_parseCfg( path, &cfgTable);
    *sz = g_slist_length(cfgTable);
    return cfgTable;
}


/* TODO write something in the created file */
bool testdb_addMachine( const char* uName,
                        const char* mName,
                        const char* mOS,
                        const char* mOSVer,
                        const char* mIP)
{
    char fname[PATH_MAX]={0}, cfgTemplate[PATH_MAX]={0};

    debug( "\nADDMACHINE [%s][%s][%s][%s]\n",mName, mOS, mOSVer, mIP );
    sprintf(fname,"%s/%s/machines/%s", USERDB, uName, mName);
    sprintf(cfgTemplate,"%s/cfg_template", MACHINES);

    if( !fop_cp( cfgTemplate, fname) )
        return false;
    return true;
}
