#ifndef _BASECLASS_H_
#define _BASECLASS_H_
struct Class {
    size_t size;
    void * (* ctor) (void * this, va_list * app);
    void * (* dtor) (void * this);
    void * (* clone) (const void * this);
    int (* differ) (const void * this, const void * b);
    int (*db_open)(const void* this, const char* const dbName, ...);
    int (*db_put)(const void* this, const char* key, const char* data);
    int (*db_get)(const void* this, const char* key, char* *data);
    int (*db_close)(const void* this);
};
int differ (const void * this, const void * b);
size_t sizeOf (const void * this);
int db_open(const void* this, const char* const dbName, ...);
int db_put(const void* this, const char* key, const char* data);
int db_get(const void* this, const char* key, char* *data);
int db_close(const void* this);
void * new (const void * _class, ...);
void delete (void * this);
#endif
