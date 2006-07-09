#include "Report.h"
#include <cstdlib>

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
    fprintf(f, "%s,%f\n", "open", timer_->elapsed() ) ;
}


void Report::Smtp::greet() 
{
    fprintf(f, "%s,%f\n", "greet", timer_->elapsed() ) ;
}


void Report::Smtp::mailFrom() 
{
    fprintf(f, "%s,%f\n", "mailFrom", timer_->elapsed() ) ;
}


void Report::Smtp::rcptTo() 
{
    fprintf(f, "%s,%f\n", "rcptTo", timer_->elapsed() ) ;
}


void Report::Smtp::beginData() 
{
    fprintf(f, "%s,%f\n", "beginData", timer_->elapsed() ) ;
}


void Report::Smtp::endData() 
{
    fprintf(f, "%s,%f\n", "endData", timer_->elapsed() ) ;
}


void Report::Smtp::quit() 
{
    fprintf(f, "%s,%f\n", "quit", timer_->elapsed() ) ;
}


void Report::Smtp::rset() 
{
    fprintf(f, "%s,%f\n", "rset", timer_->elapsed() ) ;
}
