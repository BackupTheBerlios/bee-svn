#ifndef FILEOP_H
#define FILEOP_H

bool fop_cp( const char* const s, const char* const d);
int
fop_fileSize( char *name );
int
fop_rm( const int test_type, const char *path,const char *host,const int port );
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
