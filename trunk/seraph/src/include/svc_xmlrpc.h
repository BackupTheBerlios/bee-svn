#ifndef _SVC_XMLRPC_H
#define _SVC_XMLRPC_H

#include "xmlrpc.h"
XMLRPC_VALUE x_startCallback    (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_startManagerCallback    (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_stopCallback     (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_refreshCallback  (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_copyCallback     (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_rmCallback       (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_mkdirCallback    (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_executeCallback  (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_checkCoreCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_runTestsCallback (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_listTestsCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_listUserTestsCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_listOSesCallback (XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_listMachinesCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_addMachineCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_addUserTest(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_getConfigCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_setConfigCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_checkSessionCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_setSessionCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_setSessionCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_checkLoginCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_checkRememberedCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_listJobsCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_getErrorLogCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_registerUserCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);
XMLRPC_VALUE x_clearRunnedJobsCallback(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);

#else
        #warning "*** Header allready included ***"
#endif
