/**
 * A printing workshop where players can create their own leaflets.
 * <P>
 * This can be divided up into two rooms, an office (where you pay for the
 * printing to be done) and a collection room (where you do the collection).
 * To enable this behaviour the collection room needs to know about the
 * office.  The office stores all the information, so you call
 * set_office_room(room_path); on the collection room and
 * set_no_collection() on the office.
 * If you want to use a shopkeeper, please use the built-in shopkeeper
 * variable, also don't forget to call set_use_shopkeeper(), which will
 * tell the room to work in shopkeeper mode.
 * @author Sandoz, based on Pinkfish's print shop.
 * @see set_office_room()
 * @see set_no_collection()
 */

#include <language.h>
#include <leaflet.h>
#include <time.h>

#define CM(x)                   TP->convert_message(x)
#define QMS(x)                  CM( query_multiple_short( x, "the") )
#define MSTR(x)                 MONEY_H->money_value_string( x, place )

/** This is the amount of chars that will fit in the largest leaflet. */
#define MAX_CHARS               2000
/** This is the maximum number of copies we can make at a time. */
#define MAX_COPIES              50
/** This is the cost of claiming copyright on a leaflet. */
#define COPYRIGHT_COST          6000
/** This is the default cost per 10 letters. */
#define DEFAULT_LETTER_COST     4
/** This is the base cost of printing a leaflet. */
#define DEFAULT_MATERIAL_COST   50
/** This is the base cost of an order. */
#define DEFAULT_COST_PER_ORDER  2000
/** This is the minimum time it takes to complete an order. */
#define DEFAULT_TIME_TO_COLLECT (2*60*60)
/** This is the minimum amount of time it takes to print one copy. */
#define DEFAULT_TIME_PER_COPY   120
/** This is the time it takes to print 10 letters. */
#define DEFAULT_TIME_PER_LETTER 60

nosave inherit ROOM_OBJ;

nosave object shopkeeper;
nosave function open_func;
nosave mapping translation_costs, discount;
nosave int keeper, letter_cost, material_cost, time_to_collect, time_per_copy;
nosave int no_collect, cur_num, cost_per_order, id;
nosave string default_language, office_room;

/** @ignore yes */
void create() {
    string shop_name, leaf_long;

    translation_costs = ([ ]);
    discount = ([ 1  : 100,
                  5   : 95,
                  10  : 90,
                  20  : 85,
                  50  : 80,
                  100 : 75,
                  ]);

    ::create();
    add_help_file("leaflet_shop");

    if( __FILE__[0..<3] == base_name(TO) )
        return;

    shop_name = query_property("shop_name");
    leaf_long = query_property("leaf_long");

    remove_property("shop_name");
    remove_property("leaf_long");

    if( !shop_name )
        error("No shop name specified.\n");

    if( !leaf_long )
        error("No leaflet long description specified.\n");

    id = LEAFLET_H->add_printer( shop_name, leaf_long );

} /* create() */

/**
 * This method returns the id of the shop.
 * @return our id
 */
int query_print_shop_id() { return id; }

/**
 * This method queries whether or not we are a leaflet shop.
 * This is used by the leaflet handler.
 * @return always return 1
 */
int query_leaflet_shop() { return 1; }

/**
 * This method returns the discount schedule.  There is a default
 * discount schedule setup for printing shops. The discount schedule
 * tells how much of a discount you get for certain numbers of a run.
 * It returns a mapping of the form:
 * <pre>
 * ([ num1 : dis1, num2 : dis2, ... ])
 * </pre>
 * Where each num is the number of items at and above which the discount
 * dis applies.
 * @return the discount schedule
 * @see set_discount_schedule()
 */
mapping query_discount_schedule() { return discount; }

/**
 * Sets the discount schedule for this object.  See the
 * query_discount_schedule help for the format of the mapping.
 * @param value the new discount schedule.
 * @see query_discount_schedule()
 */
void set_discount_schedule( mapping map ) { discount = map; }

/**
 * Sets the office room to which this collection room is associated.  If
 * this is set on a room, you cannot setup print jobs in the room and this
 * room specified here is queried for the information about the jobs to
 * collect.
 * @param room the new office room
 * @see query_office_room()
 * @see set_no_collection()
 */
void set_office_room( string room ) { office_room = room; }

/**
 * Returns the current office room associated with this room.
 * @return the current office room
 * @see set_office_room()
 * @see query_no_collection()
 */
string query_office_room() { return office_room; }

/**
 * This sets the no collection flag.  It makes it so this room cannot be
 * used for collections.  If this is set then there should be an associated
 * collection room used to pick up the books from.  This room should
 * use the set_office_room() function to set the office room to use
 * for the collections.
 * @param collect the new value of no_collections flag
 * @see query_no_collection()
 * @see set_office_room()
 */
void set_no_collection( int collect ) { no_collect = collect; }

