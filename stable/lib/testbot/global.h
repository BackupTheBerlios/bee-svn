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

#define PUT_TTYPE   "pt_ttype"
#define PUT_HOST    "pt_host"
#define PUT_PORT    "pt_port"
#define PUT_WORKDIR "pt_workdir"
#define PUT_STOP    "pt_stop"
#define PUT_START   "pt_start"
#define PUT_RESTART "pt_restart"
#define PUT_TOOL    "pt_tool"
#define PUT_DEFDOM  "pt_fresh"    /* Default configuration, meaning users and domains */
#define PUT_COREDIR "pt_coredir"
#define PUT_DBGDIR  "pt_dbgdir"
#define PUT_CFGFILE "pt_cfgfile"
#define EXT_EXEC    ".bat"
#define PLAT 3

#define VER "0.9.1"

#define DIM_BUFF 1024
#define MAX_LIN  750
#define BSIZE 1024
#define TEST_UNSET  0
#define TEST_LOCAL  1
#define TEST_REMOTE 2
#define OPT_YES 1
#define OPT_NO  2
#define OPT_ASK 3
#define TRUE 1
#define FALSE 0


struct globals_s {
    int    port ;
    int    test_type ;
    char*  test_dir ;
    char   tmp_dir[PATH_MAX];
    char*  hostname;
    char** argv ;
    int    argc ;
    int    refresh ;
    int    act_as_daemon;
    char*  config_file;
    char   cur_path[PATH_MAX];      //! retine variabila PATH inainte de rescriere
    char   cur_dir[PATH_MAX];       //! retine directorul curent, inainte k sa il schimb in /tmp/pid
    char   testbot_path[PATH_MAX] ; //! Path to testbot dir
    char*  pt_coreDir ;            //! Path to core dir
    char*  pt_workDir ;
    char   pt_bin[PATH_MAX] ;
    char*  pt_cfgFile;
    char*  pt_dbgDir ;
    char*  dest_coreDir ;
    int    verbose ;
    int    script_tout ;            //! script timeout in seconds. after this expires, the script is killed
    char*  platf[] ;                /*= { {"linux"}, {"bsd"}, {"windows"} }; */
} ;

#endif
