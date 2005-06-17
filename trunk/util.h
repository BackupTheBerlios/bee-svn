#ifndef UTIL_H
#define UTIL_H

extern "C"
{
#include <lauxlib.h>
}
#include <iostream>
#include "luaConfig.h"

#include "luaWrapper.h"
#include "util.h"
#define method(class, name) {#name, class::name}

using namespace std;

typedef struct ThreadInfo_t {
      unsigned int numThreads;
      string script;

};

cfg_t* parseConf( string);
void  readConfig( void);
void* luaThread( void*);
void doThreads();
#endif