/**
 * This returns the no collection flag used by the room.
 * @return the no collection flag
 * @see set_no_collection()
 * @see query_office_room()
 */
int query_no_collection() { return no_collect; }

/**
 * This method sets the function to evaluate to check to see if the shop
 * is open or not.  This should set any error messages which are needed
 * and return 1 for open status or 0 for closed.
 * @param func the open function
 * @see query_open_func()
 * @see do_print()
 * @see do_print_copies()
 * @see do_collect()
 */
void set_open_func( function func ) { open_func = func; }

/**
 * This method returns the function which is used to check for the
 * open status of the shop.
 * @see set_open_func()
 * @return the current open function
 */
function query_open_func() { return open_func; }

/**
 * This method will add the cost to translate from one language to
 * another.  The default is translating to the common language and
 * this cost will be used in both directions.  ie: the cost of translating
 * from djelian to common is the same as translating from common to
 * djelian.  If translating between two languages that are not the
 * default language, say the default is common and we want to translate
 * from djelian to wombat, then the cost from djelian->common and then
 * common->wombat will be used.
 * @param lang the language to set the translation cost of
 * @param cost the cost is in percentage, 200 means twice the price
 * @see set_default_language()
 * @see remove_translation_cost()
 * @see query_all_translation_costs()
 */
void add_translation_cost( string lang, int cost ) {
    translation_costs[lang] = cost;
} /* add_translation_cost() */

/**
 * This method will remove the translation cost for a specific language.
 * @param lang the language to remove the translation cost of
 * @see set_default_language()
 * @see add_translation_cost()
 * @see query_all_translation_costs()
 */
void remove_translation_cost( string lang ) {
    map_delete( translation_costs, lang );
} /* remove_translation_cost() */

/**
 * This method will return a mapping of all the translation costs.
 * The keys are the languages and the values are the costs in terms
 * of percentages.
 * @return the current translation cost mapping
 * @see add_translation_cost()
 * @see remove_translation_cost()
 */
mapping query_all_translantion_costs() { return translation_costs; }

/**
 * This method sets the name of the print shop which will be used on
 * leaflets when they are printed.
 * @param name the name of the print shop
 */
void set_print_shop_name( string name ) { add_property("shop_name", name ); }

/**
 * This method will return the current name of the print shop.
 * @return the name of the print shop
 */
string query_print_shop_name() { return LEAFLET_H->query_printer_name(id); }

/**
 * This method sets the long description for leaflets printed here.
 * If none is set, a default description will be used.
 * Note: $size$ and $shop$ expansion will be done on the string,
 * so if you want the long desc reflect the size of the leaflet,
 * then $size$ will be expanded to "small", "medium sized", "large",
 * "huge", etc., based on the number of characters in the leaflet, and
 * $shop$ will be expanded to the shop name.
 * @param str the long description for leaflets
 * @see query_leaflet_long()
 * @example set_leaflet_long("This $size$ leaflet looks like it has been "
 *              "churned out from one of the cities' printing presses.");
 */
void set_leaflet_long( string str ) { add_property("leaf_long", str ); }

/**
 * This method will return the long description of leaflets
 * printed here.
 * @return the long description of leaflets printed here
 * @see set_leaflet_long()
 */
string query_leaflet_long() { return LEAFLET_H->query_leaflet_long(id); }

/**
 * This method returns the maximum amount of characters
 * printable on a leaflet.
 * @return the maximum amount of characters printable on a leaflet
 */
int query_max_chars() { return MAX_CHARS; }

/**
 * This method sets the cost per order.
 * @param cost the cost per order
 */
int set_cost_per_order( int cost ) {
    if( cost > 0 )
        cost_per_order = cost;
} /* set_cost_per_order() */

/**
 * This method returns the cost per order.
 * @return the cost per order
 */
int query_cost_per_order() {
    return cost_per_order || DEFAULT_COST_PER_ORDER;
} /* query_cost_per_order() */

/**
 * This method tells the room to work in shopkeeper mode.
 * In shopkeeper mode all messages will be spoken through
 * the shopkeeper.
 */
void set_use_shopkeeper() { keeper = 1; }

/**
 * This method queries whether or not we are working in
 * shopkeeper mode.
 * @return 1 if we are in shopkeeper mode, 0 if not
 */
int query_use_shopkeeper() { return keeper; }

/**
 * This queries the cost per 10 letters.
 * @return the cost per letter
 * @see set_letter_cost()
 */
int query_letter_cost() { return letter_cost || DEFAULT_LETTER_COST; }

/**
 * This sets the cost per 10 letters.
 * @param value the new cost per letter
 * @see query_letter_cost()
 */
void set_letter_cost( int value ) { letter_cost = value; }

/**
 * This queries the material cost per leaflet.
 * This will be added to the cost of printing each leaflet.
 * @return the material cost per leaflet
 * @see set_material_cost()
 */
