/*
 * Copyright (C) 2006, 2007 Free Software Foundation, Inc.
 * Written by Negreanu Marius <groleo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING.  If not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include "dbg.h"
#include "new.h"
#include "class.h"
#include "basedb.h"
#include "plaindb.h"

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


/* database selectors */
int db_open(const void* this, const char* const fmt, ...)
{   const struct BaseDB * const * cp = this;
    va_list ap;
    int ret=0;

    assert(this && * cp && (* cp)->db_open);
    va_start(ap, fmt);
    ret = (* cp)->db_open(this, fmt, &ap);
    va_end(ap);
    return ret;
}
int db_put(const void* this, const char* key, const char* data)
{   const struct BaseDB * const * cp = this;
    assert(this && * cp && (* cp)->db_put);
    return (* cp)->db_put(this, key, data);
}
int db_get(const void* this, const char* key, char* *data)
{   const struct BaseDB * const * cp = this;
    assert(this && * cp && (* cp)->db_get);
    return (* cp)->db_get(this, key, data);
}
int db_close(const void* this)
{   const struct BaseDB * const * cp = this;
    assert(this && * cp && (* cp)->db_close);
    return (* cp)->db_close(this);
}



/* */
static const struct BaseDB _BaseDB = {
    sizeof(struct BaseDB), BaseDB_ctor,
    BaseDB_dtor, BaseDB_clone,
    BaseDB_differ,
    db_open, db_put,
    db_get, db_close
};

//const void * BaseDB = & _PlainDB ;
#if 0
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

#endif
