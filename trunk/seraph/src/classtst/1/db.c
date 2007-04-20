#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include "baseclass.h"

/*String class implementation*/
extern const void * String;
struct String {
    const void *class;
    char *text;
} ;

static void * String_ctor (void * _this, va_list * app)
{   struct String * this = _this;
    const char * text = va_arg(* app, const char *);
    this -> text = malloc(strlen(text) + 1);
    assert(this -> text);
    strcpy(this -> text, text);
    return this;
}

static void * String_dtor (void * _this)
{   struct String * this = _this;
    free(this -> text), this -> text = 0;
    return this;
}
static void * String_clone (const void * _this)
{   const struct String * this = _this;
    return new(String, this -> text);
}
static int String_differ (const void * _this, const void * _b)
{   const struct String * this = _this;
    const struct String * b = _b;
    if (this == b)
        return 0;
    if (! b || b -> class != String)
        return 1;
    return strcmp(this -> text, b -> text);
}
static const struct Class _String = {
    sizeof(struct String),
    String_ctor, String_dtor,
    String_clone, String_differ
};
const void * String = & _String;


int main()
{
    struct String* str = new(String, "text1");
    printf("%s\n", str->text);

}

/*db_create
  db_open
  db_put
  db_get
  db_close
  */
