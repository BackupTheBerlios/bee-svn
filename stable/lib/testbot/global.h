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

#define PT_TTYPE   "pt_ttype"
#define PT_HOST    "pt_host"
#define PT_PORT    "pt_port"
#define PT_WORKDIR "pt_workdir"
#define PT_STOP    "pt_stop"
#define PT_START   "pt_start"
#define PT_RESTART "pt_restart"
#define PT_TOOL    "pt_tool"
#define PT_DEFDOM  "pt_fresh"    /* Default configuration, meaning users and domains */
#define PT_COREDIR "pt_coredir"
#define PT_DBGDIR  "pt_dbgdir"
#define PT_CFGFILE "pt_cfgfile"
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
