#include "testbot.h"
#include "rshd.h"
#include "strop.h"

struct config_s cfg ;

int
main( int argc, char *argv[] )
{

        if( argc == 1 ) tb_usage( EXIT_FAILURE );
        tb_cfgInit( &cfg, argc, argv );
        tb_parseArgs( &cfg, argc, argv );

        if( cfg.act_as_daemon ) {
                if( cfg.port == 0 ) {
                        fprintf( stderr, "You have to specify -P option\n" );
                        exit( -2 );
                }
                rsh_main( cfg.port );
                return 0;
        }

        tb_cfgParse( cfg.config_file );
        tb_envInit( &cfg  );
        tb_checkTools( getenv( SUT_TOOL ) );

        if( !cfg.test_dir ) {
                printf( "! testbot: Provide the test directory.\n" );
                tb_usage( EXIT_FAILURE );
        }

        if( cfg.test_type == TEST_LOCAL && cfg.verbose == TRUE ){
                        printf( "* testbot: Tests will be done LOCALLY.\n" );
        }
        if( cfg.test_type == TEST_REMOTE ) {
                str_isEnv( cfg.verbose, SUT_HOST );
                str_isEnv( cfg.verbose, SUT_PORT );
                if( cfg.verbose == TRUE )
                        printf( "* testbot: Tests will be done REMOTE.\n" );
        }
        tb_runTests( cfg.test_dir );
        return 0;
}

