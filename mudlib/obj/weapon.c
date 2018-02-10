/**
 * ok.... now we start thinking about the strange things...
 *   We have several points we want to implement...
 *   1)  A much wider range of damages.
 *   2)  A "To hit" and "damage" rolls being seperate things
 *          But are still related.  ie a good hit will do more damage
 *  Implementation...
 *    Ok, here goes.   The being hit players ac is taken based on the
 *    attack type, the ac will be returned as a base (not randomised)
 *    and a randomised amount...  This will be used to calculate the
 *    cute number we need.  Once we have the number we create the
 *    "To hit" roll for the weapon.  Subtracting of this from our
 *    ac number tells us weather or not we hit...  If we have hit
 *    We take the amount we hit by (after subtracting off the
 *    "To hit bonus") and add it too the extra damge that is done
 *    All this is kept in an array, there can be more than one
 *    attack in the array.  They can be connected together in
 *    several ways, 1) follow on after attack one did more that x
 *    points of damage.  2) have a percentage chance of working
 *    each attack.
 *    A standard set of attacks are defined in /std/weapon_handler
 *    please see that file for more details
 *
 * Modified 18/5/93 by Piecemaker to remove octarine descriptions for non
 *                                wizards.
 */

#include <weapon.h>
#include <move_failures.h>

inherit OBJECT_OBJ;
inherit "/std/weapon_logic";
inherit "/std/basic/condition";
inherit "/std/basic/holdable";

string *un_modifyable;
mixed *wield_func;

void create() {
    set_damage_chance( 25 );
    un_modifyable = ({ });

    weapon_logic::create();
    holdable::create();
    condition::create();
    object::create();

    add_alias("weapon");
    add_plural("weapons");

    if( !query_property("shop type") )
        add_property("shop type", "armoury");

} /* create() */

/**
 * This method returns the hilt name of the weapon.
 * This is used by some of the attack messages.
 * Feel free to overwrite this in your weapon files.
 * @return the short description of the hilt - defaults to "hilt"
 */
string query_hilt() { return "hilt"; }

/**
 * This method sets the wield function of the object.  This will be
 * called when the object is wielded and unwielded.
 * @param func the function to call
 * @param ob the object to call the method on
 */
void set_wield_func( string func, mixed ob ) {
    if( !func )
        wield_func = 0;
    else
        wield_func = ({ func, ob });
} /* set_wield_func() */

/**
 * This method return true if it is a weapon.
 * @return always returns 1
 */
int query_weapon() { return 1; }

/** @ignore yes */
string long( string s, int dark ) { return ::long( s, dark ) + cond_string(); }

/**
 * This method setups the weapon with the new condition.  It sets the
 * maximum and lowest conditions to the specified condition.
 * @param new_condition the condition value of the weapon
 */
void new_weapon( int new_condition ) {
    set_cond( new_condition );
    set_max_cond( new_condition );
    set_lowest_cond( new_condition );
} /* new_weapon() */

/** @ignore yes */
int held_this_item( int held, object holder, mixed arg ) {
    // Let them know if they aren't dexterous or strong enough to hold
    // this weapon effectively.
    if( held == 1 && interactive(holder) ) {
        int i;

        i = COMBAT_H->calc_attack_percentage( holder,
            holder->query_weapons() + ({ TO }),
            holder->query_holding() + ({ TO }) - ({ 0 }), 1 );

        switch( i ) {
          case 0..33 :
            tell_object( holder, "You struggle to hold "+
                query_multiple_short( holder->query_holding() +
                ({ TO }) - ({ 0 }) )+".\n");
          break;
          case 34..66 :
            tell_object( holder, "You struggle somewhat to hold " +
                query_multiple_short( holder->query_holding() +
                ({ TO }) - ({ 0 }) )+".\n");
          break;
          default:
        }
    }

    if( wield_func ) {
        // Unwield first.
        if( !held )
            return call_other( wield_func[1], wield_func[0], 0 );
        else if( held == 1 )
            return call_other( wield_func[1], wield_func[0], holder );
    }

    return 1;

} /* held_this_item() */

