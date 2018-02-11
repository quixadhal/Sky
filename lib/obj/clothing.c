/**
 * This is the clothing file.  Contains everything you need to
 * make some nice clothing.
 */

#include <move_failures.h>

// defines for scabbards, reusing pocket variables.
#define SHEATH   pocket_mess
#define TYPES    pockets
#define SCABBARD biggest

inherit "/std/container";
inherit "/std/basic/wearable";
inherit "/std/basic/close_lock";

string pocket_mess;
mixed pockets;
int biggest;

/** @ignore yes */
private void make_scabbard();

void create() {
    do_setup++;
    container::create();
    wearable::create();
    close_lock::create();
    set_stuck(1);
    set_can_export_inventory();
    add_alias("clothing");
    add_plural("clothes");
    add_immune_to("blunt");
    set_opaque();

    if( !query_property("shop type") )
        add_property("shop type", "haberdashery");

    do_setup--;
    if( !do_setup )
        TO->setup();

} /* create() */

/**
 * This method makes the clothing object a clothing-scabbard,
 * which means it will be fixable by 'mend' etc.
 * It will allow to have sheathes for daggers inside bras etc.
 * @see query_scabbard()
 * @see query_clothing()
 * @see set_types()
 * @see query_types()
 * @see set_no_weapons_allowed()
 * @see query_no_weapons_allowed()
 * @see test_type_ok()
 */
void set_clothing_scabbard() {
    SCABBARD = -2;
    add_property("shop type", "haberdashery");
    add_immune_to("blunt");
    add_alias("clothing");
    add_plural("clothes");
} /* set_clothing_scabbard() */

/**
 * This method determines if the object is a scabbard or not.
 * @return returns 1 for a scabbard
 * @see query_clothing()
 * @see set_types()
 * @see query_types()
 * @see set_no_weapons_allowed()
 * @see query_no_weapons_allowed()
 * @see set_clothing_scabbard()
 * @see test_type_ok()
 */
int query_scabbard() { return SCABBARD < 0 && sizeof(TYPES); }

/**
 * This method will return true if the item is an item of clothing.
 * @return returns true if it's a clothing item
 * @see /obj/clothing->query_clothing()
 * @see query_scabbard()
 * @see set_types()
 * @see query_types()
 * @see set_no_weapons_allowed()
 * @see query_no_weapons_allowed()
 * @see set_clothing_scabbard()
 * @see test_type_ok()
 */
int query_clothing() { return SCABBARD != -1; }

/**
 * This method sets the number of weapons allowed into the scabbard.
 * @param i the number of weapons allowed into the scabbard
 * @see query_scabbard()
 * @see query_clothing()
 * @see set_types()
 * @see query_types()
 * @see query_no_weapons_allowed()
 * @see set_clothing_scabbard()
 * @see test_type_ok()
 */
void set_no_weapons_allowed( int i ) {

    if( i == 1 )
        remove_property("_NWA_");
    else if( i > 0 )
        add_property("_NWA_", i );
    else
        return;

    call_out( (: make_scabbard :), 0 );

} /* set_no_weapons_allowed() */

/**
 * This method returns the number of weapons allowed into the scabbard.
 * @return the number of weapons allowed into the scabbard.
 * @see query_scabbard()
 * @see query_clothing()
 * @see set_types()
 * @see query_types()
 * @see set_no_weapons_allowed()
 * @see set_clothing_scabbard()
 * @see test_type_ok()
 */
int query_no_weapons_allowed() { return query_property("_NWA_"); }

/**
 * This method returns the types of weapons that can be
 * sheathed in the scabbard.
 * @return the types that can be sheathed
 * @see query_scabbard()
 * @see query_clothing()
 * @see set_types()
 * @see set_no_weapons_allowed()
 * @see query_no_weapons_allowed()
 * @see set_clothing_scabbard()
 * @see test_type_ok()
 */
string *query_types() { return ( query_scabbard() ? TYPES : ({ }) ); }

/**
 * This method returns the message associated with the scabbard.
 * @param flag if the flag is set then the message will be
 * constructed without the "It could sheath" bit.
 * @return the message associated with the scabbard
 * @see query_scabbard()
 */