int query_material_cost() { return material_cost || DEFAULT_MATERIAL_COST; }

/**
 * This sets the material cost per leaflet.
 * This will be added to the cost of printing each leaflet.
 * @param value the new material cost per leaflet
 * @see query_material cost_cost()
 */
void set_material_cost( int value ) { material_cost = value; }

/**
 * This queries the minimum time per order.
 * @return the minimum time per order
 * @see set_time_per_order()
 */
int query_time_to_collect() {
    return time_to_collect || DEFAULT_TIME_TO_COLLECT;
} /* query_time_to_collect() */

/**
 * This sets the minimum time per order.
 * @param time the time per order
 * @see query_time_per_order()
 */
void set_time_to_collect( int time ) { time_to_collect = time; }

/**
 * This queries the time it takes to print one copy.
 * @return the minimum time per copy
 * @see set_time_per_copy()
 */
int query_time_per_copy() { return time_per_copy = DEFAULT_TIME_PER_COPY; }

/**
 * This sets the minimum time it takes to print a copy.
 * @param time the time per copy
 * @see query_time_per_copy()
 */
void set_time_per_copy( int time ) { time_per_copy = time; }

/**
 * This method sets the default language to use in the print shop.
 * Anything written in this language will cost nothing extra to
 * duplicate.
 * @param lang the new default language
 * @see query_default_language()
 * @see add_translation_cost()
 * @see check_for_untranslatable_language()
 */
void set_default_language( string lang ) { default_language = lang; }

/**
 * This method will return the current default language for the
 * print shop.
 * @return the current default language
 * @see set_default_language()
 * @see add_translation_cost()
 * @see check_for_untranslatable_language()
 */
string query_default_language() { return default_language || "common"; }

/** @ignore yes */
private int do_open_check() {
    if( keeper && ( !shopkeeper || ENV(shopkeeper) != TO ) ) {
        add_failed_mess("There is no-one here to serve you.\n");
        return 0;
    }

    return !open_func || evaluate( open_func );

} /* do_open_check() */

/** @ignore yes */
void notify_player( string mess ) {
    if( keeper && shopkeeper )
        return shopkeeper->do_command("sayto "+TP->query_name()+" "+mess );

    tell_object( TP, mess+"\n");

} /* notify_player() */

/** @ignore yes */
varargs int add_failed( string mess ) {
    if( keeper && shopkeeper ) {
        shopkeeper->do_command("sayto "+TP->query_name()+" "+mess );
        notify_fail("");
        return 1;
    }

    add_failed_mess( mess+"\n");
    return 0;

} /* add_failed() */

/**
 * Figure out how much this object will cost to make.
 * @param ob the object to price
 * @param num the number of them to print
 * @return the cost of the item
 */
int price_object( object ob, int num, string to_language ) {
    int i, cost, key, multiplier;
    mixed stuff;

    if( num < 1 || !ob || !ob->query_leaflet() )
        return 0;

    if( !sizeof( stuff = ob->query_text() ) )
        return 0;

    i = strlen( stuff[READ_MESS] );

    if( stuff[READ_LANG] != query_default_language() ) {
        cost = translation_costs[ stuff[READ_LANG] ];
        if( !cost )
            cost = 1000;
        i = i * cost / 100;
    }

    if( to_language != query_default_language() ) {
        cost = translation_costs[to_language];
        if( !cost )
            cost = 1000;
        i = i * cost / 100;
    }

    cost = ob->query_value() + i / 10 * query_letter_cost();

    foreach( key in sort_array( keys(discount), 1 ) ) {
        if( num >= key )
            multiplier = discount[key];
        else
            break;
    }

    cost += query_material_cost();

    i = ( cost * num * multiplier ) / 100;

    return query_cost_per_order() + i;

} /* price_object() */

/** @ignore yes */
protected void check_copyright( string str, object *obs, int value ) {
    object ob;
    string place;

    if( !strlen(str) || ( str[0] != 'y' && str[0] != 'Y' ) )
        return notify_player("As you wish.  See you again soon.");

    if( !do_open_check() ) {
        write("The shop seems to have been closed in the meantime, sorry.\n");
        return;
    }

    place = query_property("place") || "default";
    TP->pay_money( MONEY_H->create_money_array( value, place ), place );

    foreach( ob in obs )
        LEAFLET_H->copyright_leaflet( ob->query_leaflet_id(),
            TP->query_name() );

    notify_player("Your copyright request on "+QMS(obs)+" has been "
        "successful.");

} /* check_copyright() */

/**
 * This method sets up the copyright for the objects.  This will check for
 * existing copyright status and then check with the book handler to see
 * if it still fits the criteria.  If it does not then it will add itself
 * as a new book.
 * @param obs the objects to claim copyright on
 */
