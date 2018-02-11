/**
 * This is the vault inheritable object. There are two inheritables
 * (this one and /std/shops/vault.c) which are provided to make it
 * easy to setup your own vaults.  This object should be inherited
 * by the vault entrance area.  It is used to define most of the
 * customisation for the vault.
 *
 * You _must_ use set_vault_name and set_vault_entrance.  You must
 * also define a vault proprietor NPC using the 'proprietor' variable
 * to let the system know which NPC it is.  You should also check for
 * the proprietors location and existance every reset.  You may also
 * want to provide some protection to prevent the NPC from being
 * killed too frequently.
 *
 * @author Ceres
 * @see /std/shops/vault_entrance.c
 * @see /handlers/vault_handler.c
 * @change Mostly rewritten by Sandoz, 2002.
 */

#include <dirs.h>;

inherit ROOM_OBJ;

#define DEFAULT_VISIT_COST 1000
#define DEFAULT_OPEN_COST  18000

#define MS(x,y)     (string)MONEY_H->money_value_string(x,y)
#define PTS         proprietor->the_short()

string vault_room, vault_name, *sizes, vault_exit;
int open_cost, visit_cost;
object proprietor, unused;

object make_vault_room( object player );

/** @ignore yes */
void create() {
    ::create();

    if( base_name(TO) == __FILE__[0..<3] )
        return;

    if( !vault_name || vault_name == "") {
        error("No vault name set.\n");
        return;
    }

    if( !pointerp( sizes ) ) {
        error("No vault sizes set.\n");
        return;
    }

    VAULT_H->add_vault( vault_name, sizes );

    add_help_file("vault");

} /* create() */

/**
 * This function is used to set the location used for calculation of which
 * currency the vault proprietor will accept.
 * @param location the name of the location
 */
void set_location( string location ) { add_property("place", location ); }

/**
 * This method returns the location of the vault.
 * This is used to for calculation of which currency the
 * vault operates with.
 * @return the location of the place, or default if none is set
 */
string query_location() {
    string str;

    if( ( str = query_property("place") ) && str != "")
        return str;

    return "default";

} /* query_location() */

/**
 * This method sets the cost of opening an account with no
 * upkeep costs.
 * @param cost the cost of a new account with no upkeep costs
 */
void set_open_cost( int i ) { open_cost = i; }

/**
 * This method returns the opening cost of a vault with
 * no upkeep costs.
 * @return the cost of opening a vault account with
 * no upkeep costs
 */
int query_open_cost() { return open_cost || DEFAULT_OPEN_COST; }

/**
 * This method is used to set the cost of a visit for vaults
 * with no upkeep costs.
 * @param cost the cost of a vist
 */
void set_visit_cost( int i ) { visit_cost = i; }

/**
 * This method returns the visiting cost of vaults with
 * no upkeep costs.
 * @return the visiting cost of vaults with no upkeep costs
 */
int query_visit_cost() { return visit_cost || DEFAULT_VISIT_COST; }

/**
 * This method is used to set the name of the vault.
 * This should be a unique name for this vault, and a proper
 * name at that, ie. "Sandoz's Secret Stash", not "sandoz_vault".
 * @param str the name of the vault
 * @see /handlers/vault_handler->query_vault_names()
 */
void set_vault_name( string str ) { vault_name = str; }

/**
 * This method returns the name of the vault.
 * @return the name of the vault
 */
string query_vault_name() { return vault_name; }

/**
 * This method sets the available vault sizes for this vault.
 * The usable sizes are set in the vault handler, a list of
 * which can be gotten with VAULT_H->query_vault_sizes().
 * NOTE : "small" vaults are the ones without upkeep costs,
 * but with a visiting fee.
 * @param str the available vault size(s) in this vault
 * @see /handlers/vault_handler->query_vault_sizes()
 * @example
 *     set_vault_sizes("small");
 *     set_vault_sizes( ({"small", "large"}) );
 */
void set_vault_sizes( mixed str ) {
    if( stringp(str) )
        sizes = ({ str });
    else if( pointerp(str) )
        sizes = str;
    else
        printf("The argument to set_vault_sizes() must be either a string or "
            "an array of strings.");
} /* set_vault_sizes() */

