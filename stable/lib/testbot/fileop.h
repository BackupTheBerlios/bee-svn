#ifndef FILEOP_H
#define FILEOP_H

#include <wait.h>
#include "config.h"

int
fop_fileSize( char *name );
int
fop_rm( char *srcName ) ;
static int   fop_rmRemote( char* path, char* host, int port);
static int   fop_rmLocal( char* path);
static int
rm_fileAction( const char *fileName, struct stat *statbuf, void *junk ) ;

static int
rm_dirAction( const char *fileName, struct stat *statbuf, void *junk );
#endif