protected int do_claim( object *obs ) {
    object *bad;
    string place, ret;
    int value, player_money, ident, new_id;
    mapping uniques;

    if( !do_open_check() )
        return 0;

    if( keeper && shopkeeper )
        tell_object( TP, "You enquire about claiming copyright on "+
            QMS(obs)+".\n");

    tell_room( TO, TP->the_short()+" "+verbalize("enquire")+" about "
        "claiming copyright on "+QMS(obs)+".\n", TP );

    bad = filter( obs, (: !$1->query_leaflet() :) );
    if( !sizeof( obs -= bad ) )
        return add_failed( query_multiple_short( bad, "the")+" "+
            verbalize("do")+" not appear to be "+( sizeof(bad) > 1 ?
            "leaflets" : "a leaflet")+".");

    bad = filter( obs, (: $1->query_copyright() :) );
    if( !sizeof( obs -= bad ) )
        return add_failed("I'm afraid "+query_multiple_short( bad, "the")+" "+
            verbalize("have")+" already been copyrighted.");

    uniques = unique_mapping( obs, (: $1->query_leaflet_id() :) );

    foreach( player_money, obs in uniques ) {
        if( sizeof(obs) > 1 )
            ident = 1;
        uniques[player_money] = obs[0];
        if( ( new_id = LEAFLET_H->query_copyright_protected(
            obs[0]->query_text()[READ_MESS] ) ) &&
            LEAFLET_H->query_copyright(new_id) != TP->query_name() ) {
            bad += ({ obs[0] });
            map_delete( uniques, player_money );
            continue;
        }
        value += COPYRIGHT_COST;
    }

    ret = "";

    if( ident )
        ret += "You cannot claim copyright on several identical leaflets";

    if( sizeof(bad) ) {
        if( sizeof(ret) )
            ret += " and ";
        ret += "I'm afraid "+QMS(bad)+" "+verbalize("have")+" already been "
            "copyrighted";
    }

    if( sizeof(ret) )
        notify_player( ret+".");

    place = query_property("place") || "default";
    player_money = (int)TP->query_value_in( place );

    if( place != "default")
        player_money += (int)TP->query_value_in("default");

    obs = values(uniques);
    ret = "It would cost you "+MSTR(value)+" to claim copyright on "+QMS(obs);

    if( player_money >= value ) {
        notify_player( ret+".");
        write("Do you wish to continue [y|n]? ");
        input_to( (: check_copyright :), 0, obs, value );
    } else {
        notify_player( ret+", but you do not have enough money.");
    }

    add_succeeded_mess("");
    return 1;

} /* do_claim() */

/**
 * This adds an object into the current set to be collected,
 * and saves the auto load info for the object.
 * @param name the name of the person adding the run
 * @param ob the object being added
 * @param num the number to be printed
 * @param lang the language to print it in
 * @return the print data class
 * @see do_collect()
 */
private class print_data add_print_run( string name, object ob, int num,
                                        string lang ) {
    class print_data data;
    int new_leaflet;

    if( new_leaflet = ob->query_property("new_leaflet") )
        ob->remove_property("new_leaflet");

    data = new( class print_data,
           id     : id,
           time   : time() + sizeof(ob->query_text()[READ_MESS]) / 10 *
                    DEFAULT_TIME_PER_LETTER + query_time_to_collect() +
                    query_time_per_copy() * num,
           number : num,
           lang   : lang,
           file   : base_name(ob),
           save   : ({ ob->query_static_auto_load(),
                       ob->query_dynamic_auto_load() }),
           new_leaflet : new_leaflet );

    if( new_leaflet )
        ob->dest_me();

    if( !LEAFLET_H->add_print_run( name, data ) )
        return 0;

    return data;

} /* add_print_run() */

/**
 * This is called when the player comes back to collect
 * the leaflet they have printed.
 * @return 1 on success, 0 on failure
 */