/**
 * This method can be used to assign a proprietor NPC to the
 * proprietor variable.
 * @param ob the proprietor NPC object
 */
void set_proprietor( object ob ) { proprietor = ob; }

/**
 * Used to set the exit from this vault entrance room into the vault itself.
 * The parameters are just like those of add_exit, except that you cannot
 * use a 'door' or 'gate' type exits here.
 * @param direction the direction of the exit
 * @param dest the destination of the vault exit
 * @param type the exit type as in add_exit
 * @see add_exit
 */
void set_vault_exit( string direc, mixed dest, string type ) {
    vault_room = dest;
    vault_exit = direc;
    add_exit( direc, (: make_vault_room :), type );
    modify_exit( direc, ({"function", "check_customer", "no follow", 1 }) );
} /* set_vault_exit() */

/** @ignore yes */
object make_vault_room( object thing ) {
    string owner;

    if( thing && unused && ( owner = unused->query_owner() ) &&
        owner == thing->query_name() )
        return unused;

    return 0;

} /* make_vault_room() */

/** @ignore yes */
int check_customer( string verb, object thing, string special ) {
    int cost;
    string name, place;

    if( !thing || thing == proprietor )
        return 0;

    if( !proprietor || ENV(proprietor) != TO ) {
        tell_object( thing, "The vault is closed since the proprietor is "
            "missing.\n");
        return notify_fail("");
    }

    if( proprietor->is_fighting(thing) ) {
        proprietor->do_command("peer "+file_name(thing) );
        proprietor->do_command("lsay No way am I letting you in!");
        return notify_fail("");
    }

    name = (string)thing->query_name();
    place = (string)query_location();

    if( !VAULT_H->query_account_in( vault_name, name ) ) {
        tell_room( TO, PTS+" intercepts "+(string)thing->one_short()+".\n");
        proprietor->do_command("'You have not rented any vault space here.");
        if( interactive(thing) ) {
            proprietor->do_command("'The prices of renting our vaults in "
                "different sizes are the following.");
            proprietor->do_command("'"+VAULT_H->query_pricing( vault_name,
                place, query_open_cost(), query_visit_cost() ) );
        }
        return notify_fail("");
    }

    cost = VAULT_H->query_visit_cost( vault_name, name, query_visit_cost() );

    if( cost && (int)thing->query_value_in( place ) < cost ) {
        tell_room( TO, PTS+" intercepts "+(string)thing->one_short()+".\n");
        proprietor->do_command("'It costs "+
            MS( query_visit_cost(), place )+" for you to use the "
            "facility!");
        return notify_fail("");
    }

    if( cost ) {
        thing->pay_money( MONEY_H->create_money_array( cost, place ), place );
        tell_object( thing, "You pay "+PTS+" who waves you through.\n");
        tell_room( TO, (string)thing->the_short()+" pays "+PTS+" who waves "+
            (string)thing->HIM+" through.\n", thing );
    } else {
        tell_room( TO, PTS+" waves "+thing->the_short()+" through.\n");
    }

    if( !unused = clone_object(vault_room) ) {
        tell_object( thing, "Something has gone wrong with your vault room, "
            "please contact a creator immediately.\n");
        return notify_fail("");
    }

    unused->setup_vault( vault_name, thing );
    VAULT_H->event_visit( vault_name, name );

    return 1;

} /* check_customer() */

/** @ignore yes */
int do_open_check() {
    if( !proprietor || ENV(proprietor) != TO ) {
        add_failed_mess("The vault is closed since the proprietor is "
                        "missing.\n");
        return 0;
    }

    add_succeeded_mess("");
    return 1;

} /* do_open_check() */

/** @ignore yes */
int do_rent( string size ) {
    if( !do_open_check() )
        return 0;

    tell_object( TP, "You enquire "+PTS+" about renting some vault space.\n");
    tell_room( TO, TP->the_short()+" $V$0=enquires,enquire$V$ about "
        "renting some vault space.\n", TP );

    return VAULT_H->do_rent( vault_name, size, proprietor, query_location(),
        query_open_cost(), query_visit_cost() );

} /* do_rent() */

