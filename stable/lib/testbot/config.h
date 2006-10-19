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
#include <pcre.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

#define SUT_TTYPE   "axi_ttype"
#define SUT_HOST    "axi_host"
#define SUT_PORT    "axi_port"
#define SUT_WORKDIR "axi_workdir"
#define SUT_STOP    "axi_stop"
#define SUT_START   "axi_start"
#define SUT_RESTART "axi_restart"
#define SUT_TOOL    "axi_tool"
#define SUT_DEFDOM  "axi_fresh"    /* Default configuration, meaning users and domains */
#define SUT_COREDIR "axi_coredir"
#define SUT_DBGDIR  "axi_dbgdir"
#define SUT_CFGFILE "axi_cfgfile"
#define SUT_SYSLOG  "axi_syslog"
#define EXT_EXEC    ".bat"
#define PLATFORMS 3

#define VER "0.9.1"

#define TEST_UNSET  0
#define TEST_LOCAL  1
#define TEST_REMOTE 2
#define OPT_YES 1
#define OPT_NO  2
#define OPT_ASK 3
#define TRUE  1
#define FALSE 0


struct config_s {
    int     test_type ;
    char*   test_dir ;
    char    tmp_dir[PATH_MAX];
    char**  argv ;
    int     argc ;
    int     refresh ;
    char*   config_file;
    char    cur_path[PATH_MAX];      //! retine variabila PATH inainte de rescriere
    char    cur_dir[PATH_MAX];       //! retine directorul curent, inainte k sa il schimb in /tmp/pid
    char    cur_test[PATH_MAX];
    char    testbot_path[PATH_MAX] ; //! Path to testbot dir
    char*   hostname;
    int     port ;
    char*   axi_coreDir ;            //! Path to core dir
    char*   axi_workDir ;
    char    axi_bin[PATH_MAX] ;
    char*   axi_cfgFile;
    char*   axi_dbgDir ;
    char*   axi_syslog ;
    char*   dest_coreDir ;
    int     script_tout ;            //! script timeout in seconds. after this expires, the script is killed
    bool    act_as_daemon;
    bool    verbose ;
    bool    allways_kill;
    char*   platf[] ;                /*= { {"linux"}, {"bsd"}, {"windows"} }; */
} ;

#endif