protected int do_collect() {
    object ob, office, *obs;
    int printed, i, failed;
    class print_data info, *collectables;
    mixed auto_load;
    mapping move;
    string ret;

    if( !do_open_check() )
        return 0;

    office = ( office_room ? load_object(office_room) : TO );
    collectables = LEAFLET_H->query_print_run( id, TP->query_name() );

    if( keeper && shopkeeper )
        tell_object( TP, "You enquire about collecting some leaflets.\n");

    tell_room( TO, TP->the_short()+" "+verbalize("enquire")+" about "
        "collecting "+verbalize("their", TP->HIS )+" leaflets.\n", TP );

    if( !collectables )
        return add_failed("Sorry, there are no leaflets for you to collect.");

    foreach( info in collectables ) {
        if( time() < info->time ) {
            if( !failed || info->time < failed )
                failed = info->time;
            continue;
        }

        auto_load = info->save;
        ob = clone_object(info->file);

        if( auto_load[0] )
            ob->init_static_arg( auto_load[0] );
        if( auto_load[1] )
            ob->init_dynamic_arg( auto_load[1] );

        auto_load = ob->query_text();

        if( auto_load[READ_LANG] != info->lang ) {
            auto_load[READ_LANG] = info->lang;
            i = LEAFLET_H->new_leaflet( ob->query_author(), auto_load );
            ob->set_leaflet_id(i);
        }

        auto_load = ({ ob->query_static_auto_load(),
                       ob->query_dynamic_auto_load() });

        ob->dest_me(); // Kill the sample.
        LEAFLET_H->remove_print_run( id, TP->query_name(), info );

        obs = ({ });

        for( i = 0; i < info->number; i++ ) {
            ob = clone_object(info->file);
            ob->set_player(TP);
            if( auto_load[0] )
                ob->init_static_arg( auto_load[0] );
            if( auto_load[1] )
                ob->init_dynamic_arg( auto_load[1] );
            obs += ({ ob });
        }

        move = unique_mapping( obs, (: $1->move(TP) != 0 :) );
        ret = "";

        if( sizeof(move[0]) ) {
            if( shopkeeper ) {
                ret = shopkeeper->the_short()+" gives "+
                    query_num(sizeof(move[0]))+" "+( sizeof(move[0]) == 1 ?
                    "copy" : "copies")+" of "+move[0][0]->the_short()+" to "+
                    TP->the_short();
            } else {
                ret = "You receive your "+( sizeof(move[0]) > 1 ?
                    query_num(sizeof(move[0]))+" copies" : "copy")+" of "+
                    move[0][0]->the_short();
            }
        }

        if( sizeof(move[1]) ) {
            move[1]->move(TO);
            if( shopkeeper ) {
                if( !sizeof( move[0] ) ) {
                    ret = shopkeeper->the_short()+" places "+
                        TP->poss_short()+" "+( sizeof(move[1]) == 1 ? "copy" :
                        query_num(sizeof(move[1]))+" copies")+" of "+
                        move[1][0]->the_short()+" on the floor.\n";
                } else {
                    ret += " and places the rest on the floor.\n";
                }
            } else {
                if( !sizeof( move[0] ) ) {
                    ret = "Your "+( info->number > 1 ?
                        query_num(info->number)+" copies" : "copy")+" of "+
                        ob->the_short()+" have been placed on the floor.\n";
                } else {
                    ret += " and the rest of your copies are placed on the "
                        "floor.\n";
                }
            }
        } else {
            ret += ".\n";
        }

        if( shopkeeper )
            tell_room( TO, ret );
        else
            write( ret );
        printed++;
    }

    if( !printed )
        return add_failed("Sorry, you need to wait until "+
            mudtime(failed)+" to collect your work.");

    return printed;

} /* do_collect() */

/**
 * Checks to see if the leaflet is in a language we cannot translate.
 * @param ob the leaflet to check for languages
 * @return 1 if there is a language we cannot deal with
 * @see set_default_language()
 * @see add_translation_cost()
 */
int check_for_untranslatable_language( object ob ) {
    mixed stuff;
    string lang;

    if( !sizeof( stuff = ob->query_text() ) )
        return 0;

    lang = stuff[READ_LANG];

    return ( lang != query_default_language() && !translation_costs[lang] );

} /* check_for_untranslatable_language() */

/** @ignore yes */
object find_leaflets( object *obs ) {
    int tmp;
    object *bad;
    mapping failed;
    function f;

    f = function( mapping map ) {
            string ret;

            if( sizeof( map["not"] ) ) {
                ret = "You cannot print copies of "+query_multiple_short(
                    map["not"], "the")+" because "+verbalize("they are not "
                    "leaflets", "it is not a leaflet")+".";
                map_delete( map, "not");
                if( sizeof(map) )
                    notify_player(ret);
            }

            if( sizeof( map["copy"] ) ) {
                ret = query_multiple_short( map["copy"], "the")+" "+verbalize(
                    "have")+" been copyrighted by "+query_multiple_short( map(
                    map["copy"], (: CAP($1->query_copyright()) :) ) )+" and "
                    "cannot be copied.";
                map_delete( map, "copy");
                if( sizeof(map) )
                    notify_player(ret);
            }

            if( sizeof( map["text"] ) ) {
                ret = query_multiple_short( map["text"], "the")+" "+verbalize(
                    "have")+" no text on "+verbalize("them", "it")+"!";
                map_delete( map, "text");
                if( sizeof(map) )
                    notify_player(ret);
            }

            if( map["obs"] )
                ret = "You can only print copies of one leaflet at a time, "
                      "please be more specific.";

            add_failed(ret);
            return 0;

        };

    failed = ([ ]);

    if( sizeof( bad = filter( obs, (: !$1->query_leaflet() :) ) ) )
        failed["not"] = bad;
    if( !sizeof( obs -= bad ) )
        return evaluate( f, failed );

