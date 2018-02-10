/**
 * This is a pocketed armour inherit.
 * @author Sandoz, 2002.
 */

#define AC_LIMIT 100 // This is used in pocket_armour as well.

inherit "/std/armour_logic";
inherit CLOTHING_OBJ;

/**
 * This includes the query_ac() function.
 * We split it into a separate file because it is used
 * by standard armour inherit as well, and this way we
 * don't need to modify two files to make a change.
 * - Sandoz
 */
#include <query_ac.h>

/**
 * This method queries wether or not the object is a clothing object.
 * This is an override of the same function in the inherited
 * clothing object.
 * @return always return 0, because this is armour
 */
int query_clothing() { return 0; }

/**
 * This method returns 1 to tell us that it is armour.
 * @return always returns 1
 */
int query_armour() { return 1; }

/** @ignore yes */
void create() {
    do_setup++;
    armour_logic::create();
    clothing::create();
    remove_immune_to("blunt");
    remove_alias("clothing");
    remove_plural("clothes");
    do_setup--;

    if( !do_setup )
        TO->setup();

} /* create() */

/** @ignore yes */
void setup_armour( int i ) { return setup_clothing( i ); }

/** @ignore yes */
mixed stats() {
    return clothing::stats() + armour_logic::stats() +
        ({ ({"max ac", AC_LIMIT }) });
} /* stat() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    return ([
        "::" : clothing::int_query_static_auto_load(),
        "ac" : ac,
        "armour types" : armour_types,
        ]);
} /* query_static_auto_load() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( base_name(TO) != __FILE__[0..<3] )
        return ([ ]);
    return int_query_static_auto_load();
} /* query_static_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( !mapp( map ) )
        return;

    if( map["::"] )
        clothing::init_static_arg( map["::"] );

    if( map["ac"] )
        ac = map["ac"];

    if( map["armour types"] )
        armour_types = map["armour types"];

} /* init_static_arg() */
