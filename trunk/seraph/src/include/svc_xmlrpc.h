#ifndef _SVC_XMLRPC_H
#define _SVC_XMLRPC_H

#include "xmlrpc.h"
XMLRPC_VALUE x_startCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_stopCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_refreshCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_copyCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_rmCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_mkdirCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_executeCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_checkCoreCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_listTestsCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_listMachinesCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_getConfigCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_runTestsCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_listOSes(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);

#else
        #warning "*** Header allready included ***"
#endif