    if( sizeof( bad = filter( obs, (: $1->query_copyright() &&
        $1->query_copyright() != $2 :), TP->query_name() ) ) )
        failed["copy"] = bad;
    if( !sizeof( obs -= bad ) )
        return evaluate( f, failed );

    if( sizeof( bad = filter( obs, (: !sizeof( $1->query_text() ) :) ) ) )
        failed["text"] = bad;
    if( !sizeof( obs -= bad ) )
        return evaluate( f, failed );

    if( sizeof(obs) > 1 ) {
        failed["obs"] = 1;
        return evaluate( f, failed );
    }

    if( obs[0]->query_copyright() == TP->query_name() )
        return obs[0];

    if( ( tmp = LEAFLET_H->query_copyright_protected(
        obs[0]->query_text()[READ_MESS] ) ) &&
        LEAFLET_H->query_copyright(tmp) != TP->query_name() ) {
        add_failed("The text on "+obs[0]->the_short()+" looks awfully "
            "similar to materials copyrighted by someone other than you.  "
            "I'm afraid we cannot copy that.");
        return 0;
    }

    return obs[0];

} /* find_leaflets() */

/**
 * This method will print out the pricing schedule for the object.
 * @param ob the object to get a pricing schedule of.
 */
void print_pricing( object ob, string language ) {
    int num, cols;
    string place, ret, *bits;
    mixed stuff;
    function asterisk;

    place = query_property("place") || "default";

    asterisk = function( string *bits, int cols ) {
                   string tmp, ret = "";

                   foreach( tmp in bits )
                       ret += sprintf(" *%s", indent( tmp, 3, cols )[2..] );

                   return ret;

               };

    bits = ({ });
    cols = sizeof(ob->query_text()[READ_MESS]) / 10 * DEFAULT_TIME_PER_LETTER;

    foreach( num in sort_array( keys(discount), 1 ) ) {
        if( num <= MAX_COPIES )
            bits += ({ sprintf("%i for %s and will be ready at %s.\n",
                num, MSTR( price_object( ob, num, language ) ), mudtime(
                query_time_to_collect() + query_time_per_copy() * num + cols +
                time() ) ) });
    }

    cols = (int)TP->query_cols();

    if( sizeof(translation_costs) ) {
        ret = "You can translate a book to or from "+query_multiple_short(
            keys(translation_costs)+({ query_default_language() }) )+", the "
            "cost of doing the translation is defined in the following "
            "table.";

        if( sizeof(translation_costs) > 2 )
            ret += "  Translating between two of these languages is also "
                "possible, but it will cost the amount to translate from the "
                "language to "+query_default_language()+" and back again.";

        ret += "\n";
        bits += ({ ret });
        ret = evaluate( asterisk, bits, cols );

        asterisk = function( int i, int j ) {
                    if( i < j )
                        return 1;
                    if( i > j )
                        return -1;
                    return 0;
                };

        num = sort_array( map( keys(translation_costs), (: sizeof($1) :) ),
            asterisk )[0] + 2;

        if( num < 20 )
            num = 20;

        foreach( language, stuff in translation_costs )
            ret += sprintf("%-=*s %'.'-=*s %d%%\n", 6, "", num, language,
                stuff );
        write("The cost of copying different numbers of "+
            ob->the_short()+" is:\n"+ret );
        return;
    }

    write("The cost of copying different numbers of "+
        ob->the_short()+" is:\n"+evaluate( asterisk, bits, cols ) );

} /* print_pricing() */

/**
 * This method will be called by the print add_command.
 * It will print out the pricing schedule and how long
 * the item will take to make.
 * @param obs the objects which have been matched
 * @param language the language to print the book in
 * @return 1 on success, 0 on failure
 * @see do_print_copies()
 */
int do_print( object *obs, string language ) {
    object ob;

    if( !do_open_check() )
        return 0;

    tell_room( TO, TP->the_short()+" "+verbalize("enquire")+" about the cost "
        "of printing copies of "+QMS(obs)+".\n", TP );

    if( !ob = find_leaflets(obs) )
        return 0;

    print_pricing( ob, language );
    write("Please use 'print <number> of <leaflet>' to print copies.\n");

    add_succeeded_mess("");
    return 1;

} /* do_print() */

/**
 * This method does the actual print run.  It creates all the objects
 * and makes the player pay for them all.
 * @param ob the object to print
 * @param number the number to print
 * @param cost the cost of the object
 * @param the language to translate things to
 * @see do_print_copies()
 */
