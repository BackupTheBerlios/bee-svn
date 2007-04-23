#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include "baseclass.h"
#include "debug.h"

/*BaseDB class implementation*/
extern const void * BaseDB;
struct BaseDB {
    const void *class;
    char *dbName;
} ;

/* */
static void * BaseDB_ctor (void * _this, va_list * app)
{   struct BaseDB * this = _this;
    //const char * text = va_arg(* app, const char *);
    this -> dbName = malloc(1025);
    /*assert(this -> text);
    strcpy(this -> text, text);*/
    return this;
}

static void * BaseDB_dtor (void * _this)
{   struct BaseDB * this = _this;
    free(this -> dbName), this->dbName = 0;
    return this;
}
static void * BaseDB_clone (const void * _this)
{   const struct BaseDB * this = _this;
    return new(BaseDB, "a"/*this -> text*/);
}

static int BaseDB_differ (const void * _this, const void * _b)
{   const struct BaseDB * this = _this;
    const struct BaseDB * b = _b;
    if (this == b)
        return 0;
    if (! b || b -> class != BaseDB)
        return 1;
    return 1;
}


static int BaseDB_open(const void *_this, const char* const fmt, va_list*app)
{   const struct BaseDB * this = _this;
    FILE* ret=NULL;
    char path[PATH_MAX]={0};

    vsprintf(path, fmt, *app);
    debug("fmt[%s] path[%s]\n", fmt, path);
    strcat(path,"/userdata");
    ret = fopen( path, "r+");
    if(!ret)
    {   debug("error opening database [%s]\n", path);
        return 1;
    }
    strcpy(this -> dbName, path);
    assert(this -> dbName);
    fclose(ret);
    return 0;
}
static int BaseDB_put(const void *_this, const char* const key, const char* data)
{   const struct BaseDB * this = _this;
    int     br=0 ;
    FILE*   fh;
    char    buf[1024]={0},*pc ;
    char    line[1024]={0};

    debug("fopen [%s]\n", this->dbName);
    fh = fopen( this->dbName ,"r+") ;
    if(!fh) {
        debug("error\n");
        return 0;
    }
    br = fread( (void*)buf, 1023, 1, fh) ;
    /*if(!br) {
        debug("error reading file[%s]\n", strerror(ferror(fh)) );
        return 0;
    }*/

    debug("br[%d] searching in buf[%s]\n", br, buf);
    snprintf(line, 34, "%32s:", key);
    pc = strstr( buf, line) ;
    if( !pc )
    {
        debug("Cant find key [%s]\n", line);
        fseek(fh, 0, SEEK_END);
        fprintf( fh, "%32s:%32s\n", key, data);
        fclose(fh);
        return 0 ;
    }
    debug("key[%s] found\n", key);
    pc = strchr(pc, ':')+1;
    snprintf( pc, 34, "%32s\n", data);
    fseek(fh, 0, SEEK_SET);
    fprintf( fh, "%s", buf);
    debug("write[%s]\n", buf);
    fclose(fh);
    return 0 ;
}
static int BaseDB_get(const void *_this, const char* key, char *data)
{   const struct BaseDB * this = _this;
    int     br=0 ;
    FILE*   fh=NULL;
    char    buf[1024]={0},*pc ;
    char    line[1024]={0};

    fh = fopen( this->dbName , "r") ;
    if(!fh) {
        printf("Unable to open database [%s], in get\n", this->dbName);
        return 0;
    }
    br = fread( buf, 1, 1023, fh) ;
    fclose( fh ) ;
    snprintf(line, 33, "%32s:", key);
    pc = strstr( buf, line) ;
    if( !pc )
    {
        return 0 ;
    }
    sscanf( strchr(pc,':')+1, "%s\n", data ) ;
    return 1 ;
}
static int BaseDB_close(const void *_this)
{   const struct BaseDB * this = _this;
}
/* */



/* */
static const struct Class _BaseDB = {
    sizeof(struct BaseDB),
    BaseDB_ctor, BaseDB_dtor,
    BaseDB_clone, BaseDB_differ, BaseDB_open, BaseDB_put, BaseDB_get, BaseDB_close
};
const void * BaseDB = & _BaseDB;

/*
int main()
{
    struct Class * db = new(BaseDB, "text1");
    char u[1024]={0};
    char p[1024]={0};

    db_open(db, "testdb", "r+");
    db_put(db, "user", "username1");
    db_put(db, "pass", "60b725f10c9c85c70d97880dfe8191b3");
    db_put(db, "user", "username3");
    db_put(db, "pass", "3b5d5c3712955042212316173ccf37be");
    db_get(db, "user", &u);
    db_get(db, "pass", &p);
    printf("User:%s\n", u);
    printf("Pass:%s\n", p);
    db_close(db);

}
*/
