struct Class {
    size_t size;
    void * (* ctor) (void * this, va_list * app);
    void * (* dtor) (void * this);
    void * (* clone) (const void * this);
    int (* differ) (const void * this, const void * b);
};
