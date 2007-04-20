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

/* generic selectors */
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