void do_print_run( object ob, int number, int cost, string language ) {
    string place;
    int player_money;
    class print_data data;

    place = query_property("place") || "default";

    player_money = (int)TP->query_value_in( place );
    if( place != "default" )
        player_money += (int)TP->query_value_in("default");

    if( player_money < cost ) {
        if( ob->query_property("new_leaflet") ) {
            LEAFLET_H->delete_leaflet( ob->query_leaflet_id() );
            ob->dest_me();
        }
        return notify_player("You do not seem to have enough money any "
            "more.");
    }

    TP->pay_money( (mixed)MONEY_H->create_money_array( cost, place ), place );

    if( !data = add_print_run( TP->query_name(), ob, number, language ) )
        return notify_player("Sorry, something seems to have gone with your "
            "request.");

    notify_player("Your copies will be ready for collection"+
        ( no_collect ? " at the collection office" : "")+" at "+
        mudtime(data->time)+".  Please note that we dispose of unclaimed "
        "items after "+query_num( LEAFLET_H->query_time_out() )+" days.  "
        "Thank you!");

} /* do_print_run() */

/** @ignore yes */
void check_cost( string str, object ob, int number, int cost, string lang ) {
    if( strlen(str) < 1 || ( str[0] != 'y' && str[0] != 'Y' ) ) {
        if( ob->query_property("new_leaflet") ) {
            LEAFLET_H->delete_leaflet( ob->query_leaflet_id() );
            ob->dest_me();
        }
        return notify_player("As you wish.  See you again.");
    }

    if( !do_open_check() ) {
        if( ob->query_property("new_leaflet") ) {
            LEAFLET_H->delete_leaflet( ob->query_leaflet_id() );
            ob->dest_me();
        }
        write("The shop seems to have been closed in the meantime, sorry.\n");
        return;
    }

    do_print_run( ob, number, cost, lang );

} /* check_cost() */

/**
 * This method will be called when the player tries to print
 * a certain number of copies of a leaflet.
 * @param obs the objects which have been matched
 * @param number the number of prints to do
 * @param language the language to print the leaflet in
 * @return 1 on success, 0 on failure
 * @see do_print()
 */
int do_print_copies( object *obs, int number, string language ) {
    object ob;
    int cost, player_money;
    string place, ret;

    if( !do_open_check() )
        return 0;

    if( keeper && shopkeeper )
        tell_object( TP, "You enquire about copying some leaflets.\n");

    tell_room( TO, TP->the_short()+" "+verbalize("enquire")+" about copying "
        "some leaflets.\n", TP );

    if( number < 1 )
        return add_failed("You must print at least one copy.");

    if( number > MAX_COPIES )
        return add_failed("Sorry, we don't print more than "+
            query_num(MAX_COPIES)+" copies at a time.");

    if( !ob = find_leaflets(obs) )
        return 0;

    place = query_property("place") || "default";
    cost = price_object( ob, number, language );

    player_money = (int)TP->query_value_in( place );
    if( place != "default")
        player_money += (int)TP->query_value_in("default");

    ret = "Printing "+query_num(number)+" "+( number == 1 ? "copy" :
        "copies")+" of "+ob->the_short()+" will cost you "+MSTR(cost);

    if( player_money < cost )
        return add_failed( ret+", but you do not have enough money to "
            "afford it.", ({ ob }) );

    notify_player( ret+".  Do you wish to continue?");
    input_to( (: check_cost :), 0, ob, number, cost, language );
    add_succeeded_mess("");
    return 1;

} /* do_print_copies() */

/** @ignore yes */
int do_print_new() {
    if( !do_open_check() )
        return 0;

    if( shopkeeper )
        tell_object( TP, "You enquire about printing a new leaflet.\n");

    tell_room( TO, TP->the_short()+" "+verbalize("enquire")+" about "
        "printing "+verbalize("new leaflets", "a new leaflet")+".\n", TP );

    notify_player("Please edit what you would like your leaflet to say.  "
       "Keep in mind however that the maximum amount of characters we can "
       "print on a leaflet is "+query_num(MAX_CHARS)+".");

    tell_room( TO, TP->the_short()+" "+verbalize("start")+" editing the text "
        "for "+verbalize("their new leaflets", TP->HIS+" new leaflet")+".\n",
        TP );

    call_out( (: call_other( $(TP), "do_edit", 0, "finish_write") :), 2 );
    return 1;

} /* do_print_new() */

