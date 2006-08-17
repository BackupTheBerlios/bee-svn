#include "pop3/Protocol.h"
#include <string>
#include <iostream>
#include <stdlib.h> // itoa
//Pop3::Protocol
using namespace std;

string itoa( int a )
{
    char str[33] ;
    sprintf( str, "%i", a ) ;
    return string(str) ;
}

/** Default constructor. ___*/
Pop3::Protocol::Protocol()
{
    report_ = new Report::Pop3() ;
    report_->timer( &timer_ ) ;
}




/** Default destructor. ___*/
Pop3::Protocol::~Protocol()
{
    delete report_ ;
}



/* Overriden functions to be POP3 aware */
    void
Pop3::Protocol::read()
{
#define BUF_LEN 2048
    char buffer[BUF_LEN] ;

    for( ;;)
    {
        printf("Read\n");
        Socket::read( buffer, BUF_LEN ) ;
        //if(!resp_.empty() ) cout <<resp_ <<endl ; //debug

        if(resp_.find( "+OK ",0) != string::npos)
        {
            printf("+OK\n" ) ; // debug
            return ;
        }
        if(resp_.find("-ERR ",0) != string::npos)
        {   fprintf( stderr, "Pop3 ERROR\n") ;
            throw Socket::Exception("Pop3 Error") ;
        }
    }
}



    void
Pop3::Protocol::open( const char* host, const int& port)
{
    timer_.start() ;
    try {
        Socket::open ( Socket::Family::Inet, Socket::Type::Stream, 0);
        connect( host, port);
        read() ;
    }catch(Socket::Exception&ex)
    {
        report_->openErr();
        throw ex ;
    }
    timer_.stop() ;
    report_->open() ;
}

    void
Pop3::Protocol::open(sockaddr_in* dest ) 
{
    timer_.start() ;
    try{
        Socket::open ( Socket::Family::Inet, Socket::Type::Stream, 0);
        connect(dest) ;
        read() ;
    }catch(Socket::Exception&ex)
    {
        report_->openErr() ;
        throw ex ;
    }
    timer_.stop() ;
    report_->open() ;
}



    void
Pop3::Protocol::write( const std::string& message)
{
    //cout <<message <<endl;//debug
    Socket::write( message ) ;
}



/* POP3 Protocol specific functions */
    void
Pop3::Protocol::user( const std::string& userName )
{
    timer_.start() ;
    try{
        write( "USER "+userName+"\r\n" ) ;
        read();
    }catch(Socket::Exception&ex)
    {
        report_->userErr() ;
        throw ex;
    }
    timer_.stop() ;
    report_->user() ;
}


/*
 * LIMIT: command line length is 512 chars */
    void
Pop3::Protocol::user( const char* prefix, const int idx )
{
    char buf[512] ={0};
    sprintf( buf, "USER %s%i\r\n", prefix, idx );
    timer_.start() ;
    try{
        write( buf ) ;
        read();
    }catch(Socket::Exception&ex)
    {
        report_->userErr() ;
        throw ex;
    }
    timer_.stop() ;
    report_->user() ;
}




/*
 * LIMIT: command line length is 512 chars */
    void
Pop3::Protocol::pass( const char* prefix, const int idx )
{
    char buf[512] ={0};
    sprintf( buf, "PASS %s%i\r\n", prefix, idx );
    timer_.start() ;
    try {
        write( buf ) ;
        read();
    }catch(Socket::Exception&ex)
    {
        report_->passErr() ;
        throw ex ;
    }
    timer_.stop() ;
    report_->pass() ;
}

    void
Pop3::Protocol::pass( const std::string& passwd )
{
    timer_.start() ;
    try {
        write( "PASS "+passwd+"\r\n" ) ;
        read();
    }catch(Socket::Exception&ex)
    {
        report_->passErr() ;
        throw ex ;
    }
    timer_.stop() ;
    report_->pass() ;
}

/**/
    void
