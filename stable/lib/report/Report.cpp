#include "Report.h"
#include <cstdlib>
#include <pthread.h>

Report::Smtp::Smtp()
{
    f = fopen("timesmtp.csv", "a");
    if(!f) { fprintf(stderr, "cannot open timesmtp.csv. Exit\n");exit(2); }
}

Report::Smtp::~Smtp()
{
    fclose(f);
}

Report::Smtp::Smtp(const char* host)
{
    // still to be implemented
    printf("host=%s\n", host ) ;
    f = fopen("timesmtp.csv", "a");
    if(!f) { fprintf(stderr, "cannot open timesmtp.csv. Exit\n");exit(2); }
}


void Report::Smtp::open() 
{
    fprintf(f, "%s,%f,%u\n", "open", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::greet() 
{
    fprintf(f, "%s,%f,%u\n", "greet", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::mailFrom() 
{
    fprintf(f, "%s,%f,%u\n", "mailFrom", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::rcptTo() 
{
    fprintf(f, "%s,%f,%u\n", "rcptTo", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::beginData() 
{
    fprintf(f, "%s,%f,%u\n", "beginData", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::endData() 
{
    fprintf(f, "%s,%f,%u\n", "endData", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::quit() 
{
    fprintf(f, "%s,%f,%u\n", "quit", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::rset() 
{
    fprintf(f, "%s,%f,%u\n", "rset", timer_->elapsed(), pthread_self() ) ;
}


//----------------POP3 section
Report::Pop3::Pop3()
{
    f = fopen("timepop3.csv", "a");
    if(!f) { fprintf(stderr, "cannot open timepop3.csv. Exit\n");exit(2); }
}

Report::Pop3::~Pop3()
{
    fclose(f);
}
/*
Report::Pop3::Pop3(const char* host)
{
    // still to be implemented
    printf("host=%s\n", host ) ;
    f = fopen("timepop3.csv", "a");
    if(!f) { fprintf(stderr, "cannot open timepop3.csv. Exit\n");exit(2); }
}
*/
void Report::Pop3::open()
{
    fprintf(f, "%s,%f,%u\n", "popen", timer_->elapsed(), pthread_self() ) ;
}
void
Report::Pop3::user()
{
    fprintf(f, "%s,%f,%u\n", "puser", timer_->elapsed(), pthread_self() ) ;
}
void
Report::Pop3::pass()
{
    fprintf(f, "%s,%f,%u\n", "ppass", timer_->elapsed(), pthread_self() ) ;
}
void
Report::Pop3::stat()
{
    fprintf(f, "%s,%f,%u\n", "pstat", timer_->elapsed(), pthread_self() ) ;
}
void
Report::Pop3::retr()
{
    fprintf(f, "%s,%f,%u\n", "pretr", timer_->elapsed(), pthread_self() ) ;
}
void
Report::Pop3::dele()
{
    fprintf(f, "%s,%f,%u\n", "pdele", timer_->elapsed(), pthread_self() ) ;
}
void
Report::Pop3::quit()
{
    fprintf(f, "%s,%f,%u\n", "pquit", timer_->elapsed(), pthread_self() ) ;
}
