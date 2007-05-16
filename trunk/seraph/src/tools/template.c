#define DBGNAME1(x) #x".debug"
#define DBGNAME(x) DBGNAME1(x)
#define XQUOTE(x) #x
#define QUOTE(x) XQUOTE(x)

struct  config_s cfg;
char    *cmd = NULL;
static int parseArgs( int argc, char *argv[] );

static void usage( int status );

int
main( int argc, char *argv[] )
{
    char    *tc;
    int     testType = TEST_LOCAL, ret;

    DBG(DBGNAME(TOOL_NAME));
    parseArgs( argc, argv );
    str_isEnv( SUT_TTYPE );
    tc = getenv( SUT_TTYPE );

    if( argc<2 )
    {   dbg_error( QUOTE(TOOL_NAME)" missing operand\n" );
        dbg_error( "Try "QUOTE(TOOL_NAME)" -h for more information.\n");
        UNDBG;
        usage( EXIT_FAILURE );
    }
    cfg.verbose = getenv(SUT_VERBOSE);

    if( !strcasecmp(tc, "local") )
    {   dbg_verbose( QUOTE(TOOL_NAME)"Working Local\n" );
        testType = TEST_LOCAL;
    } else if( !strcasecmp(tc, "remote") )
    {   dbg_verbose( QUOTE(TOOL_NAME)"Working Remote\n" );
        str_isEnv( SUT_HOST );
        str_isEnv( SUT_PORT );
        testType = TEST_REMOTE;
        cfg.hostname = getenv( SUT_HOST );
        cfg.rawport = atoi( getenv( SUT_PORT ) );
    } else
    {   dbg_error( QUOTE(TOOL_NAME)"Invalid test type $SUT_TTYPE\n" );
        UNDBG;
        exit( EXIT_FAILURE);
    }
    CMD_FUNC;
    UNDBG;
    exit( ret);
}



static void
usage( int status )
{
    printf( "Usage: "QUOTE(TOOL_NAME)" [OPTION] COMMAND...\n" );
    printf( TOOL_DESCRIPTION );
    printf( "\n" );
    printf( "  -v, --verbose     print a message for each action executed\n" );
    printf( "  -h, --help        display this help and exit\n" );
    printf( "  -H host\n" );
    printf( "  -P port\n" );
    printf( "  -c params\n" );
    printf( "  -t testType\n" );
    exit( status );
}



static int
parseArgs( int argc, char *argv[] )
{
    int     c;
    while( ( c = getopt( argc, argv, "t:H:P:c:hv" ) ) != -1 )
    {   switch ( c ) {
            case 't':
                if( !strcasecmp( optarg, "remote" ) )
                {   setenv( SUT_TTYPE, optarg, 1 );
                    cfg.testType = TEST_REMOTE;
                }
                if( !strcasecmp( optarg, "local" ) )
                {   setenv( SUT_TTYPE, optarg, 1 );
                    cfg.testType = TEST_LOCAL;
                }
                break;
            case 'H':
                setenv( SUT_HOST, optarg, 1 );
                cfg.hostname = optarg;
                break;
            case 'P':
                setenv( SUT_PORT, optarg, 1 );
                cfg.rawport = atoi( optarg );
                break;
            case 'c':
                cmd = optarg;
                break;
            case 'h':
                UNDBG;
                usage( EXIT_SUCCESS );
                break;
            case 'v':
                cfg.verbose = true;
                break;
            case '?':
                if (isprint (optopt))
                    {dbg_error("Unknown option `-%c'.\n", optopt);}
                else
                    dbg_error("Unknown option character `\\x%x'.\n", optopt);
                usage( EXIT_FAILURE);
                break;
           default:
                usage( EXIT_FAILURE);
                break;
        }
    }
    return true;
}

