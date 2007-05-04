/**
 *   \brief    Global declarations
 *   \author   
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c)  
 */
#if !defined( _CONFIG_H_ )
#define _CONFIG_H_


#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <libgen.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/wait.h>
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
#include <time.h>

#define SUT_CFGFILE "SUT_CFGFILE"
#define SUT_COREDIR "SUT_COREDIR"
#define SUT_DBGDIR  "SUT_DBGDIR"
#define SUT_DEFWD   "SUT_DEFWD"    /* default workdir */
#define SUT_ERRLOG  "SUT_ERRLOG"
#define SUT_HOST    "SUT_HOST"
#define SUT_PORT    "SUT_PORT"
#define SUT_RESTART "SUT_RESTART"
#define SUT_START   "SUT_START"
#define SUT_STOP    "SUT_STOP"
#define SUT_SYSLOG  "SUT_SYSLOG"
#define SUT_TOOL    "SUT_TOOL"
#define SUT_TTYPE   "SUT_TTYPE"
#define SUT_WORKDIR "SUT_WORKDIR"
#define EXT_EXEC    ".bat"

#define VER "2.0.0"

/*#define PATH_MAX 2048*/
#define TEST_UNSET  0
#define TEST_LOCAL  1
#define TEST_REMOTE 2
#define OPT_YES 1
#define OPT_NO  2
#define OPT_ASK 3
/* TODO: fa-le enum-uri*/
#define TB_BE_DAEMON 1
#define TB_BE_SETUP 2
#define TB_BE_TESTER 3


struct config_s {
    bool    allways_kill;
    bool    start_jabber;
    bool    start_rawrpc;
    bool    start_xmlrpc;
    bool    verbose ;
    char    axi_bin[FILENAME_MAX] ;
    char    cur_dir[FILENAME_MAX];      /* retine directorul curent, inainte k sa il schimb in /tmp/pid*/
    char    cur_test[FILENAME_MAX];
    char    seraph_path[FILENAME_MAX] ;/* Path to seraph dir*/
    char    tmp_dir[FILENAME_MAX];
    char*   axi_cfgFile;
    char*   axi_coreDir ;               /* Path to core dir*/
    char*   axi_dbgDir ;
    char*   axi_syslog ;
    char*   axi_workDir ;
    char*   config_file;
    char*   cur_path;                   /* retine variabila PATH inainte de rescriere */
    char*   dest_coreDir ;
    char*   hostname;
    char*   jabuser ;
    char*   machine;
    char*   notifyMail;
    char*   test_dir ;
    char*   sutDefWD ;
    char**  argv ;
    int     argc ;
    int     behaviour;
    int     rawport ;
    int     refresh ;
    int     script_tout ;               /* script timeout in seconds. after this expires, the script is killed */
    int     test_type ;
    int     xmlport ;
    int     (*takeAction)(const char *name, const char *value, void* overwrite) ;
} ;

#else
        #warning "*** Header allready included ***"
#endif
