/**
 *   \brief    Global declarations
 *   \author   Cristina Balan, Andrei Paduraru 
 *   \date     Thu Aug 17 17:38:13 2006
 *
 *   Copyright (c) Gecad Technologies
 */
#if !defined( GLOBAL_H )
#define GLOBAL_H


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

#include "debug.h"

#define SUT_TTYPE   "SUT_TTYPE"
#define SUT_HOST    "SUT_HOST"
#define SUT_PORT    "SUT_PORT"
#define SUT_WORKDIR "SUT_WORKDIR"
#define SUT_STOP    "SUT_STOP"
#define SUT_START   "SUT_START"
#define SUT_RESTART "SUT_RESTART"
#define SUT_TOOL    "SUT_TOOL"
#define SUT_DEFDOM  "SUT_FRESH"    /* Default configuration, meaning users and domains */
#define SUT_COREDIR "SUT_COREDIR"
#define SUT_DBGDIR  "SUT_DBGDIR"
#define SUT_CFGFILE "SUT_CFGFILE"
#define SUT_SYSLOG  "SUT_SYSLOG"
#define SUT_ERRLOG  "SUT_ERRLOG"
#define EXT_EXEC    ".bat"
#define PLATFORMS 3

#define VER "2.0.0"

/*#define LINE_MAX 2048*/
#define TEST_UNSET  0
#define TEST_LOCAL  1
#define TEST_REMOTE 2
#define OPT_YES 1
#define OPT_NO  2
#define OPT_ASK 3
#define TRUE  1
#define FALSE 0
/* TODO: fa-le enum-uri*/
#define TB_BE_DAEMON 1
#define TB_BE_SETUP 2
#define TB_BE_TESTER 3


struct config_s {
    int     test_type ;
    char*   test_dir ;
    char**  argv ;
    int     argc ;
    int     refresh ;
    int     port ;
    char    tmp_dir[FILENAME_MAX];
    char    cur_dir[FILENAME_MAX];      /* retine directorul curent, inainte k sa il schimb in /tmp/pid*/
    char    cur_test[FILENAME_MAX];
    char    seraph_path[FILENAME_MAX] ;/* Path to seraph dir*/
    char*   cur_path;                   /* retine variabila PATH inainte de rescriere */
    char*   config_file;
    char*   hostname;
    char    axi_bin[FILENAME_MAX] ;
    char*   axi_coreDir ;               /* Path to core dir*/
    char*   axi_workDir ;
    char*   axi_cfgFile;
    char*   axi_dbgDir ;
    char*   axi_syslog ;
    char*   dest_coreDir ;
    char*   machine;
    int     script_tout ;               /* script timeout in seconds. after this expires, the script is killed */
    int     behaviour;
    bool    verbose ;
    bool    allways_kill;
    bool    start_rawrpc;
    bool    start_xmlrpc;
    bool    start_jabber;
    char*   jabuser ;
    char*   notifyMail;
    int     (*takeAction)(const char *name, const char *value, void* overwrite) ;
} ;

#else
        #warning "*** Header allready included ***"
#endif
