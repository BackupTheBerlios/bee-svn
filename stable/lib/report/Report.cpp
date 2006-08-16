#include "Report.h"
#include <cstdlib>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "structs.h"

Report::Smtp::Smtp()
{
    out = ::open("timesmtp.csv", O_APPEND );
    if( -1 == out ){
        fprintf(stderr, "cannot open timesmtp.csv\n");
        exit(2);
    }
}

Report::Smtp::~Smtp()
{
    ::close(out);
}

Report::Smtp::Smtp(const char* host)
{
    // still to be implemented
    out = ::open("timesmtp.csv", O_APPEND);
    if(-1 == out) { fprintf(stderr, "cannot open timesmtp.csv. Exit\n");exit(2); }
}


void Report::Smtp::open()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "open", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::greet()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "greet", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::mailFrom()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "mailFrom", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::rcptTo()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "rcptTo", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::beginData()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "beginData", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::endData()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "endData", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::quit()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "quit", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::rset()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "rset", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


// Err reporting function

void Report::Smtp::openErr()
{
    char buf[128] ={0};
    timer_->stop() ;
    sprintf(buf, "ERR%s,%buf,%u\n", "open", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::greetErr()
{
    char buf[128] ={0};
    timer_->stop() ;
    sprintf(buf, "ERR%s,%buf,%u\n", "greet", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::mailFromErr()
{
    char buf[128] ={0};
    timer_->stop() ;
    sprintf(buf, "ERR%s,%buf,%u\n", "mailFrom", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::rcptToErr()
{
    char buf[128] ={0};
    timer_->stop() ;
    sprintf(buf, "ERR%s,%buf,%u\n", "rcptTo", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::beginDataErr()
{
    char buf[128] ={0};
    timer_->stop() ;
    sprintf(buf, "ERR%s,%buf,%u\n", "beginData", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::endDataErr()
{
    char buf[128] ={0};
    timer_->stop() ;
    sprintf(buf, "ERR%s,%buf,%u\n", "endData", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::quitErr()
{
    char buf[128] ={0};
    timer_->stop() ;
    sprintf(buf, "ERR%s,%buf,%u\n", "quit", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}


void Report::Smtp::rsetErr()
{
    char buf[128] ={0};
    timer_->stop() ;
    sprintf(buf, "ERR%s,%buf,%u\n", "rset", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}





//----------------POP3 section
Report::Pop3::Pop3()
{
    out = ::open("timepop3.csv", O_APPEND);
    if(-1 == out ) { fprintf(stderr, "cannot open timepop3.csv. Exit\n");exit(2); }
}

Report::Pop3::~Pop3()
{
    ::close(out);
}
/*
Report::Pop3::Pop3(const char* host)
{
    // still to be implemented
    printf("host=%s\n", host ) ;
    buf = fopen("timepop3.csv", "a");
    if(!buf) { sprintf(stderr, "cannot open timepop3.csv. Exit\n");exit(2); }
}
*/
void Report::Pop3::open()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "popen", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::user()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "puser", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::pass()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "ppass", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::stat()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "pstat", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::retr()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "pretr", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::dele()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "pdele", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::quit()
{
    char buf[128] ={0};
    sprintf(buf, "%s,%buf,%u\n", "pquit", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}



// Error reporting for pop3

void Report::Pop3::openErr()
{
    timer_->stop() ;
    char buf[128] ={0};
    sprintf(buf, "ERR%s,%buf,%u\n", "popen", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::userErr()
{
    timer_->stop() ;
    char buf[128] ={0};
    sprintf(buf, "ERR%s,%buf,%u\n", "puser", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::passErr()
{
    timer_->stop() ;
    char buf[128] ={0};
    sprintf(buf, "ERR%s,%buf,%u\n", "ppass", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::statErr()
{
    timer_->stop() ;
    char buf[128] ={0};
    sprintf(buf, "ERR%s,%buf,%u\n", "pstat", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::retrErr()
{
    timer_->stop() ;
    char buf[128] ={0};
    sprintf(buf, "ERR%s,%buf,%u\n", "pretr", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::deleErr()
{
    timer_->stop() ;
    char buf[128] ={0};
    sprintf(buf, "ERR%s,%buf,%u\n", "pdele", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
void
Report::Pop3::quitErr()
{
    timer_->stop() ;
    char buf[128] ={0};
    sprintf(buf, "ERR%s,%buf,%u\n", "pquit", timer_->elapsed(), pthread_self() ) ;
    write( out, buf, strlen(buf) );
}