/** @ignore yes */
varargs int move( mixed dest, string messin, string messout ) {
    int ret;

    ret = holdable::move(dest);

    if( ret != MOVE_OK )
        return ret;

    return object::move( dest, messin, messout );

} /* move() */

/** @ignore yes */
void dest_me() {
    holdable::dest_me();
    object::dest_me();
} /* dest_me() */

int modify_damage( int val, string name ) {
    val += ( val * query_enchant() ) /
           ( query_max_enchant() + query_enchant() );

    if( member_array( name, un_modifyable ) != -1 )
        return val;

    val = ( val * query_cond() ) / query_max_cond();

    return val;

} /* modify_damage() */

void hit_weapon( int amount, string type ) {
    if( member_array( type, un_modifyable) == -1 )
        do_damage( type, amount );
} /* hit_weapon() */

/* immune to condtion loss */
int add_immune( string name ) {
    if( member_array( name, un_modifyable) != -1 )
        return 0;

    un_modifyable += ({ name });
    return 1;

} /* add_immune() */

int remove_immune( string name ) {
    int i;

    if( ( i = member_array( name, un_modifyable ) ) == -1 )
        return 0;

    un_modifyable = delete( un_modifyable, i, 1 );
    return 1;

} /* remove_immune() */

int query_value() {
   return ( ::query_value() * ( 10 + ( 90 * query_cond() ) /
            query_max_cond() ) ) / 100;
} /* query_value() */

int query_full_value() { return ::query_value(); }

mixed *query_money_array() {
    return MONEY_H->create_money_array( query_value() );
} /* query_money_array() */

int query_money( string type ) {
    int i;
    mixed *m_a;

    m_a = MONEY_H->create_money_array( query_value() );

    if( ( i = member_array( type, m_a ) ) == -1 )
        return 0;

    return m_a[i+1];

} /* query_money() */

/** @ignore yes */
mixed *stats() {
   return object::stats() + condition::stats() + weapon_logic::stats();
} /* stats() */

/** @ignore yes */
void break_me() {
    object ob;

    if( ob = query_holder() ) {
        tell_object( ob, "%^RED%^Your "+short(0)+" breaks!%^RESET%^\n");
        tell_room( ENV(ob), (string)ob->the_short()+"'s "+
            short(0)+" breaks!\n", ob );
        ob->set_unhold( TO );
    }

    ::break_me();

} /* break_me() */

/** @ignore yes */
void player_wield( int pos ) {
    if( !environment() )
        return;

    TO->hold_item( environment(), pos );

} /* player_wield() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return 0;
} /* query_static_auto_load() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    mapping tmp;

    tmp = object::int_query_static_auto_load();

    return ([
        "::" : tmp,
        "attack names" : attack_names,
        "attack data" : attack_data,
        "hold" : holdable::query_static_auto_load(),
        "condition" : condition::query_static_auto_load(),
    ]);

} /* int_query_static_auto_load() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;

    map = ([
        "::" : object::query_dynamic_auto_load(),
        "condition" : condition::query_dynamic_auto_load(),
        "hold" : holdable::query_dynamic_auto_load(),
    ]);

    return map;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( !mapp( map ) )
        return;

    if( map["::"] )
        object::init_static_arg( map["::"] );

    if( map["attack names"] )
        attack_names = map["attack names"];

    if( map["attack data"] )
        attack_data = map["attack data"];

    if( !undefinedp( map["condition"] ) )
        condition::init_static_arg( map[ "condition" ] );

    if( map["no limbs"] ) {
        // The new method handles it differently.
        holdable::init_static_arg( map );
    } else {
        holdable::init_static_arg( map["hold"] );
    }

} /* init_static_arg() */

/** @ignore yes */
void init_dynamic_arg( mapping map, mixed not_used ) {
    if( map["::"] )
        object::init_dynamic_arg( map["::"] );

    if( !undefinedp( map["condition"] ) )
        condition::init_dynamic_arg( map["condition"] );

    holdable::init_dynamic_arg( map["hold"], 1 );

} /* init_dynamic_arg() */
