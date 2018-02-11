/**
 * The basic container inheritable, please note this is *not* a container
 * like a bucket.  This now allows things inside the container to
 * inform us that they should be used as part of the inventory of this
 * object.  This means the contents of containers will look like they
 * are not inside containers to the outside world.
 * @author Pinkfish
 * @see /obj/container.c
 * @see /obj/baggage.c
 * @see /obj/vessel.c
 * @see /obj/clothing.c
 */

#include <move_failures.h>
#include <player.h>

inherit OBJECT_OBJ;
inherit "/std/basic/cute_look";
inherit "/std/basic/export_inventory";

nosave int max_weight, loc_weight, prevent_insert;

/*
 * This changes the containers properties based on if
 * something with get/drop properties is placed inside it.
 */
private nosave int int_pickup_flag;
private nosave string ownership;
private nosave int n_tracked_items;
private nosave int tracking;

void create() {
    registered_containers = ({ });
    n_tracked_items = 0;
    tracking = 1;
    export_inventory::create();
    object::create();
} /* create() */

/**
 * This method returns the maximum amount of weight that can
 * be carried in this container.
 * @return the maximum weight
 * @see set_max_weight()
 */
int query_max_weight() { return max_weight; }

/**
 * This method sets the maximum amount of weight that can
 * be carried in this container.
 * @param number the new maximum weight
 * @see query_max_weight()
 */
void set_max_weight( int number ) { max_weight = number; }

/**
 * This method returns the current local weight in this
 * container.
 * @return the local weight
 */
int query_loc_weight() { return loc_weight; }

/**
 * This method determines the current local weight from all the
 * objects inside the conatiner
 * @see query_loc_weight()
 */
void update_loc_weight() {
    object thing;

    loc_weight = 0;

    foreach ( thing in INV(TO) )
        loc_weight += thing->query_complete_weight();

} /* update_loc_weight() */

/**
 * This method returns the complete weight of the object.  This is the
 * weight of the container itself, plus the weight of the things
 * inside it
 * @return the complete_weight()
 * @see /std/basic/misc->query_weight()
 * @see query_loc_weight()
 */
int query_complete_weight() {
    return ::query_complete_weight() + loc_weight;
} /* query_complete_weight() */

/**
 * This method is called in the move functions, it adds extra
 * weight onto the object when something is moved inside it.
 * @return 1 if successfuly added
 * @param n the amount to add
 * @see /std/basic/misc->query_weight()
 */
int add_weight( int n ) {
    if( prevent_insert )
        return 0;

    if( !max_weight ) {
        loc_weight += n;
        return 1;
    }

    if( n + loc_weight > max_weight )
        return 0;

    if( !environment() ) {
        loc_weight += n;
        return 1;
    }

    if( !environment()->add_weight( n ) )
        return 0;

    loc_weight += n;
    return 1;

} /* add_weight() */

/**
 * This returns who owns the container, if it set to 0 then no one
 * owns it.  This is used in the theft determination for the object.
 * @return who owns the container
 * @see set_ownership()
 */
string query_ownership() { return ownership; }

/**
 * This sets who owns the container, if it set to 0 then no one
 * owns it.  This is used in the theft determination for the object.
 * @param word who owns the container
 * @see query_ownership()
 */
void set_ownership( string word ) { ownership = word; }

/**
 * This method checks to see if the object can be taken out of
 * us.
 * @param thing the object coming out
 * @param flag the move flag
 * @param dest the destination object
 * @see /std/basic/move.c
 * @return 1 if it can be taken out, 0 if not.
 */
int test_remove( object thing, int flag, mixed dest ) {
    object owner;

    if( !ownership || !TP )
        return 1;

    if( objectp( dest ) )
        dest = file_name( dest );

    if( dest == "/room/rubbish")
        return 1;

    if( TP->query_name() == ownership || !PLAYER_H->test_user( ownership ) )
        return 1;

    if( owner = find_player( ownership ) ) {
        if( !pk_check( TP, owner ) ) {
            event( ENV(TP), "theft", TP, TO, ({ thing }) );
            return 1;
        }
    }

    write("An unseen force stays your hand.\n");
    return 0;

} /* test_remove() */

/**
 * This method allows things to be added into us.  If we have an
 * environment check that for the ability to add into us.
 * @param ob the object being added
 * @param flag the add flags
 * @see /std/basic/move.c
 * @see add_weight()
 */
int test_add( object ob, int flag ) { return 1; }

/**
 * This prevents this container from being added into other containers.
 * @see reset_prevent_insert()
 * @see query_prevent_insert()
 */
int set_prevent_insert() { prevent_insert = 1; }

/**
 * This allows this container to be added into other containers (default).
 * @see set_prevent_insert()
 * @see query_prevent_insert()
 */
int reset_prevent_insert() { prevent_insert = 0; }

/**
 * If this is true, this container cannot be added into other
 * containers.
 * @see reset_prevent_insert()
 * @see set_prevent_insert()
 */
int query_prevent_insert() { return prevent_insert; }

/** @ignore yes */
varargs int move( mixed dest, mixed messin, mixed messout ) {
    if( prevent_insert && loc_weight && !living(dest) && ENV(dest) )
        return MOVE_INVALID_DEST;
    return object::move( dest, messin, messout );
} /* move() */

