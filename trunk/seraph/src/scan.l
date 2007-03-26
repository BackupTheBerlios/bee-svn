/* scanner for _linux config file */

%{
/* need this for the call to atof() below */
#include <math.h>
#include <string.h>
#include "seraph.h"

extern struct config_s cfg;
int num_lines = 1, num_chars = 0;
char *p1=0, *p2=0;
int is_bat=0, is_cfg=0;
%}

DIGIT    [0-9]
ID       [a-zA-Z0-9_\$\{\}/\"-\.\; ]*
SPACE    [ \t\n]+
%option noyywrap

%%

#{SPACE}*axi_fi{SPACE}*={SPACE}[y|Y|n|N] {
    if(! is_bat ) return 0;
    
    ;
}

{ID}{SPACE}*"="{SPACE}*"'"{ID}"'" {
    if( ! is_cfg ) return 0;
    char* val=0;
 
    yytext[strlen(yytext)-1] = '\0';
    p1 = strchr( yytext, '=');
    *p1='\0';
    p1++;
    p2 = strchr( yytext, ' ');
    if(p2) *p2='\0';
    p1 = strchr(p1, '\'');
    *p1 = '\0';
    p1++;
    val = expand_vars(p1);
    if ( cfg.verbose )
        debug("export '%s'='%s'\n", yytext, val);
    setenv(yytext, val, 1 );
    free(val);
}


.+      fprintf( stderr, "Error on line %d\n", num_lines);

#.*\n   /* comment lines */

\n      ++num_lines;

%%

int
srph_parseCfg( char* config_file )
{
    is_cfg = 1;
    yyin = fopen( config_file, "r" );
    if(!yyin) { printf("Unable to open config file '%s'\n", config_file); exit(-1); }
    yylex();
    return 0;
}

int srph_parseBat( char* bat_file )
{
    is_bat = 1;
    yyin = fopen( bat_file, "r" );
    yylex();
    return 0;
}