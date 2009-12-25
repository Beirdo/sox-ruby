#include <ruby.h>
#include <sox.h>

static VALUE RubySox = Qnil;

void Init_sox(void);
sox_bool overwrite_callback(const char *filename);
VALUE wrap_new(VALUE class);
VALUE wrap_initialize(VALUE self);
static void destroy(void *p);
VALUE wrap_sox_format_init(VALUE self);
VALUE wrap_sox_format_quit(VALUE self);
VALUE wrap_sox_open_read(VALUE self, VALUE path, VALUE signal, VALUE encoding,
                         VALUE filetype);
VALUE wrap_sox_format_supports_encoding(VALUE self, VALUE path, VALUE filetype, 
                                        VALUE encoding);
VALUE wrap_sox_open_write(VALUE self, VALUE path, VALUE signal, VALUE encoding,
                          VALUE filetype, VALUE oob);
VALUE wrap_sox_read(VALUE self, VALUE ft, VALUE buf, VALUE len);
VALUE wrap_sox_write(VALUE self, VALUE ft, VALUE buf, VALUE len);
VALUE wrap_sox_close(VALUE self, VALUE ft);
VALUE wrap_sox_seek(VALUE self, VALUE ft, VALUE offset, VALUE whence);
VALUE wrap_sox_find_format(VALUE self, VALUE name, VALUE no_dev);
VALUE wrap_sox_find_effect(VALUE self, VALUE name);
VALUE wrap_sox_create_effect(VALUE self, VALUE eh);
VALUE wrap_sox_effect_options(VALUE self, VALUE effp, VALUE argc, VALUE argv);
VALUE wrap_sox_create_effects_chain(VALUE self, VALUE in_enc, VALUE out_enc);
VALUE wrap_sox_delete_effects_chain(VALUE self, VALUE ecp);
VALUE wrap_sox_add_effect(VALUE self, VALUE chain, VALUE effp, VALUE in, VALUE out);
VALUE wrap_count(VALUE self);

void Init_sox(void)
{
    RubySox = rb_define_class("RubySox", rb_cObject);
    rb_define_singleton_method( RubySox, "new", wrap_new, 0);
    rb_define_singleton_method( RubySox, "count", wrap_count, 0);
    rb_define_method( RubySox, "initialize", wrap_initialize, 0); 
    rb_define_method( RubySox, "sox_format_init", wrap_sox_format_init, 0); 
    rb_define_method( RubySox, "sox_format_quit", wrap_sox_format_quit, 0); 
    rb_define_method( RubySox, "sox_open_read", wrap_sox_open_read, 4);
    rb_define_method( RubySox, "sox_format_supports_encoding", wrap_sox_format_supports_encoding, 3);
    rb_define_method( RubySox, "sox_open_write", wrap_sox_open_write, 5); 
    rb_define_method( RubySox, "sox_read", wrap_sox_read, 3); 
    rb_define_method( RubySox, "sox_write", wrap_sox_write, 3); 
    rb_define_method( RubySox, "sox_close", wrap_sox_close, 1); 
    rb_define_method( RubySox, "sox_seek", wrap_sox_seek, 3); 
    rb_define_method( RubySox, "sox_find_format", wrap_sox_find_format, 2); 
    rb_define_method( RubySox, "sox_find_effect", wrap_sox_find_effect, 1); 
    rb_define_method( RubySox, "sox_create_effect", wrap_sox_create_effect, 1); 
    rb_define_method( RubySox, "sox_effect_options", wrap_sox_effect_options, 3); 
    rb_define_method( RubySox, "sox_create_effects_chain", wrap_sox_create_effects_chain, 2); 
    rb_define_method( RubySox, "sox_delete_effects_chain", wrap_sox_delete_effects_chain, 1); 
    rb_define_method( RubySox, "sox_add_effect", wrap_sox_add_effect, 4); 
}

