
int
str_search( const char *hay_stack, int hay_size, const char *needle,
              int needle_size );
int
str_isEnv( int verbose, char *var_name );

int
str_startsWith( char *str, char *exp ) ;

int
str_matches( char *buffer, char *string ) ;

int
str_endsWith( const char *str, const char *model );

int
str_getCode( char *str );

int
str_isNumber( char *nr );

