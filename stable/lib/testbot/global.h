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

#define AXI_TTYPE   "axi_ttype"
#define AXI_HOST    "axi_host"
#define AXI_PORT    "axi_port"
#define AXI_WORKDIR "axi_workdir"
#define AXI_STOP    "axi_stop"
#define AXI_START   "axi_start"
#define AXI_RESTART "axi_restart"
#define AXI_TOOL    "axi_tool"
#define AXI_DEFDOM  "axi_fresh"    /* Default configuration, meaning users and domains */
#define AXI_COREDIR "axi_coredir"
#define AXI_DBGDIR  "axi_dbgdir"
#define AXI_CFGFILE "axi_cfgfile"
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
    char*  axi_coreDir ;            //! Path to core dir
    char*  axi_workDir ;
    char   axi_bin[PATH_MAX] ;
    char*  axi_cfgFile;
    char*  axi_dbgDir ;
    char*  dest_coreDir ;
    int    verbose ;
    int    script_tout ;            //! script timeout in seconds. after this expires, the script is killed
    char*  platf[] ;                /*= { {"linux"}, {"bsd"}, {"windows"} }; */
} ;

#endif
