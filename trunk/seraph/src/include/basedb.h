#ifndef _BASEDB_H_
#define _BASEDB_H_

/*BaseDB class implementation*/
extern const void * BaseDB;
struct BaseDB {
    const void *class;
    char *dbName;
} ;

#define DB_CREATE 1
#define DB_OPEN 2

#define KEY_LEN 40
#endif
