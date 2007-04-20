struct Class {
    size_t size;
    void * (* ctor) (void * this, va_list * app);
    void * (* dtor) (void * this);
    void * (* clone) (const void * this);
    int (* differ) (const void * this, const void * b);
    int (*db_open)(const void* this, const char* const dbName, va_list* app );
    int (*db_put)(const void* this, const char* key, const char* data);
    int (*db_get)(const void* this, const char* key, char* *data);
    int (*db_close)(const void* this);
};
