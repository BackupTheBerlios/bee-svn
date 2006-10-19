#ifndef FILEOP_H
#define FILEOP_H

#include <wait.h>
#include "config.h"

int
fop_fileSize( char *name );

int
fop_rm( char *srcName ) ;

#endif
