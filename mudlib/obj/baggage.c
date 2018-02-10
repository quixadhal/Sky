/**
 * Copied from /obj/container.c.  The idea is to remove the
 * liquid and potion-space stuff, since it's not needed for chests
 * and sacks.
 * @author Jeremy, Pinkfish, Ember
 * @see /obj/vessel.c
 */

#include <move_failures.h>

inherit "/std/container";
inherit "/std/basic/close_lock_container";
inherit "/std/basic/condition";
inherit "/std/basic/holdable";

void create() {
    do_setup++;
    container::create();
    close_lock_container::create();
    condition::create();
    holdable::create();
    set_max_cond(400);
    set_cond(400);
    set_damage_chance(20);
    set_can_export_inventory();
    set_opaque();
    do_setup--;
    if( !do_setup )
        TO->setup();
} /* create() */

/**
 * This method sets up the condition of the baggage.
 * @see setup clothing()
 * @see setup_armour()
 */
void setup_baggage( int number ) {
    set_max_cond( number );
    set_cond( number );
    set_lowest_cond( number );
} /* setup_baggage() */

/** @ignore yes */
void init() {
    close_lock_container::init();
} /* init() */

/** @ignore yes */
int add_weight( int n ) {
    if( !::add_weight( n ) )
        return 0;
    if( n >= 0 ) {
        remove_call_out("check_breakages");
        call_out("check_breakages", 5 + random( 16 ) );
    }
    return 1;
} /* add_weight() */

/**
 * This method checks to see if any of the things contained in the
 * container should be broken.
 * @see /std/container->add_weight()
 */
void check_breakages() {
    /* check loc_weight against fragility of every object and break some
     *   if necessary.  If the container has the "padded" property, adjust
     *   the chance of breakage.
     */
    object *obs, carrier;
    int i, amt, wt;

    // See if it's being carried by a living object
    carrier = ENV(TO);
    while( carrier && !living(carrier) )
        carrier = ENV(carrier);

    if( !carrier )
        return;

    obs = all_inventory();
    wt = loc_weight - (int)query_property("padded");

    for( i = 0; i < sizeof(obs); i++ )
        if( amt = obs[i]->query_property("fragile") ) {
            if( wt <= amt || ( ( wt - amt ) * 100 ) / amt <= random(100) )
                obs[i] = 0;
        } else
            obs[i] = 0;

    obs -= ({ 0 });

    if( sizeof(obs) && environment(carrier) ) {
        tell_room( ENV(carrier), carrier->the_short()+" breaks "+
            ( query_group(obs) ? "some things" : "something")+" in "+
            poss_short()+".\n", ({ carrier }) );
        tell_object( carrier, "You break "+query_multiple_short(obs)+" in "+
            poss_short()+".\n");
        obs->dest_me();
    }

} /* check_breakages() */

/** @ignore yes */
string long( string word, int dark ) {
    string ret;

    ret = ::long( word, dark );
    ret += cond_string();

    if( query_transparent() || !query_closed() ) {
        if( dark == 2 || dark == -2 ) {
            if( query_contents() != "")
                ret += PLAYER_OBJ->convert_message("$C$$the_short:"+
                    file_name(TO)+"$ contains some items you cannot "
                    "make out.\n");
        } else {
            ret += query_contents( PLAYER_OBJ->convert_message(
                "$C$$the_short:"+file_name(TO)+"$ contains ") );
        }
    }

    return ret + long_status();

} /* long() */

/**
 * This method returns true if the object is open and prints a message
 * about the open status of the object.
 * @return 1 if it is open, 0 if not
 */
int ensure_open() {
    if( query_locked() ) {
        write( the_short()+" $V$0=is,are$V$ locked.\n");
        return 0;
    }

    if( query_closed() ) {
        if( do_open() ) {
            write("You open "+the_short()+".\n");
            return 1;
        } else {
            write( "You can't open "+the_short()+".\n");
            return 0;
        }
    }

    return 1;

} /* ensure_open() */

