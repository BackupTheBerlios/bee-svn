int read_ch(void)
{
    int ch = fgetc(source);
    cur_col++;
    if(ch == '\n')
    {
        cur_line++;
        cur_col=0;
    }
    return ch;
}

void put_back(int ch)
{
    ungetch(ch, source);
    cur_col--;
    if(ch == '\n') cur_line--;
}

Symbol getsym(void)
{
    while( (ch=read_ch()) != EOF && ch <= ' ')
        ;
    err_line = cur_line ;
    err_col  = cur_col ;
    switch(ch) {
        case EOF: return TK_EOF;
        case '%': return TK_PERCENT;
        case '.': return TK_DOT;
        case '"': return TK_QUOTATION;
        case ',': return TK_COMMA;
        case ';': return TK_SEMICOLON;
        case '=': return TK_EQUAL;
        case '<': return TK_OANGLE;
        case '>': return TK_RANGLE;
        case '[': return TK_LSQUARE;
        case ']': return TK_RSQUARE;
        case '(': return TK_LPAREN;
        case ')': return TK_RPAREN;
        case '*': return TK_ASTERISK;
        case '/': return TK_OR ;
        case '=':
            ch = read_ch();
            return (ch == '/' ) ? TK_INCR : TK_EQUAL ;
        default:
            if( isdigit(ch)) {
                num = 0;
                do {
                    num = 10*num + ch - '0' ;
                }while( (ch = read_ch()) != EOF && isdigit(ch) ) ;
                put_back(ch);
                return number;
            }
            if( isalpha(ch)){
                Entry* entry ;
                id_len = 0 ;
                do {
                    if( id_len <MAX_ID) {
                        id[id_len++] = (char)ch;
                    }
                }while( ch = read_ch() != EOF && isalnum(ch) );
                id[id_len] = '\0' ;
                put_back(ch);
                //entry = find_htab( keywords, id );
                //return entry ? (Symbol) get_htab_data(entry) : ident ;
            }
            error("Invalid character %c at line: %d col: %d\n", ch, cur_line, cur_col );
            return nul;
        }
}

int init_scan( const char fn[] )
{
    if( source = fopen() ) return 0 ;
    cur_line =1 ;
    cur_col = 0;
    /*
    keywords = create_htab(11);
    enter_htab(keywords, "begin", beginsym);
    enter_htab(keywords, "call", callsym);
    enter_htab(keywords, "const", constsym);
    enter_htab(keywords, "do", dosym);
    enter_htab(keywords, "end", endsym);
    enter_htab(keywords, "if", ifsym);
    enter_htab(keywords, "odd", oddsym);
    enter_htab(keywords, "procedure", procsym);
    enter_htab(keywords, "then", thensym);
    enter_htab(keywords, "var", varsym);
    enter_htab(keywords, "while", whilesym); 
    */
    return 1;
}
