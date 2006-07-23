#include "Report.h"
#include <cstdlib>
#include <pthread.h>
#include "structs.h"

Report::Smtp::Smtp()
{
    f = fopen("timesmtp.csv", "a");
    if(!f) { fprintf(stderr, "cannot open timesmtp.csv. Exit\n");exit(2); }
    /* On 4.2BSD and 4.3BSD systems, setbuf() always uses a
       suboptimal buffer size and should be avoided.
    */
    setlinebuf(f);
}

Report::Smtp::~Smtp()
{
    fclose(f);
}

Report::Smtp::Smtp(const char* host)
{
    // still to be implemented
    f = fopen("timesmtp.csv", "a");
    if(!f) { fprintf(stderr, "cannot open timesmtp.csv. Exit\n");exit(2); }
    setlinebuf(f);
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


// Err reporting function

void Report::Smtp::openErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "open", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::greetErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "greet", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::mailFromErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "mailFrom", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::rcptToErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "rcptTo", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::beginDataErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "beginData", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::endDataErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "endData", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::quitErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "quit", timer_->elapsed(), pthread_self() ) ;
}


void Report::Smtp::rsetErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "rset", timer_->elapsed(), pthread_self() ) ;
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



// Error reporting for pop3

void Report::Pop3::openErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "popen", timer_->elapsed(), pthread_self() ) ;
    throw ReportEx("Pop::open");
}
void
Report::Pop3::userErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "puser", timer_->elapsed(), pthread_self() ) ;
    throw ReportEx("Pop::user");
}
void
Report::Pop3::passErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "ppass", timer_->elapsed(), pthread_self() ) ;
    throw ReportEx("Pop::pass");
}
void
Report::Pop3::statErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "pstat", timer_->elapsed(), pthread_self() ) ;
    throw ReportEx("Pop::stat");
}
void
Report::Pop3::retrErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "pretr", timer_->elapsed(), pthread_self() ) ;
    throw ReportEx("Pop::retr");
}
void
Report::Pop3::deleErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "pdele", timer_->elapsed(), pthread_self() ) ;
    throw ReportEx("Pop::dele");
}
void
Report::Pop3::quitErr()
{
    fprintf(f, "ERR%s,%f,%u\n", "pquit", timer_->elapsed(), pthread_self() ) ;
    throw ReportEx("Pop::quit");
}
