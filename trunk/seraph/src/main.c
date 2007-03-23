#include "debug.h"
#include "seraph.h"
#include "rshd.h"
#include "strop.h"

struct config_s cfg;

int main( int argc, char *argv[] )
{


        DBG;
        if( argc == 1 )
                srph_usage( EXIT_FAILURE );

        /*1. fill in the config with default values */
        srph_initCfg( &cfg, argc, argv );

        /*2. export axi_ttype axi_host axi_port specified as -t, -H, -P */
        srph_parseArgs( &cfg, argc, argv );
        switch ( cfg.behaviour ) {
        case TB_BE_DAEMON:
                if( cfg.start_rawrpc ) {
                        if( cfg.port == 0 ) {
                                fprintf( stderr,
                                         "! seraph: You have to specify -P option\n" );
                                exit( -2 );
                        } else {
                                start_rawrpc( cfg.port );
                        }

                }               /*if( cfg.start_jabber)
                                   start_jabber(); */
                if( cfg.start_xmlrpc ) {
                        /*3. export any variable from config file */
                        srph_parseCfg( cfg.config_file );

                        /*4. check if the needed variables are exported */
                        srph_initEnv( &cfg );
                        /*5. check if the proper tools are installed */
                        srph_checkTools( getenv( SUT_TOOL ) );
                        start_xmlrpc( cfg.port );
                }
                return 0;
        case TB_BE_SETUP:
                //setup(cfg.machine);
                return 0;
        }


        /*3. export any variable from config file */
        srph_parseCfg( cfg.config_file );

        /*4. check if the needed variables are exported */
        srph_initEnv( &cfg );

        /*5. check if the proper tools are installed */
        srph_checkTools( getenv( SUT_TOOL ) );

        if( !cfg.test_dir ) {
                printf( "! seraph: Provide the test directory.\n" );
                srph_usage( EXIT_FAILURE );
        }

        if( cfg.test_type == TEST_LOCAL && cfg.verbose == TRUE ) {
                debug( "* seraph: Tests will be done LOCALLY.\n" );
        }
        if( cfg.test_type == TEST_REMOTE ) {
                str_isEnv( cfg.verbose, SUT_HOST );
                str_isEnv( cfg.verbose, SUT_PORT );
                if( cfg.verbose == TRUE )
                        debug( "* seraph: Tests will be done REMOTE.\n" );
        }
        srph_runTests( cfg.test_dir );
        srph_free( &cfg );
        UNDBG;
        return 0;
}
