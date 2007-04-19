#include "config.h"
#include <sys/wait.h>
#include <sys/types.h>

#include "sut.h"
#include "socket.h"
#include "strop.h"
#include "debug.h"
#include "rshd.h"
#include "strop.h"
#include "fileop.h"
#include "wall.h"
#include "testdb.h"

#include <glib.h>
#include <libgen.h>
#include <sys/wait.h>
#include <limits.h>
#include <time.h>


extern struct config_s cfg;
int running ;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    char**
testdb_listTests(const char* td, int* sz)
{
    DIR* dir;
    struct dirent* ent;
    int i=0;

    char** buf =(char**)calloc(8192, sizeof(char*)); //TODO
    if( !( dir = opendir( td ) ) ) {
        debug( "1: Can't open test directory [%s] : %s\n",
                td, strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    while( ( ent = readdir( dir ) ) ) {
        if( !strcmp(ent->d_name,".") || !strcmp(ent->d_name,"..") ||!strcmp(ent->d_name, "CVS") )
            continue;
        buf[i] = (char*)calloc(512, sizeof(char));
        strcpy(buf[i++], ent->d_name);
    }
    *sz = i;
    return buf;
}/*------------------------------------------------------------------*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    char**
testdb_listMachines(const char* td, int* sz)
{
    DIR* dir;
    struct dirent* ent;
    int i=0;
    char** buf;

    debug("LISTMACHINES: [%s]\n", td);
    buf =(char**)calloc(8192, sizeof(char*)); /*TODO use linked list */

    if( !( dir = opendir(td) ) ) {
        debug( "1: Can't open directory [%s] : %s\n",
                td, strerror( errno ) );
        *sz = 0;
        return NULL ;
    }
    while( ( ent = readdir( dir ) ) ) {
        if( !strcmp(ent->d_name,".") || !strcmp(ent->d_name,"..") ) continue;
        buf[i] = (char*)calloc(512, sizeof(char));
        strcpy(buf[i++], ent->d_name);
    }
    *sz = i;
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


GSList* testdb_getConfig(const char* machine, int* sz)
{
    char path[PATH_MAX]={0};
    debug( "\nGETCONFIG\n" );
    GSList*   cfgTable =  g_slist_alloc() ;
    cfg.takeAction = onLineParsed ;
    sprintf(path,"%s/%s", MACHINES, machine);
    srph_parseCfg( path, &cfgTable);
    *sz = g_slist_length(cfgTable);
    return cfgTable;
}


/* TODO write something in the created file */
bool testdb_addMachine(const char* mName,
                    const char* mOS,
                    const char* mOSVer,
                    const char* mIP)
{
    char fname[PATH_MAX]={0}, cfgTemplate[PATH_MAX]={0};

    debug( "\nADDMACHINE [%s][%s][%s][%s]\n",mName, mOS, mOSVer, mIP );
    sprintf(fname,"%s/%s", MACHINES, mName);
    sprintf(cfgTemplate,"%s/cfg_template", MACHINES);

    if( !fop_cp( cfgTemplate, fname) )
        return false;
    return true;
}


bool
testdb_register( const char* const name, const char* const email,
                const char* const username, const char* const password)
{
    return true;
}


bool
testdb_checkSession(const char* const username,
                    const char* const cookie,
                    const char* const session,
                    const char* const ip )
{
}

bool
testdb_setSession(  const char* const id,
                    const char* const session,
                    const char* const ip){}

bool
testdb_checkLogin(  const char* const username,
                    const char* const password){}

bool
testdb_checkRemembered( const char* const username,
                        const char* const cookie ) {}
