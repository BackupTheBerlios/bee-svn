/**
 *   \brief    Global declarations
 *   \author   
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c)
 */
#if !defined( _CONFIG_H_ )
#define _CONFIG_H_

#if !defined(VERSION)
#define VERSION "1.0.0"
#endif


#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <libgen.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SUT_CFGFILE "SUT_CFGFILE"
#define SUT_COREDIR "SUT_COREDIR"
#define SUT_DBGDIR  "SUT_DBGDIR"
#define SUT_DEFWD   "SUT_DEFWD"    /* default workdir */
#define SUT_ERRLOG  "SUT_ERRLOG"
#define SUT_HOST    "SUT_HOST"
#define SUT_PORT    "SUT_PORT"
#define SUT_RESTART "SUT_RESTART"
#define SUT_REFRESH "SUT_REFRESH"
#define SUT_START   "SUT_START"
#define SUT_STOP    "SUT_STOP"
#define SUT_SYSLOG  "SUT_SYSLOG"
#define SUT_TOOL    "SUT_TOOL"
#define SUT_TTYPE   "SUT_TTYPE"
#define SUT_WORKDIR "SUT_WORKDIR"
#define SUT_DESTCOREDIR "SUT_DESTCOREDIR"
#define SUT_VERBOSE "SUT_VERBOSE"

/*#define PATH_MAX 2048*/
enum { OPT_NO, OPT_YES, OPT_ASK } ;
enum { TEST_UNSET, TEST_LOCAL, TEST_REMOTE } ;
enum { TB_BE_DAEMON, TB_BE_SETUP, TB_BE_TESTER } ;
enum { NOTIFY_MAIL=1, NOTIFY_JABBER=2 };


struct config_s {
    bool    allwaysKill;
    bool    startJabber;
    bool    startRawRPC;
    bool    startXmlRPC;
    char    curTest[FILENAME_MAX];
    bool    verbose ;
    char*   sutCfgFile;
    char*   sutCoreDir ;               /* Path to core dir*/
    char*   sutDbgDir ;
    char*   maillog ;
    char*   sutWorkDir ;
    char*   configFile;
    char*   oldPATH;                   /* retine variabila PATH inainte de rescriere */
    char*   destCoreDir ;
    char*   hostname;
    char*   machine;
    char*   notifyMail;
    char*   ntfmail;
    char*   testDir ;
    char*   sutDefWD ;
    char*   username;
    char**  argv ;
    int     notifyType;
    int     argc ;
    int     behaviour;
    int     refresh ;
    int     scriptTout ;               /* script timeout in seconds. after this expires, the script is killed */
    int     testType ;
    int     rawport ;
    int     xmlport ;
    int     (*takeAction)(const char *name, const char *value, void* overwrite) ;
    #if 0
    /* Removed */
    char    seraph_path[FILENAME_MAX] ;/* Path to seraph dir*/
    char    tmp_dir[FILENAME_MAX];
    char*   jabuser ;
    #endif
    GSList**children;
} ;

#else
        #warning "*** Header allready included ***"
#endif
