#ifndef FILEOP_H
#define FILEOP_H

#include "config.h"

int
fop_fileSize( char *name );
int
fop_rm( char *srcName ) ;
int
recursiveAction( const char *fileName,
                 int recurse, int followLinks, int depthFirst,
                 int ( *fileAction ) ( const char *fileName,
                                       struct stat * statbuf,
                                       void *userData ),
                 int ( *dirAction ) ( const char *fileName,
                                      struct stat * statbuf,
                                      void *userData ), void *userData );

#endif
