/**
 * This is the inherit for a money changing office.
 * It will allow players to change their money into
 * different denominations.
 * If you want to use the built in NPC support, please call
 * set_use_clerk() on the room, and clone a clerk NPC into
 * the room, using the built-in 'clerk' variable.
 * @author Avelan
 * @changed Changed to work with different currency areas
 * - Sandoz, April 2003.
 */

#define MSTR(x)   (string)MONEY_H->money_string(x)

nosave inherit ROOM_OBJ;

int profit;

nosave mapping types;
nosave string place, savefile;
nosave int fee, use_clerk;
nosave object clerk;

/**
 * This method returns the coin types we change to.
 * @return the coin types we change to
 */
mapping query_types() { return types; }

/**
 * This method sets the place that is used when determining
 * if a currency is valid here.
 * @param str the name of the place to use
 * @see query_place()
 */
void set_place( string str ) { place = str; }

/**
 * This method returns the place that is used when determining
 * whether or not a currency is valid here.
 * @return the place
 * @see set_place()
 */
string query_place() { return place || "default"; }

/** @ignore yes */
void create() {
    int i;
    mixed arr;

    ::create();

    if( __FILE__[0..<3] == base_name(TO) )
        return;

    if( savefile && file_exists( savefile ) )
        unguarded( (: restore_object, savefile :) );

    add_help_file("money_changer");

    types = ([ ]);
    arr = MONEY_H->query_values_in( query_place() );

    for( i = 0; i < sizeof(arr); i += 2 )
        types[ MONEY_H->query_main_plural_for( arr[ i ] ) ] = arr[ i ];

} /* create() */

/**
 * This method tells us to use the built-in NPC code.
 */
void set_use_clerk() { use_clerk = 1; }

/**
 * This method queries whether or not we are using the built-in NPC code.
 * @return 1 if we are using the built-in NPC code
 */
int query_use_clerk() { return use_clerk; }

/**
 * This method sets the fee percentage used when changing money.
 * @param i the fee to set
 * @see query_fee()
 */
void set_fee( int i ) { fee = i; }

/**
* This method returns the fee percentage used when changing money.
* @see set_fee()
*/
int query_fee() { return fee; }

/**
 * This method sets the savefile for the room, which is then
 * used to store the profit that has been made by the office.
 * If you don't want to keep track of profit made, then
 * leave it unset.
 * @see query_save_file()
 */
void set_save_file( string file ) {
    if( !file || file == "")
        return;

    if( file[<2..] != ".o")
        file += ".o";

    savefile = file;

} /* set_save_file() */

/**
 * This method returns the savefile for the room.
 * @see set_save_file()
 */
string query_save_file() { return savefile; }

/** @ignore yes */
void clerk_say( string str ) {
    if( use_clerk && clerk )
        return clerk->do_command("whisper "+TP->query_name()+" "+str );

    tell_room( TO, "The clerk whispers something to "+TP->the_short()+"\n");

} /* clerk_say() */

/** @ignore yes */
int do_change( object *money, string str ) {
    object cont, *tender, legal, illegal;
    int val, new_val, i;
    string type;
    mixed arr;

    if( use_clerk && ( !clerk || ENV(clerk) != TO ) ) {
        add_failed_mess("There is no-one here to see to your needs.\n");
        return 0;
    }

    if( sizeof( filter( money, (: !$1->query_property("money") :) ) ) ) {
        add_failed_mess("You can only $V money here.\n");
        return 0;
    }

    if( undefinedp( type = types[str] ) ) {
        add_failed_mess("Sorry, you cannot change money into "+str+" here, "
            "please 'list types' for the available denominations.\n");
        return 0;
    }

    // Let us merge the different money objects.
    cont = clone_object("/std/container");
    money->move(cont);
    money = INV(cont);

    if( sizeof( money ) != 1 ) {
        money->move(TP);
        cont->dest_me();
        add_failed_mess("Something is broken, please contact a creator.\n");
        return 0;
    }

    tell_room( TO, "You ask "+( use_clerk ? clerk->the_short() :
        "the clerk")+" about changing "+
        query_multiple_short( money[0]->half_short(1) )+" into "+str+".\n");
    tell_room( TO, TP->the_short()+" $V$0=asks,ask$V$ "+( use_clerk ?
        clerk->the_short() : "the clerk")+" about changing some money "
        "into "+str+".\n", TP );
    tender = MONEY_H->filter_legal_tender( money[0], "calarien");
    legal = tender[ 0 ];
    illegal = tender[ 1 ];
    cont->dest_me();

    if( illegal ) {
        clerk_say("I'm afraid "+MSTR( illegal->query_money_array() )+" isn't "
            "changeable here.");
        illegal->move(TP);
    }

    if( !legal )
        return 1;

    val = legal->query_value_in( query_place() );
    if( query_place() != "calarien" )
        val += legal->query_value_in("calarien");

    arr = MONEY_H->query_values_in( query_place() );
    new_val = arr[ member_array( type, arr ) + 1 ];

    if( new_val > val ) {
        clerk_say("You cannot change "+MSTR(
            legal->query_money_array() )+" into "+str+" coins, "
            "because "+str+" have a higher value.");
        legal->move(TP);
        return 1;
    }

    i = val;

//    if( TP->query_value_in( ) < i ) {
//        clerk_say("You don't have enough money to pay the fee, I'm afraid.");
//        legal->move(TP);
//        return 1;
//    }

    TP->pay_money( MONEY_H->create_money_array( i, query_place() ),
        query_place() );

    profit += i;

    // Calculate the number of new coins.
    i = val / new_val;

    // That's our change.
    illegal = MONEY_H->make_new_amount( val - i * new_val, query_place() );
    if( illegal )
        arr = illegal->query_money_array();
    else
        arr = 0;

    if( !illegal )
        illegal = clone_object(MONEY_OBJ);

    illegal->adjust_money( i, type );

    if( i == 1 )
        str = "one "+MONEY_H->query_short_for(type);
    else
        str = sprintf("%i %s", i, str );

    tell_object( TP, "You give "+MSTR( legal->query_money_array() )+
        " to the clerk and receive "+
        str+( sizeof(arr) ? " and the change of "+
        MONEY_H->money_string(arr) : "")+" in return.\n");

    // Give them the coins.
    if( illegal->move(TP) ) {
        illegal->move(TO);
        tell_object( TP, "Unfortunately you are too burdened to accept all "
            "that, so the clerk puts it on the ground for you.\n");
    }

    legal->move("/room/rubbish");

    if( savefile )
        unguarded( (: save_object, savefile :) );

    clerk_say("Thank you for your custom.");
    return 1;

} /* do_change() */

/** @ignore yes */
int do_list() {
    if( use_clerk && ( !clerk || ENV(clerk) != TO ) ) {
        add_failed_mess("There is no-one here to see to your needs.\n");
        return 0;
    }

    clerk_say("We currently change money into "+
        query_multiple_short( keys(types) )+".");

    return 1;

} /* do_list() */

/** @ignore yes */
void init() {
    ::init();

    add_command("change",
        "<indirect:object:me'money'> into <string'type'>",
        (: do_change( $1, $4[1] ) :) );
    add_command("list", "types");

} /* init() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({
        ({"profit made", profit }),
        ({"fee", query_fee() }),
        ({"place", query_place() }),
        ({"using clerk", query_use_clerk() }),
    });
} /* stats() */