VALUE wrap_new(VALUE class)
{
    static int count = 0;

    count++;
    VALUE unitnum = Data_Wrap_Struct(RubySox, 0, destroy, &count);

    rb_iv_set(class, "@count", INT2NUM(count));
    rb_obj_call_init(unitnum, 0, 0);
    return( unitnum );
}

VALUE wrap_initialize(VALUE self)
{
    int *count;

    Data_Get_Struct(self, int, count);
    if( *count == 1 ) {
        sox_format_init();
    }
    return( self );
}

VALUE wrap_count(VALUE class)
{
    return rb_iv_get(class, "@count");
}

static void destroy( void *p )
{
    int *count;

    count = (int *)p;
    *count--;
    if( *count == 0 ) {
        sox_format_quit();
    }
    rb_iv_set(RubySox, "@count", INT2NUM(*count));
}

VALUE wrap_sox_format_init(VALUE self)
{
    sox_format_init();
    return( Qnil );
}

VALUE wrap_sox_format_quit(VALUE self)
{
    sox_format_quit();
    return( Qnil );
}

VALUE wrap_sox_open_read(VALUE self, VALUE path, VALUE signal, VALUE encoding,
                         VALUE filetype)
{
    sox_format_t **c_fmt;
    sox_signalinfo_t *c_signal;
    sox_encodinginfo_t *c_encoding;
    VALUE fmt;

    Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
    Data_Get_Struct(encoding, sox_encodinginfo_t, c_encoding);
    fmt = Data_Make_Struct(RubySox, sox_format_t *, 0, free, c_fmt);
    *c_fmt = sox_open_read( STR2CSTR(path), c_signal, c_encoding, 
                            STR2CSTR(filetype) );
    return( fmt );
}

VALUE wrap_sox_format_supports_encoding(VALUE self, VALUE path, VALUE filetype, 
                                        VALUE encoding)
{
    sox_encodinginfo_t *c_encoding;
    sox_bool results;

    Data_Get_Struct(encoding, sox_encodinginfo_t, c_encoding);
    results = sox_format_supports_encoding( STR2CSTR(path), STR2CSTR(filetype),
                                            c_encoding );
    return( INT2NUM(results) );
}

sox_bool overwrite_callback(const char *filename)
{
    return sox_false;
}

VALUE wrap_sox_open_write(VALUE self, VALUE path, VALUE signal, VALUE encoding,
                          VALUE filetype, VALUE oob)
{
    sox_format_t **c_fmt;
    sox_signalinfo_t *c_signal;
    sox_encodinginfo_t *c_encoding;
    sox_oob_t *c_oob;
    VALUE fmt;

    Data_Get_Struct(signal, sox_signalinfo_t, c_signal);
    Data_Get_Struct(encoding, sox_encodinginfo_t, c_encoding);
    Data_Get_Struct(oob, sox_oob_t, c_oob);
    fmt = Data_Make_Struct(RubySox, sox_format_t *, 0, free, c_fmt);
    *c_fmt = sox_open_write( STR2CSTR(path), c_signal, c_encoding, 
                             STR2CSTR(filetype), c_oob,
                             rb_block_given_p() ? overwrite_callback : NULL );
    return( fmt );
}

VALUE wrap_sox_read(VALUE self, VALUE ft, VALUE buf, VALUE len)
{
    size_t retval;
    sox_format_t *c_ft;
    sox_sample_t *c_buf;

    Data_Get_Struct(ft, sox_format_t, c_ft);
    Data_Get_Struct(buf, sox_sample_t, c_buf);
    retval = sox_read(c_ft, c_buf, NUM2INT(len));

    return( INT2NUM(retval) );
}

VALUE wrap_sox_write(VALUE self, VALUE ft, VALUE buf, VALUE len)
{
    size_t retval;
    sox_format_t *c_ft;
    sox_sample_t *c_buf;

    Data_Get_Struct(ft, sox_format_t, c_ft);
    Data_Get_Struct(buf, sox_sample_t, c_buf);
    retval = sox_write(c_ft, c_buf, NUM2INT(len));

    return( INT2NUM(retval) );
}

