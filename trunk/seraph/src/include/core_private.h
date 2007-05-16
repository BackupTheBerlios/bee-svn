#ifndef _CORE_PRIVATE_H
#define _CORE_PRIVATE_H

/*Don't include this header.
 * core.h does that.*/
static bool
core_checkCoreRemote( const char *dumpSrcDir, const char *dbgSrcDir,
                      const char *sutWorkDir, const char *sutCfgFile,
                      const char *dumpDestDir );
static bool
core_checkCoreLocal( const char *dumpSrcDir, const char *dbgSrcDir,
                     const char *workDir, const char *cfgFile,
                     const char *dumpDestDir );

static int
core_runBat( const char *batName, int timeout );

static int
core_setupTmp( char const *sourceBat, char *tmpDir );

static int
core_parseBat( const char *filename );

static int
core_dirAction( const char *fileName, struct stat *statBuf,
                           void *junk );

static int
core_fileAction( const char *fileName, struct stat *statBuf,
                            void *junk );

static int
core_cleanupTmp( const char *tmpDir );

static int
core_runRecursive( const char *srcName );

static bool
core_setupDstDir( char *dst, char *coreName,
                  const char *dumpSrcDir, const char *core_dstDir );

static bool
core_moveCore( char *src, char *dst );

static bool
core_moveDebugs( const char *srcDir, char *dst );

static bool
core_moveLog( const char *workDir, char *dst );

static bool
core_copyCfg( const char *cfgFile, char *dst );

static void
killChildren(gpointer data, gpointer user_data);
#endif
