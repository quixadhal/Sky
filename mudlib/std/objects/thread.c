/**
 * Continuous Thread Inherit.
 * @author Sandoz, 18th June 2001.
 * @example
 *
 *   void setup() {
 *       set_thread_colour("white");
 *       set_value_scale( 0.200 );
 *       set_medium_alias("WhiteThreadOnSpool";
 *   }
 *
 */

inherit "/obj/cont_medium";

string thread_colour;

/** @ignore yes */
int query_thread() { return 1; }

/**
 * This method returns the colour of the thread.
 * @return the colour of the thread
 * @see set_thread_colour
 */
string query_thread_colour() { return thread_colour; }

/**
 * This method sets the colour of the thread.
 * @param word the colour of the thread
 * @see query_thread_colour
 */
void set_thread_colour( string word ) { thread_colour = word; }

/** @ignore yes */
void create() {
    thread_colour = "buggy";

    ::create();

    set_name("thread");
    add_alias("spool");
    add_plural("spools");
    add_adjective( ({"spool of", thread_colour }) );
    set_determinate("a ");
    set_short("spool of "+thread_colour+" thread");
    set_main_plural("spools of "+thread_colour+" thread");
    set_long("This is a sturdy wooden spool, around which $amount_size$ "
        "of "+thread_colour+" dyed cotton thread has been wound.\n");

    set_no_join();
    set_weight_unit( ({ 1, 100 }) );
    set_value(1000);
    set_amount(10);
    set_amount_types( ([
        "turn" : ({ 1, "turns" }),
        "skein": ({ 8, "skeins" }),
        ]) );

} /* setup() */

/** @ignore yes */
string amount_size_short() { return ""; }

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;

    if( !query_name() || query_name() == "object")
        return 0;

    map = ([ "::" : ::query_dynamic_auto_load() ]);

    if( thread_colour )
        map["thread colour"] = thread_colour;

    return map;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg( mapping map ) {
    if( map["::"] )
        ::init_dynamic_arg( map["::"] );
    if( map["thread colour"] )
        thread_colour = map["thread colour"];
} /* init_dynamic_arg() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    return ([ "::" : ::int_query_static_auto_load() ]);
} /* int_query_static_auto_load() */

/** @ignore yes */
mixed query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return ([ ]);
} /* query_static_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( !mappingp( map ) )
        return;
    if( map["::"] )
        ::init_static_arg( map["::"] );
} /* init_static_arg() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({ ({ "thread colour", thread_colour }) });
} /* stats() */