/** @ignore yes */
mixed stats() {
    return container::stats()+close_lock_container::stats()+condition::stats();
} /* stats() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    mapping map;
    mixed tmp;

    map = ([ "::" : container::int_query_static_auto_load(),
             "condition" : condition::query_static_auto_load(),
             "hold" : holdable::query_static_auto_load(),
             "difficulty" : query_difficulty(),
             ]);

    if( tmp = query_transparent() )
        map["trans"] = tmp;

    if( tmp = query_key() )
        map["key"] = tmp;

    if( tmp = query_open_trap_func() )
        map["trap open func"] = tmp;

    if( tmp = query_lock_trap_func() )
        map["trap lock func"] = tmp;

    if( tmp = query_open_trap_ob() )
        map["trap open ob"] = tmp;

    if( tmp = query_lock_trap_ob() )
        map["trap lock ob"] = tmp;

    return map;

} /* int_query_static_auto_load() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;
    mixed tmp;

    map = ([ "::" : container::query_dynamic_auto_load(),
             "condition" : condition::query_dynamic_auto_load(),
             "hold" : holdable::query_dynamic_auto_load(),
             ]);

    if( tmp = query_locked() )
        map["locked"] = tmp;

    if( tmp = query_stuck() )
        map["stuck"] = tmp;

    if( tmp = query_closed() )
        map["closed"] = tmp;

    return map;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg( mapping map, mixed dummy ) {
    object money;

    if( map["::"] )
        container::init_dynamic_arg( map["::"] );

    if( map["condition"] )
        condition::init_dynamic_arg( map["condition"] );

    if( map["hold"] )
        holdable::init_dynamic_arg( map["hold"] );

    if( sizeof( map["money"] ) ) {
        money = clone_object(MONEY_OBJ);
        money->set_money_array( map["money"] );
        money->move(TO);
    }

    if( map["closed"] )
        set_closed();
    else
        set_open();

    if( map["locked"] )
        set_locked();
    else
        set_unlocked();

    set_stuck( map["stuck"] );

} /* init_dynamic_arg() */

/** @ignore yes */
void init_static_arg( mapping args ) {
    if( args["::"] )
        ::init_static_arg(args["::"]);

    if( args["condition"] )
        condition::init_static_arg(args["condition"]);

    if( args["hold"] )
        holdable::init_static_arg(args["hold"]);

    if( args["trans"] )
        set_transparent();
    else
        set_opaque();

    set_difficulty( args["difficulty"] );
    set_key( args["key"] );

    if( !undefinedp( args["trap open func"] ) )
        set_open_trap( args["trap open ob"], args["trap open func"] );

    if( !undefinedp( args["trap lock func"] ) )
        set_lock_trap( args["trap lock ob"], args["trap lock func"] );

    // Make sure we don't have infinite capacity bags.
    if( !query_max_weight() )
         set_max_weight(5);

} /* init_static_arg() */

/** @ignore yes */
mixed query_static_auto_load() {
    if( !query_name() || query_name() == "object")
        return 0;

    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();

    return ([ ]);

} /* query_static_auto_load() */

/** @ignore yes */
int can_find_match_recurse_into( object looker ) {
    if( query_closed() )
        return 0;
    return ::can_find_match_recurse_into( looker );
} /* can_find_match_recurse_into() */

/**
 * @ignore yes
 * This is added so that it acts like a living object and
 * things like non-movable signs cannot be added to it.
 */
int test_add( object ob, int flag ) {
    return !flag;
} /* test_add() */

/**
 * @ignore yes
 * Added here to make it expose its inventory upwards when moved.
 */
varargs int move( mixed *dest, string mess1, string mess2 ) {
    object from;
    int result;

    result = holdable::move(dest);
    if( result != MOVE_OK )
        return result;

    from = environment();
    result = container::move( dest, mess1, mess2 );

    if( result == MOVE_OK )
        we_moved( from, environment() );

    return result;

} /* move() */

/** @ignore yes */
void break_me() {
    all_inventory()->move( environment(), "$N fall$s out of "+a_short()+".");
    ::break_me();
} /* break_me() */

/** @ignore yes */
void dest_me() {
    close_lock_container::dest_me();
    container::dest_me();
} /* dest_me() */
