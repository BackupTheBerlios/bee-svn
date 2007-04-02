/*
 * Server-side remote procedure call implemented over jabber.
 */

int jstartCallback char *buf ) {
        int timeout = 0, rval = 0;
        char *startCmd = NULL;
        char *maillog;

        if( NULL == buf ) {
                fprintf( stderr, "! START: (null) internal `buf` parameter\n" );
                return 0;
        }
        rewindParams(  );
        timeout = getValueInt(  );

        nextParam(  );
        maillog = getValueString(  );

        nextParam(  );
        startCmd = getValueString(  );

        printf( "START %d [%s][%s]\n", timeout, maillog, startCmd );
        rval = sut_start( TEST_LOCAL, timeout, maillog, startCmd, 0, 0 );
        return rval;
}


int jlstestsCallback ) {
        opendir(  );
        while( readdir(  ) ) {
                append_name( list );
        }
        return list;
}


int jstopCallback int sock, char *buf ) {
        int timeout = 0, rb;
        char *stopCmd = NULL;
        char maillog[LINE_MAX] = {
        0};

        if( NULL == buf ) {
                fprintf( stderr, "! STOP: (null) internal `buf` parameter\n" );
                return 0;
        }
        rewindParams(  );
        timeout = getValueInt(  );
        maillog = getValueString(  );
        stopCmd = getValueString(  );

        printf( "STOP %d [%s][%s]\n", timeout, maillog, stopCmd );
        rval = sut_stop( TEST_LOCAL, timeout, maillog, stopCmd, 0, 0 );
        return rval;
}

/**
 * \todo Replace this with a newer version 
 */
int jrefreshCallback int sock, char *buf ) {
        char com[LINE_MAX] = {
        0};
        struct stat stats;
        char cale_s[LINE_MAX] = {
        0};
        char cale_d[LINE_MAX] = {
        0};
        int cod = 0, rb = 0;

        if( NULL == buf ) {
                fprintf( stderr,
                         "! REFRESH: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%s %s", cale_s, cale_d );

        cod = stat( cale_s, &stats );
        if( cod != 0 ) {
                printf( "rsh: ERR: Can't find %s\n", cale_s );
                exit( -1 );
        }
        sprintf( com, "rm -rf %s", cale_d );
        if( system( com ) == -1 ) {
                return errno;
        }
        sprintf( com, "/bin/cp -r %s %s", cale_s, cale_d );
        if( system( com ) == -1 ) {
                return errno;
        }

        sock_sendStatus( sock, 0 );
        return 0;
}



/**
 * \brief Read a stream for socket, and writes it to dest_dir/src_file.
 */
int jcopyCallback int sock, char *buf ) {
        int f, bw, len, rb;     /* bytes written to disk */
        char buff[LINE_MAX] = {
        0};
        char dest_file[FILENAME_MAX] = {
        0};
        char src_file[FILENAME_MAX] = {
        0};
        char dest_dir[FILENAME_MAX] = {
        0};

        if( NULL == buf ) {
                fprintf( stderr, "! COPY: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%s %s %d", src_file, dest_dir, &len );

        if( rb != 3 || len <= 0 ) {
                printf( "Invalid syntax for COPY\n" );
                return FALSE;
        }

        sprintf( dest_file, "%s/%s", dest_dir, src_file );
        f = creat( dest_file, S_IRUSR | S_IWUSR );
        if( f < 0 ) {
                sock_sendStatus( sock, errno );
                return errno;
        }
        while( len > 0 && ( bw = read( sock, buff, LINE_MAX - 1 ) ) ) {
                int ret = write( f, buff, bw < len ? bw : len );
                if( bw < 0 ) {
                        perror( "Transmission error" );
                        close( f );
                        return errno;
                }
                if( ret < 0 ) {
                        close( f );
                        return errno;
                }
                len -= bw;
        }

        sock_sendStatus( sock, 0 );
        close( f );
        return 0;
}



int jrmCallback int sock, char *buf ) {
        char cmd[LINE_MAX] = {
        0};
        int rb;
        char path[LINE_MAX] = {
        0};

        if( NULL == buf ) {
                fprintf( stderr, "! RM: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%s", path );

        if( rb != 1 ) {
                printf( "Invalid syntax for RM\n" );
                return FALSE;
        }

        sprintf( cmd, "/bin/rm -rf %s", path );
        sock_sendStatus( sock, 0 );     /* fix this pls */
        return system( cmd );
}



int jmkdirCallback int sock, char *buf ) {
        int rb;
        char path[LINE_MAX] = {
        0};

        if( NULL == buf ) {
                fprintf( stderr, "! MKDIR: (null) internal `buf` parameter\n" );
                return 0;
        }
        rb = sscanf( buf, "%s", path );

        if( rb != 1 ) {
                printf( "Invalid syntax for MKDIR\n" );
                return FALSE;
        }

        if( mkdir( path, 0777 ) == -1 ) {
                printf( "mkdir: ERR:[%s]", strerror( errno ) );
                sock_sendStatus( sock, errno );
                return errno;
        }
        sock_sendStatus( sock, 0 );
        return 0;
}

/**
 * @todo Look at strchr
 */
int jexecuteCallback int sock, char *buf ) {
        char *p = NULL;
        int rc = 0;

        p = buf;
        if( !p ) {
                printf( "ERR: null command\n" );
                return -1;
        }
        rc = system( p );
        sock_sendStatus( sock, 0 );
        return 0;
}



int jcheckCoreCallback int sock, char *buf ) {
        char core_srcDir[FILENAME_MAX] = {
        0};
        char dbg_srcDir[FILENAME_MAX] = {
        0};
        char axi_workDir[FILENAME_MAX] = {
        0};
        char axi_cfgFile[FILENAME_MAX] = {
        0};
        char crash_destDir[FILENAME_MAX] = {
        0};
        int rc = FALSE;

        if( NULL == buf ) {
                fprintf( stderr,
                         "! CHECKCORE (null) internal `buf` parameter\n" );
                return 0;
        }
        rewindParams(  );
        timeout = getValueInt(  );
        maillog = getValueString(  );
        stopCmd = getValueString(  );

        printf( "CORE %s %s %s %s %s",
                core_srcDir, dbg_srcDir, axi_workDir,
                axi_cfgFile, crash_destDir );
        rc = sut_checkCore( TEST_LOCAL,
                            core_srcDir, dbg_srcDir, axi_workDir,
                            axi_cfgFile, crash_destDir );
        return rc;              /* means no core was found */
}