varargs string query_scabbard_mess( int flag ) {
    if( query_scabbard() ) {
        int num;
        string mess;

        if( !num = query_no_weapons_allowed() )
            num = 1;

        mess = "";

        if( !flag )
            mess += ({"It", "They"})[query_group(TO)]+" could sheath ";

        switch( sizeof(TYPES) ) {
          case 0 :
            return "";
          case 1 :
            return mess + ( num > 1 ? query_num(num)+" "+pluralize(TYPES[0]) :
                   add_a(TYPES[0]) )+( !flag ? ".\n" : "");
          case 2 :
            return mess + ( num > 1 ? query_num(num)+" "+pluralize(TYPES[0]) :
                   add_a(TYPES[0]) )+" or "+( num > 1 ? pluralize(TYPES[1]) :
                   add_a(TYPES[1]) )+( !flag ? ".\n" : "");
          default :
            return mess + ( num > 1 ? query_num(num)+" "+
                   implode( map( TYPES[1..], (: pluralize($1) :) ), ", ") :
                   implode( map( TYPES[1..], (: add_a($1) :) ), ", ") )+" or "+
                   ( num > 1 ? pluralize( TYPES[0] ) : add_a(TYPES[0]) )+
                   ( !flag ? ".\n" : "");
        }
    }

    return "";

} /* query_scabbard_mess() */

/** @ignore yes */
private void make_scabbard() {

    add_alias("scabbard");
    add_plural("scabbards");

    if( SCABBARD == -1 ) {
        add_property("shop type", "armoury");
        remove_immune_to("blunt");
        remove_alias("clothing");
        remove_plural("clothes");
    }

    SHEATH = query_scabbard_mess();

} /* make_scabbard() */

/**
 * This method sets the types of weapons that can be
 * sheathed in the scabbard.
 * @param words the array of types that can be sheathed
 * @see query_scabbard()
 * @see query_clothing()
 * @see query_types()
 * @see set_no_weapons_allowed()
 * @see query_no_weapons_allowed()
 * @see set_clothing_scabbard()
 * @see test_type_ok()
 */
void set_types( string *words ) {

    TYPES = words;

    if( SCABBARD > -1 )
        SCABBARD = -1;

    call_out( (: make_scabbard :), 0 );

} /* set_types() */

/** @ignore yes */
string long( string word, int dark ) {
    int i;
    string ret, *sizes;
    mapping types;

    ret = container::long( word, dark );

    if( dark == 2 || dark == -2 )
        return ret;

    if( query_scabbard() ) {
        i = query_group(TO);
        return ret + SHEATH + query_contents( ( !i ? "It contains: " :
            "They contain: ") ) + wearable::long( word, dark );
    }

    if( sizeof( pockets ) && !dark ) {
        types = ([ ]);

        for( i = 0; i < sizeof( pockets ); i += 2 )
            types[ pockets[ i ] ]++;

        sizes = keys( types );
        sizes = map( sizes, (: query_num($2[$1]) + " " + $1 + " pocket"+
                   ( $2[$1] > 1 ? "s" : "") :), types );

        i = query_group(TO);

        ret += PLAYER_OBJ->convert_message("$C$"+the_short() )+" has "+
            query_multiple_short( sizes )+".\n"+
            query_contents( ( !i ? "It contains: " : "They contain: ") );
        ret += close_lock::long_status();

    }

    return ret + wearable::long( word, dark );

} /* long() */

/** @ignore yes */
varargs string pretty_short( object thing ) {
    return close_lock::short_status() + ::pretty_short( thing );
} /* pretty_short() */

/** @ignore yes */
varargs string pretty_plural( object thing ) {
    return close_lock::short_status() + ::pretty_plural( thing );
} /* pretty_plural() */

/**
 * This method returns the message associated with the pocket.
 * @return the message associated with the pocket
 * @see set_pocket_mess()
 * @see query_pockets()
 * @see add_pocket()
 */
string query_pocket_mess() {
    return ( !query_scabbard() ? pocket_mess : "" );
} /* query_pocket_mess() */

/**
 * This method sets the message associated with the pocket.
 * @param words the message associated with the pocket
 * @see query_pocket_mess()
 * @see add_pocket()
 */
