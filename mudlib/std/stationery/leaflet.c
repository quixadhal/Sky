/**
 * This is the new handler based leaflet object.
 * @author Sandoz, 2002.
 */

inherit OBJECT_OBJ;

private int id, printer_id;
private string size;

/**
 * This method returns the raw long description of the leaflet.
 */
string leaflet_long() {
    return replace_string( LEAFLET_H->query_leaflet_long(printer_id),
        "$size$", size || "average sized" );
} /* leaflet_long() */

/** @ignore yes */
void create() {
    do_setup++;
    ::create();
    do_setup--;

    if( !do_setup )
        TO->setup();

    set_long( (: leaflet_long :) );

} /* create() */

/**
 * This method queries whether or not we are a leaflet.
 * @return always returns 1
 */
int query_leaflet() { return 1; }

/** @ignore yes */
int query_writeable() { return 1; }

/**
 * This method returns our text array.
 * @return the pages data
 */
mixed query_text() { return LEAFLET_H->query_text(id); }

/** @ignore yes */
mixed query_read_mess() {
    mixed mess;

    if( id == -1 || !sizeof( mess = query_text() ) )
        return ::query_read_mess();

    return ({ query_text() }) + ::query_read_mess();

} /* query_read_mess() */

/**
 * This method sets the size of the leaflet.
 * @param str the size to set us to
 */
void set_leaflet_size( string str ) {
    size = str;
} /* set_leaflet_size() */

/**
 * This method returns the size of the leaflet.
 * @return the size of the leaflet
 */
string query_leaflet_size() { return size; }

/**
 * This method sets the id of the leaflet,
 * so that we can get our content from the leaflet handler.
 * @param i the id to set us to
 */
void set_leaflet_id( int i ) {
    id = i;
} /* set_leaflet_id() */

/**
 * This method returns the id of the leaflet.
 * @return the id of the leaflet
 */
int query_leaflet_id() { return id; }

/**
 * This method sets the id of the printer of the leaflet,
 * so that we can get our printer name from the leaflet handler.
 * @param i the id to set out printer to
 */
void set_printer_id( int i ) {
    printer_id = i;
} /* set_printer_id() */

/**
 * This method returns the id of the printer of the leaflet.
 * @return the id of the printer
 */
int query_printer_id() { return printer_id; }

/**
 * This method returns the print shop name who printed us.
 * @return the name of out printer
 */
string query_printer_name() {
    return LEAFLET_H->query_printer_name( printer_id );
} /* query_printer_name() */

/**
 * This method returns the author of the leaflet.
 * @return the author of the leaflet
 */
string query_author() { return LEAFLET_H->query_author(id); }

/**
 * This method returns the copyright holder of the leaflet.
 * @return the copyright holder of the leaflet
 */
string query_copyright() { return LEAFLET_H->query_copyright(id); }

/** @ignore yes */
mapping query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return ([ ]);
} /* query_static_auto_load() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;
    int tmp;

    tmp = id;
    id = -1;
    map = ([ "::" : ::query_dynamic_auto_load() ]);
    id = tmp;

    if( id )
        map["id"] = id;
    if( printer_id )
        map["printer id"] = printer_id;
    if( size )
        map["size"] = size;

    return map;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg( mapping map ) {
    if( !mapp( map ) )
        return;
    if( map["::"] )
        ::init_dynamic_arg( map["::"] );
    id = map["id"];
    printer_id = map["printer id"];
    size = map["size"];
    set_long( (: leaflet_long :) );
} /* init_dynamic_arg() */

/** @ignore yes */
string *parse_command_adjectiv_id_list() {
    if( size )
        return explode( size, " ") + ::parse_command_adjectiv_id_list();
    return ::parse_command_adjectiv_id_list();
} /* parse_command_adjectiv_id_list() */


/** @ignore yes */
mixed stats() {
    return ::stats() + ({
        ({ "id", id }),
        ({ "author", query_author() }),
        ({ "copyright", query_copyright() }),
        ({ "printer id", query_printer_id() }),
        ({ "printed in", query_printer_name() }),
        ({ "size", query_leaflet_size() }),
    });
} /* stats() */
