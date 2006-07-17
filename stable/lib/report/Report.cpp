#include "Report.h"
#include <cstdlib>
#include <pthread.h>

Report::Smtp::Smtp()
{
    f = fopen("statistics.csv", "a");
    if(!f) { fprintf(stderr, "cannot open statistics.csv. Exit\n");exit(2); }
} 

Report::Smtp::~Smtp()
{
    fclose(f);
} 

Report::Smtp::Smtp(const char* host)
{
    // still to be implemented
    printf("host=%s\n", host ) ;
    f = fopen("statistics.csv", "a");
    if(!f) { fprintf(stderr, "cannot open statistics.csv. Exit\n");exit(2); }
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