/**
 * This method finds the matching objects inside this object
 * that are visible to the looker.  This will also use the
 * registered containers and add them into the array returned
 * (if visible and contained in the object).
 * @see /secure/simul_efun->find_match()
 * @param words the words matched on
 * @param looker who is looking
 * @see add_inventory_container()
 */
object *find_inv_match( string words, object looker ) {
    return filter( INV(TO), (: $1->short(0) &&
        ( !$2 || $1->query_visible($2) ) :), looker );
} /* find_inv_match() */

/**
 * This method handles moving objects into the inventory from an auto
 * load.  This should be over ridden by things inheriting us to
 * make sure that the objects can be moved into the inventory.  It
 * should handle the bypassing of open/close/locked etc flags.
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
mixed stats() {
    return ::stats() + ({
        ({"loc_weight", query_loc_weight() }),
        ({"max_weight", query_max_weight() }),
        ({"export invent", query_can_export_inventory() }),
    });
} /* stats() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    return ([
        "::" : ::int_query_static_auto_load(),
        "max weight" : max_weight,
        "prevent insert" : prevent_insert,
        "can export inventory" : can_export_inventory,
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
 * This method enables item tracking
 * @see event_enter_tracked_item
 * @see disable_item_tracking
 */
nomask void enable_item_tracking() { tracking = 1; }

/**
 * This method disables item tracking
 * @see event_enter_tracked_item
 * @see enable_item_tracking
 */
nomask void disable_item_tracking() { tracking = 0; }

/**
 * This method handles the movement of the containers of this object.
 * Tracked items want to be notified if the container moves or anything
 * containing it does.  The movement of the outmost moving container is
 * propagated down to all tracked items.
 * @param mover the outermost container object that moved
 * @param from the start environment of the outermost container
 * @param to the destination of the outermost container
 * @see event_enter_tracked_item
 */
nomask void event_container_move( object mover, mixed from, mixed to ) {
    if( n_tracked_items )
        all_inventory()->event_container_move( mover, from, to );
} /* event_container_move() */

/**
 * This method is called from move to notify any tracked items contained of
 * the move using event_container_moved.
 * @param from start
 * @param to destination
 */
void event_move_object( mixed from, mixed to ) {
    if( n_tracked_items && tracking && !interactive() ) {
        all_inventory()->event_container_move( TO, from, to );
        if( objectp(from) )
            from->remove_tracked_items( n_tracked_items );
        if( objectp(to) )
            to->add_tracked_items( n_tracked_items );
    }
} /* event_move_object() */

/**
 * This method is called to adjust the status reason.
 * occurences.
 * @see event_buried
 * @see event_player_quit
 * @see event_enter_tracked_item
 */
nomask void set_tracked_item_status_reason( string reason ) {
    if( n_tracked_items )
        all_inventory()->set_tracked_item_status( reason );
} /* set_tracked_item_status_reason() */

/**
 * This method handles the addition of tracked items.
 * Tracked items want to be notified if the container moves.
 * @param n_items the number of tracked items added
 * @see remove_tracked_items
 * @see event_container_move
 */
nomask void add_tracked_items( int n_items ) {
    n_tracked_items += n_items;
    if( environment() )
        environment()->add_tracked_items( n_items );
} /* add_tracked_items() */

/**
 * This method handles the removal of tracked items.
 * Tracked items want to be notified if the container moves.
 * @param n_items the number of tracked items  removed
 * @see add_tracked_items
 * @see event_container_move
 */
nomask void remove_tracked_items( int n_items ) {
    n_tracked_items -= n_items;
    if( environment() )
        environment()->remove_tracked_items( n_items );
} /* remove_tracked_items() */

/**
 * This method returns the number of tracked item contained.
 * Tracked items want to be notified if the container moves.
 * @see add_tracked_items
 * @see event_container_move
 */
nomask int query_tracked_items() { return n_tracked_items; }

/**
 * This method allows the container to have stuff inside it checked.
 * @param looker the person doing the checking
 * @return 1 on success, 0 on failur
 */
int can_find_match_recurse_into(object looker) {
    object env;

    // If the looked is one of our environments, then yes, they can.
    env = environment();

    while( env && !living(env) && env != looker && env != ENV(looker) )
        env = ENV(looker);

    return env == looker || env == ENV(looker);

} /* can_find_match_recurse_into() */

/**
 * This method checks to see if the find match code can actually
 * reference this object inside us.
 * @param thing the thing to reference
 * @param looker the person looking at it
 * @return 1 if they can, 0 if they cannot
 */
int can_find_match_reference_inside_object( object thing, object looker ) {
    return 1;
} /* can_find_match_reference_inside_object() */

/** @ignore yes */
void init_dynamic_arg( mapping bing ) {
    function f;

    if( bing[ "::" ] )
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

    if( !undefinedp( bing["prevent insert"] ) )
        prevent_insert = bing["prevent insert"];

    if( !undefinedp( bing["can export inventory"] ) )
        can_export_inventory = bing["can export inventory"];

} /* init_static_arg() */

/** @ignore yes */
mixed query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return ([ ]);
} /* query_static_auto_load() */

/** @ignore yes */
void dest_me() {
    all_inventory(TO)->dest_me();
    ::dest_me();
} /* dest_me() */
