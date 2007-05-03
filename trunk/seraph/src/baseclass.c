#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include "baseclass.h"

void * new (const void * _class, ...)
{   const struct Class * class = _class;
    void * p = calloc(1, class->size);
    assert(p);
    * (const struct Class **) p = class;
    if (class->ctor)
    {   va_list ap;
        va_start(ap, _class);
        p = class->ctor(p, & ap);
        va_end(ap);
    }
    return p;
}

void delete (void * this)
{   const struct Class ** cp = this;
    if (this && * cp && (* cp)->dtor)
        this = (* cp)->dtor(this);
    free(this);
}

/* generconst char* const DbOpenMode*/
int differ (const void * this, const void * b)
{   const struct Class * const * cp = this;
    assert(this && * cp && (* cp)->differ);
    return (* cp)->differ(this, b);
}

size_t sizeOf (const void * this)
{   const struct Class * const * cp = this;
    assert(this && * cp);
    return (* cp)->size;
}

/* database selectors */
int db_open(const void* this, const char* const fmt, ...)
{   const struct Class * const * cp = this;
    va_list ap;
    int ret=0;

    assert(this && * cp && (* cp)->db_open);
    va_start(ap, fmt);
    ret = (* cp)->db_open(this, fmt, &ap);
    va_end(ap);
    return ret;
}
int db_put(const void* this, const char* key, const char* data)
{   const struct Class * const * cp = this;
    assert(this && * cp && (* cp)->db_put);
    return (* cp)->db_put(this, key, data);
}
int db_get(const void* this, const char* key, char* *data)
{   const struct Class * const * cp = this;
    assert(this && * cp && (* cp)->db_get);
    return (* cp)->db_get(this, key, data);
}
int db_close(const void* this)
{   const struct Class * const * cp = this;
    assert(this && * cp && (* cp)->db_close);
    return (* cp)->db_close(this);
}

