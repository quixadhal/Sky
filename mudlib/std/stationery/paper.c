
inherit OBJECT_OBJ;

private int crumpled;

/**
 * This method returns 1 to notify that we can be written on,
 * or 0 if we are crumpled and cannot be written on.
 * @return 1 if we can be written on, 0 if not
 */
int query_writeable() { return !crumpled; }

/** @ignore yes */
void create() {
    do_setup++;
    ::create();
    do_setup--;

    set_name("paper");

    if( !function_exists("setup", TO ) ) {
        set_short("piece of writing paper");
        add_adjective( ({"piece", "pieces", "of", "writing"}) );
        set_long("This is a nice sheet of writing paper.\n");
    }

    add_extra_look(TO);
    set_value(100);
    set_material("wood");
    add_property("paper", 1 );
    set_max_size(1000);

    if( !do_setup )
        TO->setup();

} /* create() */

/** @ignore yes */
void init() {
    add_command("eat");
    add_command("crumple");
    add_command("untangle");
    add_command("uncrumple");
} /* init() */

/** @ignore yes */
int do_eat() {
    move("/room/rubbish");
    return 1;
} /* do_eat() */

/**
 * This method queries whether or not this object is a paper.
 * @return always returns 1
 */
int query_paper() { return 1; }

/**
 * This method queries whether or not this paper is edible.
 * @return always returns 1
 */
int query_edible() { return 1; }

/** @ignore yes */
string short( int dark ) {
    if( crumpled && !query_property("being crumpled") )
        return "crumpled up "+::short( dark );
    return ::short( dark );
} /* short() */

/** @ignore yes */
string extra_look( object ob ) {
    if( ob != TO || !crumpled )
        return "";
    return "It appears to have been crumpled.\n";
} /* extra_look() */

/** @ignore yes */
int do_crumple() {
    if( crumpled ) {
        add_failed_mess("$D $V$0=is,are$V$ already crumpled.\n");
        return 0;
    }

    add_property("being crumpled", 1, 2 );
    crumpled = 1;
    return 1;

} /* do_crumple() */

/** @ignore yes */
int do_untangle() {
    if( !crumpled ) {
        add_failed_mess("$D $V$0=is,are$V$ already $Vd.\n");
        return 0;
    }

    crumpled = 0;
    return 1;

} /* do_untangle() */

/** @ignore yes */
int do_uncrumple() { return do_untangle(); }

/** @ignore yes */
string query_readable_message( object player, int ignore_labels ) {
    string ret;

    ret = ::query_readable_message( player, ignore_labels );

    if( !crumpled || !ret )
        return ret;

    return "Only bits of ink can be seen on the crumpled paper.\n";

} /* query_readable_message() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;

    map = ([ "::" : ::query_dynamic_auto_load() ]);

    if( crumpled )
        map["crumpled"] = crumpled;

    return map;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg( mapping map ) {
    if( mapp(map) ) {
        ::init_dynamic_arg( map["::"] );
        crumpled = map["crumpled"];
    }
} /* init_dynamic_arg() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return ([ ]);
} /* query_static_auto_load() */

/** @ignore yes */
string *parse_command_adjectiv_id_list() {
    if( crumpled )
        return ::parse_command_adjectiv_id_list() + ({"crumpled", "up"});
    return ::parse_command_adjectiv_id_list();
} /* parse_command_adjectiv_id_list() */

/** @ignore yes */
mixed stats() { return ::stats() + ({ ({"crumpled", crumpled }) }); }
