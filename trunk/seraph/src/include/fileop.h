#ifndef FILEOP_H
#define FILEOP_H

int
fop_fileSize( char *name );
int
fop_rm( int test_type, char *path, char *host, int port );
int
recursiveAction( const char *fileName,
                 int recurse, int followLinks, int depthFirst,
                 int ( *fileAction ) ( const char *fileName,
                                       struct stat * statbuf,
                                       void *userData ),
                 int ( *dirAction ) ( const char *fileName,
                                      struct stat * statbuf,
                                      void *userData ), void *userData );

#else
        #warning "*** Header allready included ***"
#endif