/** @ignore yes */
int do_pay( int i ) {
    if( !do_open_check() )
        return 0;

    tell_object( TP, "You enquire "+PTS+" about paying rent for your vault "
        "space.\n");
    tell_room( TO, TP->the_short()+" $V$0=enquires,enquire$V$ about "
        "paying rent for "+verbalize("their vault accounts",
        TP->HIS+" vault account")+".\n", TP );

    return VAULT_H->do_pay( vault_name, i, proprietor, query_location() );

} /* do_pay() */

/** @ignore yes */
int do_switch( string size ) {
    if( !do_open_check() )
        return 0;

    tell_object( TP, "You enquire "+PTS+" about changing your vault size.\n");
    tell_room( TO, TP->the_short()+" $V$0=enquires,enquire$V$ about "
        "changing "+verbalize("their vault sizes",
        TP->HIS+" vault size")+".\n", TP );

    return VAULT_H->do_switch( vault_name, size, proprietor, query_location(),
        query_open_cost(), query_visit_cost() );

} /* do_switch() */

/** @ignore yes */
int do_check_rent() {
    if( !do_open_check() )
        return 0;

    tell_object( TP, "You enquire "+PTS+" about the rent for your vault "
        "space.\n");
    tell_room( TO, TP->the_short()+" $V$0=enquires,enquire$V$ about "
        "the rent for "+verbalize("their vault spaces",
        TP->HIS+" vault space")+".\n", TP );

    return VAULT_H->do_check_rent( vault_name, proprietor );

} /* do_check_rent() */

/** @ignore yes */
int do_check_prices() {
    TP->more_string("The pricing of our vaults is the following:\n"+
    VAULT_H->query_pricing( vault_name, query_location(),
        query_open_cost(), query_visit_cost(), 1 ), "vault prices");
    return 1;
} /* do_check_prices() */

/** @ignore yes */
void init() {
    string str;

    ::init();

    str = "{"+implode( sizes, "|")+"}";

    add_command("switch", "vault size to "+str, (: do_switch($4[0]) :) );
    add_command("change", "vault size to "+str, (: do_switch($4[0]) :) );
    add_command("rent", "[a] {"+implode( sizes, "|")+"} vault",
        (: do_rent($4[0]) :) );
    add_command("pay", "rent for <number> months", (: do_pay($4[0]) :) );
    add_command("check", "[my] rent", (: do_check_rent() :) );
    add_command("check", "prices", (: do_check_prices() :) );

} /* init() */

/** @ignore yes */
void player_quitting( object who, object env ) {
} /* player_quitting() */

/** @ignore yes */
void do_clean( object room ) {
    object *inv, *liv, ob;
    string prop;

    if( !room )
        return;

    if( sizeof( inv = INV(room) ) ) {
        prop = ( proprietor && ENV(proprietor) == TO ?
                 proprietor->the_short() : "a vault assistant");

        liv = filter( inv, (: living($1) :) );

        if( sizeof(liv) ) {
            inv -= liv;
            foreach( ob in liv ) {
                tell_object( ob, prop+" drags you out of the vault.\n");
                ob->move_with_look( TO,
                    prop+" drags $N out from one of the vaults.",
                    prop+" drags $N out of the vault.");
            }
        }

        if( sizeof(inv) ) {
            inv->move(TO);
            tell_room( TO, prop+" drags "+query_multiple_short(inv)+" out "
                "from one of the vaults.\n");
        }

        if( sizeof( INV(room) ) ) {
            tell_creator("sandoz", "Unmovable stuff in vault : %s",
                file_name(room) );
            return;
        }
    }

    room->dest_me();

} /* do_clean() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({
        ({"vault name", vault_name }),
        ({"vault sizes", implode( sizes, ", ") }),
        ({"open cost", query_open_cost() }),
        ({"visit cost", query_visit_cost() }),
    });
} /* stats() */