Pop3::Protocol::apop( void )
{
    timer_.start() ;
    try {
        write( "APOP \r\n" ) ;
        read() ;
    }catch(Socket::Exception&ex)
    {
        //report_->apopErr() ;
        throw ex ;
    }
    timer_.stop() ;
    //report_->apop() ; // TODO
}

    void
Pop3::Protocol::stat( int* mails, int* size )
{
    timer_.start() ;
    try {
        write( "STAT\r\n" ) ;
        read() ;
    }catch(Socket::Exception&ex)
    {
        report_->statErr();
        throw ex ;
    }
    timer_.stop() ;
    report_->stat() ;
    sscanf( resp_.c_str(), "+OK %u %u", mails, size ) ;
}


    int
Pop3::Protocol::list( long int msgNo )
{
    try {
        // LIST all messages
        if( msgNo < 0 ) 
        {
            timer_.start() ;
            write( "LIST\r\n" ) ;
            read( ) ;           // receive a multi line message ( Ends with . )
            timer_.stop() ;
            report_->list() ;
        }else
        {
            int msg_sz = 0, msg_no=0 ;
            timer_.start() ;
            write( "LIST "+itoa(msgNo)+"\r\n" ) ;
            read( ) ;           // receive one line only
            timer_.stop() ;
            report_->list() ;
            sscanf( resp_.c_str(), "+OK %u %u", &msg_no, &msg_sz ) ;
            return msg_sz ;
        }
    }catch(Socket::Exception&ex)
    {
        report_->listErr() ;
        throw ex ;
    }
}

    void
Pop3::Protocol::noop( void )
{
    timer_.start() ;
    write( "NOOP\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    //report_->noop() ;//TODO
}


    void
Pop3::Protocol::quit( void )
{
    timer_.start() ;
    try{
        write( "QUIT\r\n" ) ;
        read();
        close() ;
    }catch(Socket::Exception&ex)
    {
        report_->quitErr() ;
        throw ex;
    }
    timer_.stop() ;
    report_->quit() ;
}

    void
Pop3::Protocol::retr( long int msgNo )
{
    if(  !msgNo ) return ;
    char buf[512];
    sprintf( buf, "RETR %lu\r\n", msgNo ) ;
    timer_.start() ;
    try {
        write( buf ) ;
        read();
    }catch(Socket::Exception&ex)
    {
        report_->retrErr() ; // multiline response
        throw ex ;
    }
    timer_.stop() ;
    report_->retr() ;
}

    void
Pop3::Protocol::dele( unsigned long int msgNo )
{
    if(  !msgNo ) return ;
    char buf[512] ;
    sprintf( buf, "DELE %lu\r\n", msgNo ) ;
    timer_.start() ;
    try{
        write( buf ) ;
        read( ) ;
    }catch(Socket::Exception&ex)
    {
        report_->deleErr() ;
        throw ex ; // Why does it crash here ?
    }
    timer_.stop() ;
    report_->dele() ;
}

    void
Pop3::Protocol::rset( void )
{
    timer_.start() ;
    write( "RSET\r\n" ) ;
    read( ) ;
    timer_.stop() ;
    //report_->rset() ;//TODO
}

    void
Pop3::Protocol::top( long int msgNo, long int n )
{
    timer_.start() ;
    write( "TOP "+itoa(msgNo)+" "+itoa(n)+"\r\n" ) ;
    //if(!read()) report_->topErr() ;// if no err is returned, expect a multinile response
    timer_.stop() ;
    //report_->top() ;//TODO
}

    void
Pop3::Protocol::uidl( long int msgNo )
{
    if( msgNo <0 )
    {
        timer_.start() ;
        write( "UIDL\r\n" ) ;
        read() ; //multiline
        timer_.stop() ;
        //report_->uidl() ;//TODO
    }else // it can be = 0 ??
    {
        timer_.start() ;
        write( "UIDL "+itoa(msgNo)+"\r\n" ) ;
        read( ) ; //single line. extract the UniqueID
        timer_.stop() ;
        //report_->uidl() ;//TODO
    }
}
