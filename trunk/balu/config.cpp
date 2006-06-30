#include <iostream>
#include "config.h"
using namespace std;
extern vector<Connection_t> conVector;


/*   Implementata functia mails( numberOfMails, mail_Iternaldate);*/

string
Config::host( int connection)
{
      if( conVector.size() < connection+1)
      {
            cerr <<
                  "ERR>> config(out of range)"
                  <<endl;
            exit(1);
      }
      return conVector[ connection].host_s;
}



string
Config::domain( int connection)
{
      if( conVector.size() < connection+1)
      {
            cerr <<
                  "ERR>> config(out of range)"
                  <<endl;
            return "";
      }
      return conVector[ connection].domain_s;
}




int
Config::port( int connection)
{
      if( conVector.size() < connection+1)
      {
            cerr <<
                  "ERR>> config(out of range)"
                  <<endl;
            return -1;
      }
      return conVector[ connection].port_ui;
}




string
Config::user( int connection)
{
      if( conVector.size() < connection+1)
      {
            cerr <<
                  "ERR>> config(out of range)"
                  <<endl;
            return "";
      }
      return conVector[ connection].user_s;
}




string
Config::pass( int connection)
{
      if( conVector.size() < connection+1)
      {
            cerr <<
                  "ERR>> config(out of range)"
                  <<endl;
            return "";
      }
      return conVector[ connection].pass_s;
}




string
Config::date( int connection, int stored)
{
      if( conVector.size() < connection+1)
      {
            cerr <<
                  "ERR>> config(out of range)"
                  <<endl;
            return "";
      }
      if ( conVector[ connection].storedVector.size() < stored)
      {
            cerr <<
                  "ERR>> config(out of range)"
                  <<endl;
            return "";
      }
      return conVector[ connection].storedVector[stored].date_s;
}




int
Config::mails( int connection, int stored)
{
      if( conVector.size() < connection+1)
      {
            cerr <<
                  "ERR>> config(out of range)"
                  <<endl;
            return -1;
      }
      if ( conVector[ connection].storedVector.size() < stored)
      {
            cerr <<
                  "ERR>> config(out of range)"
                  <<endl;
            return -1;
      }
      return conVector[ connection].storedVector[ stored].mails_ui;
}