void set_pocket_mess( string words ) {
    if( !query_scabbard() )
        pocket_mess = words;
} /* set_pocket_mess() */

/**
 * This method returns all the pockets on the object.
 * @return all the pockets on the object
 * @see add_pocket()
 */
mixed query_pockets() {
    return ( !query_scabbard() && sizeof(pockets) ? pockets : ({ }) );
} /* query_pockets() */

/**
 * This method adds a pocket onto the clothing.
 * @param type the type of pocket
 * @param amount the amount the pocket can hold
 * @see query_pockets()
 * @see set_pocket_mess()
 * @see query_pocket_mess()
 */
void add_pocket( string type, int amount ) {
    if( !type || type == "" || amount < 1 || query_scabbard() )
        return;

    if( !pocket_mess )
        pocket_mess = "a pocket in $ob_short$";

    if( !pointerp( pockets ) )
        pockets = ({ });

    pockets += ({ type, amount });
    max_weight += amount;

    if( amount > biggest )
        biggest = amount;

} /* add_pocket() */

/**
 * This method queries whether or not this weapon
 * can be sheathed in the scabbard.
 * @param thing the weapon to test
 * @param flag the move flag (see test_add)
 * @return 1 if the weapon can be sheathed in the scabbard, 0 if not
 * @see query_scabbard()
 * @see query_clothing()
 * @see set_types()
 * @see query_types()
 * @see set_no_weapons_allowed()
 * @see query_no_weapons_allowed()
 * @see set_clothing_scabbard()
 */
int test_type_ok( object thing, int flag, int test ) {
    string word;
    int i;

    if( !ENV( thing ) || !query_scabbard() )
        return ::test_add( thing, flag );

    foreach( word in TYPES )
        if( sizeof( match_object_in_array( word, ({ thing }) )->objects ) ) {
            if( !test ) {
                if( !i = query_no_weapons_allowed() )
                    i = 1;

                if( sizeof( INV(TO) ) >= i ) {
                    write( the_short()+" already holds "+
                        query_multiple_short( INV(TO) )+".\n");
                    return 0;
                }

                if( TO->can_not_sheathe( thing ) )
                    return 0;
            }

            return ::test_add( thing, flag );

        }

    if( !test )
        write( thing->the_short()+" doesn't fit very well in "+
            the_short()+".  "+SHEATH );
    return 0;

} /* test_type_ok() */

/** @ignore yes */
int test_add( object thing, int flag ) {
    int i;

    // no pockets nor scabbard types.
    if( !sizeof( pockets ) || flag )
        return 0;

    if( query_scabbard() )
        return test_type_ok( thing, flag, 0 );

    if( !ENV( thing ) )
        return 1;

    if( !biggest ) {
        for( i = 0; i < sizeof( pockets ); i+= 2 ) {
            if( pockets[ i + 1 ] > biggest )
                biggest = pockets[ i + 1 ];
        }
    }

    if( (int)thing->query_complete_weight() > biggest )
        return write( (string)thing->the_short() +" is too big "+
            "to fit in any of "+the_short()+"'s pockets.\n" );

    return 1;

} /* test_add() */

/** @ignore yes */
int query_ac( string type, int amount ) {
    do_damage( type, amount );
    return 0;
} /* query_ac() */

/**
 * This method sets up the condition for the clothing.
 * @param number the maximum condition for the clothing
 */
void setup_clothing( int number ) {
    set_max_cond( number );
    set_cond( number );
    set_lowest_cond( number );
} /* setup_clothing() */

/** @ignore yes */
int query_value() { return modify_value( container::query_value() ); }

/** @ignore yes */
int query_full_value() { return container::query_value(); }

/** @ignore yes */
int drop( mixed stuff ) {
    if( worn_by && living( worn_by ) )
        return 1;
    return container::drop(stuff);
} /* drop() */

/** @ignore yes */
varargs int move( mixed dest, string messin, string messout ) {
    int flag;
    object from;

    from = ENV(TO);
    flag = container::move( dest, messin, messout );

    if( !flag && worn_by ) {
        set_worn_by( 0 );
        if( from )
            from->remove_inventory_container(TO);
        if( ENV(TO) )
            ENV(TO)->add_inventory_container(TO);
    }

    return flag;

} /* move() */

