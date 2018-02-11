/**
 * This is the basic armour object.
 */

#include <move_failures.h>

#define AC_LIMIT 100 // This is used in pocket_armour as well.

#undef INFORM

inherit OBJECT_OBJ;
inherit "/std/basic/wearable";
inherit "/std/basic/holdable";
inherit "/std/armour_logic";

int no_limbs;
nosave int held_in;

/**
 * This includes the query_ac() function.
 * We split it into a separate file because it is used
 * by the pocketed armour inherit as well, and this way
 * we don't need to modify two files to make a change.
 * - Sandoz
 */
#include <query_ac.h>

void create() {
    do_setup++;
    object::create();
    armour_logic::create();
    wearable::create();
    do_setup--;

    add_alias("armour");
    add_plural("armours");

    if( !query_property("shop type") )
        add_property("shop type", "armoury");

    if( !do_setup )
        TO->setup();

} /* create() */

/**
 * This method returns 1 to tell us that it is armour.
 * @return always returns 1
 */
int query_armour() { return 1; }

/** @ignore yes */
string long( string word, int dark ) {
    return object::long( word, dark ) + wearable::long( word, dark );
} /* long() */

/** @ignore yes */
void setup_armour( int number ) {
    set_max_cond( number );
    set_cond( number );
    set_lowest_cond( number );
} /* setup_armour() */

int query_value() {
    return modify_value( object::query_value() );
} /* query_value() */

int query_full_value() { return object::query_value(); }

int query_no_limbs() { return no_limbs; }

void set_no_limbs( int number ) { no_limbs = number; }

mixed set_holder( object thing, int number ) {
    TO->remove_hide_invis("concealed");

    if( !thing ) {
        worn_by = 0;
        held_in = 0;
    } else {
        worn_by = thing;
        held_in = number;
    }

    return 1;

} /* set_holder() */

/**
 * This function is called on an object and causes it to be worn
 * if it can be by it's environment(), i.e the player carrying it.
 */
void player_wear() {
    if( !environment() )
        return;
    if( no_limbs )
        TO->hold_item( environment(), held_in );
    else
        wearable::player_wear();
} /* player_wear() */

/** @ignore yes */
int drop( mixed dest ) {
    if( worn_by && !no_limbs && living( worn_by ) )
        return 1;
    return object::drop(dest);
} /* drop() */

/** @ignore yes */
varargs int move( mixed dest, string messin, string messout ) {
    int flag;

    if( no_limbs && worn_by && worn_by != dest )
        if( !sizeof( (int *)worn_by->set_unhold( TO ) ) )
            return MOVE_NO_UNHOLD;

    flag = object::move( dest, messin, messout );

    if( flag == MOVE_OK && worn_by )
        set_worn_by( 0 );

    return flag;

} /* move() */

/** @ignore yes */
void dest_me() {
    set_worn_by( 0 );
    object::dest_me();
} /* dest_me() */

/** @ignore yes */
mixed stats() {
   return object::stats() + armour_logic::stats() + wearable::stats() +
       ({ ({"max ac", AC_LIMIT }), ({"no limbs", no_limbs }) });
} /* stat() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    return ([
      "::" : object::int_query_static_auto_load(),
      "ac" : ac,
      "armour types" : armour_types,
      "no limbs" : no_limbs,
    ]) + wearable::query_static_auto_load();
} /* query_static_auto_load() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( base_name(TO) != __FILE__[0..<3] )
        return ([ ]);
    return int_query_static_auto_load();
} /* query_static_auto_load() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;

    map = ([ "::" : object::query_dynamic_auto_load() ]) +
                    wearable::query_dynamic_auto_load();

    if( no_limbs )
        map["worn"] = ( worn_by ? held_in + 1 : 0 );

    return map;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( !mapp( map ) )
        return;

    if( map["::"] )
        object::init_static_arg( map["::"] );

    wearable::init_static_arg( map );

    if( map["ac"] )
        ac = map["ac"];

    if( map["armour types"] )
        armour_types = map["armour types"];

    if( map["no limbs"] )
        no_limbs = map["no limbs"];

} /* init_static_arg() */

/** @ignore yes */
void init_dynamic_arg( mapping map, mixed dummy ) {
    if( map["::"] )
        object::init_dynamic_arg( map["::"] );
    wearable::init_dynamic_arg( map );
    if( map["worn"] )
        held_in = map["worn"] - 1;
} /* init_dynamic_arg() */
