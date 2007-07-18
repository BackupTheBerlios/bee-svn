#ifndef _BASEDB_H_
#define _BASEDB_H_

extern const void * BaseDB;

struct BaseDB {
    const void *class;
    char *dbName;
    int (*db_open) (const void *_this, const char* const fmt, va_list*app);
    int (*db_put)  (const void *_this, const char* const key, const char* data);
    int (*db_get)  (const void *_this, const char* key, char *data);
    int (*db_close)(const void *_this);

} ;
int db_open(const void* this, const char* const fmt, ...);
int db_put(const void* this, const char* key, const char* data);
int db_get(const void* this, const char* key, char* *data);
int db_close(const void* this);

#define DB_CREATE 1
#define DB_OPEN 2

#define KEY_LEN 40

#else
    #warn "******** Header included twice ******"
#endif