VALUE wrap_sox_close(VALUE self, VALUE ft)
{
    size_t retval;
    sox_format_t *c_ft;

    Data_Get_Struct(ft, sox_format_t, c_ft);
    retval = sox_close(c_ft);

    return( INT2NUM(retval) );
}

VALUE wrap_sox_seek(VALUE self, VALUE ft, VALUE offset, VALUE whence)
{
    size_t retval;
    sox_format_t *c_ft;

    Data_Get_Struct(ft, sox_format_t, c_ft);
    retval = sox_seek(c_ft, NUM2LONG(offset), NUM2INT(whence));

    return( INT2NUM(retval) );
}

VALUE wrap_sox_find_format(VALUE self, VALUE name, VALUE no_dev)
{
    sox_format_handler_t const *retval;
    VALUE retruby;

    retval = sox_find_format( (char const *)STR2CSTR(name), NUM2INT(no_dev) );
    retruby = Data_Wrap_Struct(RubySox, 0, free, (sox_format_handler_t *)retval);

    return( retruby );
}

VALUE wrap_sox_find_effect(VALUE self, VALUE name)
{
    sox_effect_handler_t const *retval;
    VALUE retruby;

    retval = sox_find_effect( (char const *)STR2CSTR(name) );
    retruby = Data_Wrap_Struct(RubySox, 0, free, (sox_effect_handler_t *)retval);

    return( retruby );
}

VALUE wrap_sox_create_effect(VALUE self, VALUE eh)
{
    sox_effect_handler_t *c_eh;
    sox_effect_t *retval;
    VALUE retruby;

    Data_Get_Struct(eh, sox_effect_handler_t, c_eh);
    retval = sox_create_effect( c_eh );
    retruby = Data_Wrap_Struct(RubySox, 0, free, retval);

    return( retruby );
}

VALUE wrap_sox_effect_options(VALUE self, VALUE effp, VALUE argc, VALUE argv)
{
    sox_effect_t *c_effp;
    char **c_argv;
    int retval;

    Data_Get_Struct(effp, sox_effect_t, c_effp);
    Data_Get_Struct(argv, char *, c_argv);
    retval = sox_effect_options( c_effp, NUM2INT(argc), c_argv );

    return( INT2NUM(retval) );
}

VALUE wrap_sox_create_effects_chain(VALUE self, VALUE in_enc, VALUE out_enc)
{
    sox_encodinginfo_t *c_in_enc, *c_out_enc;
    sox_effects_chain_t *retval;
    VALUE retruby;

    Data_Get_Struct(in_enc, sox_encodinginfo_t, c_in_enc);
    Data_Get_Struct(out_enc, sox_encodinginfo_t, c_out_enc);
    retval = sox_create_effects_chain( c_in_enc, c_out_enc );
    retruby = Data_Wrap_Struct(RubySox, 0, free, retval);

    return( retruby );
}

VALUE wrap_sox_delete_effects_chain(VALUE self, VALUE ecp)
{
    sox_effects_chain_t *c_ecp;

    Data_Get_Struct(ecp, sox_effects_chain_t, c_ecp);
    sox_delete_effects_chain( c_ecp );

    return( Qnil );
}

VALUE wrap_sox_add_effect(VALUE self, VALUE chain, VALUE effp, VALUE in, VALUE out)
{
    sox_effects_chain_t *c_chain;
    sox_effect_t *c_effp;
    sox_signalinfo_t *c_in, *c_out;
    int retval;

    Data_Get_Struct(chain, sox_effects_chain_t, c_chain);
    Data_Get_Struct(effp, sox_effect_t, c_effp);
    Data_Get_Struct(in, sox_signalinfo_t, c_in);
    Data_Get_Struct(out, sox_signalinfo_t, c_out);
    retval = sox_add_effect( c_chain, c_effp, c_in, c_out );

    return( INT2NUM(retval) );
}


/* 
 * vim:ts=4:sw=4:ai:et:si:sts=4 
 */

