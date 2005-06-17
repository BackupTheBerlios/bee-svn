#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <confuse.h>

using namespace std;

typedef struct Stored_t
{
      string date_s;
      unsigned int mails_ui;
};

typedef struct Connection_t
{
      string user_s;
      string pass_s;
      string host_s;
      string domain_s;
      unsigned int port_ui;
      vector<Stored_t>     storedVector;
};

class Config
{
      public:
            Config( int connection){};
            //cfg_t* parseConf( string filename);
            string host( int connection);
            string user( int connection);
            string pass( int connection);
            int    port( int connection);
            string date( int connection, int stored);
            int    mails( int connection, int stored);
            string domain( int connection);
};
#endif
