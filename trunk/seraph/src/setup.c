/*
 * Installs seraph on remote machines
 */

static bool parse( const char *machine, int *os, char **scpArg )
{
        char *p = 0;
        p = strchr( machine, '#' );
        if( !p )
                return false;
        *p = '\0';
        p++;
        *scpArg = p;
        if( stricmp( machine, "openbsd" ) )
                *os = ST_OPENBSD;
        else if( stricmp( machine, "freebsd" ) )
                *os = ST_FREEBSD;
        else if( stricmp( machine, "netbsd" ) )
                *os = ST_NETBSD;
        else if( stricmp( machine, "linux" ) )
                *os = ST_LINUX;
        return true;
}

int setup( char *machine )
{
        int os = 0;
        char *scpDst = 0, scpSrc = 0;
        if( !parse( machines, &os, &scpDst ) ) {
                printf( "Invalid -S parameters\n" );
                return false;
        }
        switch ( os ) {
        case ST_OPENBSD:
                scpSrc = "./lib/OpenBSD/";
                break;
        }

        // scp -r ./lib/OpenBSD/ localhost:/home/groleo/seraph/
        // ln -sf /home/groleo/seraph/lib/OpenBSD/seraph /home/groleo/seraph/seraph
        execlp( "scp", "scp", "-r", scpSrc, scpDst );
        return true;

}
