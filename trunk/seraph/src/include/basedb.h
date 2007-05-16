
/*BaseDB class implementation*/
extern const void * BaseDB;
struct BaseDB {
    const void *class;
    char *dbName;
} ;

#define DB_CREATE 1
#define DB_OPEN 2