/** @ignore yes */
void check_number( string str, object ob ) {
    int cost, number;
    int player_money;
    string place;

    if( !sizeof(str) || str[0] == 'q' || str[0] == 'Q' ) {
        if( ob->query_property("new_leaflet") ) {
            LEAFLET_H->delete_leaflet( ob->query_leaflet_id() );
            ob->dest_me();
        }
        return notify_player("As you wish.  See you next time.");
    }

    if( !do_open_check() ) {
        if( ob->query_property("new_leaflet") ) {
            LEAFLET_H->delete_leaflet( ob->query_leaflet_id() );
            ob->dest_me();
        }
        write("The shop seems to have been closed in the meantime, sorry.\n");
        return;
    }

    sscanf( str, "%s%d", str, number );

    if( number < 1 ) {
        notify_player("You must print at least one copy.  How many copies "
            "would you like?");
        input_to( (: check_number :), ob );
        return;
    }

    if( number > MAX_COPIES ) {
        notify_player("We cannot print more than "+
            query_num(MAX_COPIES)+" copies at a time.  How many copies would "
            "you like?");
        input_to( (: check_number :), ob );
        return;
    }

    place = query_property("place") || "default";
    cost = price_object( ob, number, query_default_language() );

    player_money = (int)TP->query_value_in( place );

    if( place != "default" )
        player_money += (int)TP->query_value_in("default");

    if( player_money < cost ) {
        if( ob->query_property("new_leaflet") ) {
            LEAFLET_H->delete_leaflet( ob->query_leaflet_id() );
            ob->dest_me();
        }
        return notify_player("Printing "+query_num(number)+" "+( number == 1 ?
            "copy" : "copies")+" would cost you "+MSTR(cost)+", but you do "
            "not have enough money to afford it.");
    }

    notify_player("Printing "+query_num(number)+" "+( number == 1 ? "copy" :
        "copies")+" will cost you "+MSTR(cost)+".  Do you wish to continue?");

    input_to( (: check_cost :), 0, ob, number, cost,
        query_default_language() );

} /* check_number() */

/** @ignore yes */
void finish_write( string input ) {
    object ob;
    string adj;
    int i, new_id;

    if( !do_open_check() ) {
        write("The shop seems to have been closed in the meantime, sorry.\n");
        return;
    }

    tell_room( TO, TP->the_short()+" "+verbalize("finish")+" writing the "
        "text for "+verbalize("their", TP->HIS )+" new leaflet.\n", TP );

    if( !input || input == "")
        return notify_player("What would be the point of printing a leaflet "
            "with no writing on it?");

    i = strlen(input);

    if( i > MAX_CHARS )
        return notify_player("Sorry, but we have no leaflets big enough to "
            "print "+query_num(i)+" characters on.  The most we can print "
            "on a leaflet is "+query_num(MAX_CHARS)+" characters.");

    if( ( new_id = LEAFLET_H->query_copyright_protected( input ) ) &&
        LEAFLET_H->query_copyright(new_id) != TP->query_name() )
        return notify_player("I'm sorry, but someone other than you is "
            "holding the copyright on the text you have written.");

    ob = clone_object(LEAFLET_OBJ);
    ob->set_name("leaflet");
    ob->set_weight(0);

    switch( i ) {
        case 0..100 :
          adj = "tiny";
          ob->set_value( 1 );
        break;
        case 101..400 :
          adj = "small";
          ob->set_value( 2 );
        break;
        case 401..800:
          adj = "medium sized";
          ob->set_value( 3 );
        break;
        case 801..1600:
          adj = "large";
          ob->set_value( 4 );
        break;
        default:
          adj = "huge";
          ob->set_value( 5 );
          ob->set_weight( 1 );
    }

    ob->set_short( adj+" leaflet");

    if( !new_id )
        new_id = LEAFLET_H->new_leaflet( TP->query_name(),
            ({ input, "printing", "common", 1 }) );

    ob->set_printer_id(id);
    ob->set_leaflet_id(new_id);
    ob->set_leaflet_size(adj);
    ob->add_property("new_leaflet", 1 );

    print_pricing( ob, query_default_language() );

    notify_player("How many copies would you like?");
    input_to( (: check_number :), ob );

} /* finish_write() */

/** @ignore yes */
void init() {
    string languages;

    ::init();

    if( !office_room ) {
        add_command("print", "<indirect:object:me>",
            (: do_print( $1, query_default_language() ) :));
        add_command("print", "new leaflet",
            (: do_print_new() :));
        add_command("print", "<number> [copies] of <indirect:object:me>",
            (: do_print_copies( $1, $4[0], query_default_language() ) :) );
        add_command("claim", "copyright on <indirect:object:me>",
            (: do_claim :) );
        if( sizeof(translation_costs) ) {
            languages = implode(
                keys(translation_costs)+({ query_default_language() }), "|");
            add_command("print", "<indirect:object:me> in {"+languages+"}",
                (: do_print($1, $4[1]) :));
            add_command("print", "<number> [copies] of <indirect:object:me> "
                "in {"+languages+"}",
                (: do_print_copies( $1, $4[0], $4[2] ) :) );
        }
    }

    if( !no_collect )
        add_command("collect", "", (: do_collect :) );

} /* init() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({
        ({ "no collection", no_collect }),
        ({ "office room", office_room }),
        ({ "shop id", query_print_shop_id() }),
        ({ "shop name", query_print_shop_name() }),
        ({ "letter cost", query_letter_cost() }),
        ({ "default language", query_default_language() }),
        ({ "shopkeeper", keeper }),
        ({ "cost per order", query_cost_per_order() }),
    });
} /* stats() */
