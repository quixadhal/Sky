
#include <player.h>

inherit OBJECT_OBJ;
inherit "/std/basic/cute_look";

nosave int loc_weight, max_weight;

/**
 * This method sets our maximum capacity.
 * @param arg the maximum weight to set
 */
void set_max_weight( int arg ) { max_weight = arg; }

/**
 * This method return the maximum weight of things that can fit in us.
 * @return the maximum weight
 */
int query_max_weight() { return max_weight; }

/**
 * This method returns the weight of all things in us.
 * @return the weight of all things within us
 */
int query_loc_weight() { return loc_weight; }

/**
 * This method queries whether or not we are a surface.
 * @return always returns 1
 */
int query_has_surface() { return 1; }

/** @ignore yes */
void update_loc_weight() {
    loc_weight = 0;

    foreach( object ob in INV(TO) )
        loc_weight += ob->query_complete_weight();

} /* update_loc_weight() */

/** @ignore yes */
int query_complete_weight() {
    return ::query_complete_weight() + query_loc_weight();
} /* query_complete_weight() */

/** @ignore yes */
int add_weight( int i ) {
    if( !max_weight )
        return 1;

    if( i + loc_weight > max_weight )
        return 0;

    if( !environment() ) {
        loc_weight += i;
        return 1;
    }

    if( !environment()->add_weight( i ) )
        return 0;

    loc_weight += i;
    return 1;

} /* add_weight() */

/** @ignore yes */
string long( string word, int dark ) {
    object *things;

    if( !sizeof( things = TO->find_inv_match("all", TP ) ) )
        return object::long( word, dark )+"There is nothing on "+
            the_short()+".\n";

    return object::long( word, dark )+query_contents("On "+
        the_short()+" there "+( query_group(things) ? "are" : "is")+" ",
        things );

} /* long() */

/** @ignore yes */
int test_remove( object ob, int flag ) { return 1; }

/** @ignore yes */
int test_add( object ob, int flag ) { return 1; }

/** @ignore yes */
object *find_inv_match( string words, object looker ) {
    object *things;

    things = filter( INV(TO), (: $1->short(0) &&
             ( !$2 || $1->query_visible($2) ) :), looker );

    return things;

} /* find_inv_match() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({
        ({"loc_weight", query_loc_weight() }),
        ({"max_weight", query_max_weight() }),
    });
} /* stats() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    return ([
        "::" : ::int_query_static_auto_load(),
        "max weight" : max_weight,
    ]);
} /* int_query_static_auto_load() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mixed inventory;

    // Check the player's object.
    if( TP )
        catch( inventory = TP->create_auto_load( INV(TO) ) );

    // Try the autoload object.
    if( !inventory )
        catch( inventory = AUTO_LOAD_OB->create_auto_load( INV(TO) ) );

    // Try the main player object.
    if( !inventory )
        catch( inventory = PLAYER_OBJ->create_auto_load( INV(TO) ) );

    return ([
        "::" : object::query_dynamic_auto_load(),
        "inv" : inventory,
    ]);

} /* query_dynamic_auto_load() */

/**
 * This method handles moving objects into the inventory from an auto
 * load.
 * @param ob the object to move into ourselves
 */
protected int handle_restore_inventory( object ob ) {
    // The standard container needs to do nothing special.
    int move_flag;

    ob->disable_item_tracking();
    move_flag = ob->move(TO);
    ob->enable_item_tracking();

    return move_flag;

} /* handle_restore_inventory() */

/** @ignore yes */
void init_dynamic_arg( mapping bing ) {
    function f;

    if( bing["::"] )
        ::init_dynamic_arg( bing["::"] );

    // Potential order of inventory generation problem here.
    // The upper parts of the container don't initialise until after we return.
    if( bing["inv"] ) {
        f = (: handle_restore_inventory($1) :);
        if( !player )
            player = TP;
        if( player )
            player->load_auto_load_to_inventory( bing["inv"], TO, player, f );
        else
            PLAYER_OBJ->load_auto_load_to_inventory( bing["inv"], TO, TP, f );
    }

} /* init_dynamic_arg() */

/** @ignore yes */
void init_static_arg( mapping bing ) {
    if( bing["::"] )
        ::init_static_arg( bing["::"] );

    if( !undefinedp( bing["max weight"] ) )
        max_weight = bing["max weight"];

} /* init_static_arg() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( base_name(TO) != __FILE__[0..<3] )
        return 0;
    return int_query_static_auto_load();
} /* query_static_auto_load() */

/** @ignore yes */
void dest_me() {
    INV(TO)->dest_me();
    object::dest_me();
} /* dest_me() */
