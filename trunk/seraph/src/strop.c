#include <strings.h>
#include "debug.h"
#include "config.h"
#include "strop.h"

extern struct config_s cfg;

int
str_search( const char *hay_stack, int hay_size, const char *needle,
            int needle_size )
{
        int i, j;

        j = needle_size - 1;
        for( i = hay_size - 1; i >= 0; --i ) {
                if( hay_stack[i] == needle[j] ) {
                        if( --j == -1 )
                                return TRUE;
                        continue;
                } else {
                        j = needle_size - 1;
                }
        }
        return FALSE;
}



int str_isEnv( int verbose, char *var_name )
{
        if( !var_name )
                return FALSE;
        if( !getenv( var_name ) ) {
                debug( "$%s not exported.\n", var_name );
                exit( -1 );
        }
        if( verbose == TRUE )
                printf( "$%s=%s\n", var_name, getenv( var_name ) );
        return 0;
}



int str_startsWith( char *str, char *exp )
{
        int i;

        for( i = 0; i < strlen( exp ); ++i ) {
                if( str[i] != exp[i] )
                        return 0;
        }
        return 1;
}




int str_matches( char *buffer, char *string )
{
        if( !strncasecmp( buffer, string, strlen( string ) ) )
                return 1;
        return 0;
}

int str_endsWith( const char *str, const char *model )
{
        int len_str = strlen( str );
        int len_m = strlen( model );
        int i;

        for( i = 1; i <= len_m; ++i ) {
                if( str[len_str - i] != model[len_m - i] )
                        return 0;
        }
        return 1;
}

int str_getCode( char *str )
{
        int i = 1;
        char *b = NULL;
        char cod[LINE_MAX];

        if( !str_startsWith( str, "OK:" )
            && !str_startsWith( str, "ERR:" ) ) {
                fprintf( stderr, "The string received from host is '%s'\n",
                         str );
                fprintf( stderr, "Expecting OK: or ERR:\n" );
                exit( 1 );
        }
        printf( "* Received : `%s`\n", str );
        b = strchr( str, ':' );

        while( isdigit( b[i] ) )
                ++i;
        if( i == 1 )
                return 0;

        memcpy( cod, b + 1, i - 1 );
        cod[i - 1] = '\0';
        return atoi( cod );
}



int str_isNumber( char *nr )
{
        int i;

        for( i = 0; i < strlen( nr ); ++i ) {
                if( !isdigit( nr[i] ) )
                        return 0;
        }
        return 1;
}