/** @ignore yes */
void dest_me() {
    set_worn_by( 0 );
    container::dest_me();
} /* dest_me() */

/**
 * This method causes the object to be broken.
 */
void break_me() {
    object env;

    if( living( ENV(TO) ) )
        env = ENV(TO);

    if( env || ( env = worn_by ) )
        INV(TO)->move( ENV(env), "$N fall$s out from "+
            env->poss_short()+" "+short()+".");
    else
        INV(TO)->move( ENV(TO), "$N fall$s out of "+the_short()+".");

    ::break_me();

} /* break_me() */

/** @ignore yes */
mixed stats() {
    mixed i, ret;

    ret = container::stats() + wearable::stats();

    if( query_scabbard() && sizeof(TYPES) ) {
        foreach( i in TYPES )
           ret += ({ ({ "scabbard type", i }) });
    } else if( sizeof(pockets) )
        for( i = 0; i < sizeof( pockets ); i += 2 )
           ret += ({ ({ pockets[ i ] +" pocket", pockets[ i + 1 ] }) });

    return ret;

} /* stats() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    mapping map;

    map = ([
        "::" : container::int_query_static_auto_load(),
        "wear" : wearable::query_static_auto_load(),
        ]);

    if( sizeof(pockets) )
        map["pockets"] = pockets;
    if( pocket_mess )
        map["pocket mess"] = pocket_mess;
    if( query_transparent() )
        map["trans"] = query_transparent();
    map["stuck"] = query_stuck();
    if( query_difficulty() )
        map["difficulty"] = query_difficulty();
    if( query_key() )
        map["key"] = query_key();
    if( query_open_trap_func() )
        map["trap open func"] = query_open_trap_func();
    if( query_lock_trap_func() )
        map["trap lock func"] = query_lock_trap_func();
    if( query_open_trap_ob() )
        map["trap open ob"] = query_open_trap_ob();
    if( query_lock_trap_ob() )
        map["trap lock ob"] = query_lock_trap_ob();
    if( biggest < 0 )
        map["biggest"] = biggest;
    return map;

} /* int_query_static_auto_load() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( base_name(TO) != __FILE__[0..<3] )
        return ([ ]);
    return int_query_static_auto_load();
} /* query_static_auto_load() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;

    map = ([
        "::" : container::query_dynamic_auto_load(),
        "wear" : wearable::query_dynamic_auto_load(),
        ]);

    if( query_locked() )
        map["locked"] = query_locked();
    if( query_closed() )
        map["closed"] = query_closed();

    return map;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( !mapp( map ) )
        return;

    if( map[ "::" ] )
        container::init_static_arg( map[ "::" ] );
    if( map[ "wear" ] )
        wearable::init_static_arg( map[ "wear" ] );
    if( map[ "pocket mess" ] )
        pocket_mess = map[ "pocket mess" ];
    if( map[ "pockets" ] )
        pockets = map[ "pockets" ];
    if( map["trans"] )
        set_transparent();
    else
        set_opaque();
    if( map["difficulty"] )
        set_difficulty( map["difficulty"] );
    if( map["key"] )
        set_key( map["key"] );
    if( map["trap open func"] )
        set_open_trap( map["trap open ob"], map["trap open func"] );
    if( map["trap lock func"] )
        set_lock_trap( map["trap lock ob"], map["trap lock func"] );
    if( !undefinedp( map["stuck"] ) )
        set_stuck( map["stuck"] );
    if( map["biggest"] )
        biggest = map["biggest"];
    if( SCABBARD == -2 )
        add_immune_to("blunt");

    // call out because of the properties.
    if( query_scabbard() )
        call_out( (: make_scabbard :), 0 );

} /* init_static_arg() */

/** @ignore yes */
void init_dynamic_arg( mapping map ) {

    if( map[ "::" ] )
        container::init_dynamic_arg( map["::"] );
    if( map[ "wear" ] )
        wearable::init_dynamic_arg( map["wear"] );

    if( map[ "locked" ] )
        set_locked();
    else
        set_unlocked();

    if( map["closed" ] )
        set_closed();
    else
        set_open();

} /* init_dynamic_arg() */
