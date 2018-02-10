/**
 * This is the peddler object, it acts as a walking talking item
 * shop.
 */

#define MAX_PROP "max per reset"
#define TYPE "item shop type"
#define NUM_SOLD "number sold this reset"

inherit NPC_OBJ;

object cont;

object query_cont() { return cont; }

void create() {
    do_setup++;
    ::create();

    cont = clone_object("/std/container");
    cont->set_name("peddler_store");

    do_setup--;
    if( !do_setup )
        TO->setup();

    if( base_name(TO) != __FILE__[0..<3] || clonep(TO) )
        call_out("reset_me", 1800 );

} /* create() */

/** @ignore yes */
void init() {
    ::init();
    TP->add_command("browse", TO, "<indirect:object:"+
        file_name(cont)+"> {from|of} <direct:living:here>");
    TP->add_command("buy", TO, "<indirect:object:"+
        file_name(cont)+"> from <direct:living:here>");
    TP->add_command("list", TO, "goods {from|of} <direct:living:here>");
} /* init() */

/** @ignore yes */
void dest_me() {
    if( cont )
        cont->dest_me();
    ::dest_me();
} /* dest_me() */

/** @ignore yes */
private void check_cont() {
    if( !cont ) {
        cont = clone_object("/std/container");
        cont->set_name("peddler_store");
    }
} /* check_cont() */

/**
 * @ignore yes
 */
void reset_me() {
    object item;

    check_cont();

    foreach( item in INV(cont) )
        item->add_property( NUM_SOLD, 0 );

    call_out("reset_me", 1800 );

} /* reset_me() */

/**
 * This method adds an armour in as something to be able to be bought
 * from the peddles.
 * @param word the name of the armoud
 * @param number the number of the items that can be sold
 * @return 1 on success, 0 on failure
 */
int add_armour( string word, int number ) {
    object thing;

    check_cont();
    thing = (object)TO->create_object( word );

    if( !thing )
       thing = (object)ARMOURY_H->request_item( word, 100 );

    if( !thing )
        return 0;

    thing->move( cont );
    thing->add_property("item name", word );
    thing->add_property( MAX_PROP, number );
    thing->add_property( TYPE, "armour");

    return 1;

} /* add_armour() */

/**
 * This method adds an weapon in as something to be able to be bought
 * from the peddles.
 * @param word the name of the armoud
 * @param number the number of the items that can be sold
 * @return 1 on success, 0 on failure
 */
int add_weapon( string word, int number ) {
    object thing;

    check_cont();
    thing = (object)TO->create_object( word );

    if( !thing )
        thing = (object)ARMOURY_H->request_item( word, 100 );

    if( !thing )
        return 0;

    thing->move( cont );
    thing->add_property("item name", word );
    thing->add_property( MAX_PROP, number );
    thing->add_property( TYPE, "weapon");

    return 1;

} /* add_weapon() */

/**
 * This method adds an object in as something to be able to be bought
 * from the peddles.
 * @param word the name of the armoud
 * @param number the number of the items that can be sold
 * @return 1 on success, 0 on failure
 */
int add_object( string word, int number ) {
    object thing;

    check_cont();
    thing = (object)TO->create_object( word );

    if( !thing )
        thing = clone_object( word );

    if( !thing )
        return 0;

    thing->move( cont );
    thing->add_property("item name", word );
    thing->add_property( MAX_PROP, number );
    thing->add_property( TYPE, "object");

    return 1;

} /* add_object() */

/**
 * This method returns the cost of the specified item to the buyer.
 * @param thing the thing to cost
 * @param buyer who is buying it
 * @return the cost of the item
 */
int query_cost( object thing, object buyer ) {
    return (int)thing->query_value_at(TO);
} /* query_cost() */

/**
 * This method returns the costs of the item as a string.
 * @param thing the thing to buy
 * @param place the money area it is being bought in
 * @param buyer the person who is buying the object
 * @return the string money value
 */
string cost_string( object thing, string place, object buyer ) {
    return (string)MONEY_H->money_value_string(
        query_cost( thing, buyer ), place );
} /* cost_string() */

/**
 * The main entrace to the browse for things command.
 * @return 1 on success, 0 on failure
 */
int do_browse( mixed indirect_obs, string dir_match, string indir_match,
               string *words ) {
    int i;
    string place;
    object *things;

    place = query_property("place");
    if( !place || place == "")
        place = "default";

    check_cont();

    things = indirect_obs;

    if( !sizeof( things ) ) {
        init_command("say I'm afraid I don't have any of those.", 2 );
    } else {
        for( i = 0; i < sizeof( things ); i++ ) {
            init_command("say $C$"+things[ i ]->the_short()+" is priced at "+
                cost_string( things[ i ], place, TP )+".  Let me show it "
                "to you.", 4 * i + 2 );
            write( one_short()+" shows you "+things[i]->one_short()+":\n"+
                things[i]->long() );
        }
    }

    TP->add_succeeded_mess( TO, "$N ask$s $D about "+words[ 0 ]+".\n");
    return 1;

} /* do_browse() */

/**
 * The main entrace to the buy things command.
 * @return 1 on success, 0 on failure
 */
int do_buy( mixed indirect_obs, string dir_match, string indir_match,
            string *words ) {
    int i, num_left;
    object *things;

    check_cont();

    things = indirect_obs;

    if( !sizeof( things ) ) {
        init_command("say I'm afraid I don't have any of those.", 2 );
    } else {
        for( i = 0; i < sizeof( things ); i++ ) {
            num_left = (int)things[ i ]->query_property( MAX_PROP ) -
                       (int)things[ i ]->query_property( NUM_SOLD );
            if( num_left < 1 ) {
                init_command("say I'm afraid I have no "+
                    things[ i ]->query_plural()+" left.", 2 * i + 2 );
                continue;
            }
            call_out("sell_thing", 2 * i, TP, things[ i ] );
        }
    }

    TP->add_succeeded_mess( TO, "$N ask$s $D about buying "+add_a(words[ 0 ])+".\n");
    return 1;

} /* do_buy() */

/**
 * The main entrace to the list stuff command.
 * @return 1 on success, 0 on failure
 */
int do_list() {
    int num_left;
    string place, *list;
    object thing;

    list = ({ });
    place = query_property("place");
    if( !place || place == "")
        place = "default";

    check_cont();

    if( find_call_out("reset_me") == -1 )
        reset_me();

    thing = first_inventory( cont );

    while ( thing ) {
        num_left = (int)thing->query_property( MAX_PROP ) -
                   (int)thing->query_property( NUM_SOLD );
        if( num_left < 1 ) {
            thing = next_inventory( thing );
            continue;
        }

        list += ({ (string)thing->a_short()+" for "+cost_string( thing,
            place, TP )+" ("+query_num( num_left, 0 )+" left)" });
        thing = next_inventory( thing );
    }

    if( sizeof( list ) ) {
        init_command("say I have the following items for sale:", 2 );
        init_command("say $C$"+query_multiple_short( list )+".", 4 );
    } else {
       init_command("say I'm afraid I have nothing for sale.", 2 );
    }


    TP->add_succeeded_mess( TO, "$N ask$s $D what goods "+
        HE+" has in stock.\n");
    return 1;

} /* do_list() */

/**
 * The main entrace to the sell stuff command.
 * @return 1 on success, 0 on failure
 */
void sell_thing( object player, object thing ) {
    int value;
    string item_name, place;
    object copy;

    if( !player )
        return;

    if( ENV(player) != ENV(TO) )
        return;

    if( !thing )
        return;

    item_name = (string)thing->query_property("item name");
    copy = (object)TO->create_object( item_name );

    if( !copy )
        switch ( (string)thing->query_property( TYPE ) ) {
            case "armour" :
            case "weapon" :
              copy = ARMOURY_H->request_item( item_name, 80 + random( 20 ) );
            break;
            case "object" :
              copy = clone_object( item_name );
            break;
        }

    if( !copy )
        copy = clone_object( explode( file_name( thing ), "#" )[ 0 ] );

    if( !copy )
        return;

    place = query_property("place");
    if( !place || place == "")
        place = "default";

    value = (int)player->query_value_in( place );
    if( place != "default" )
        value += (int)player->query_value_in("default");

    if( TO->query_cost( copy, player ) > value ) {
        do_command("say I'm afraid you can't afford to buy "+
            copy->the_short()+".");
        copy->move( "/room/rubbish" );
        return;
    }

    thing->add_property( NUM_SOLD, thing->query_property( NUM_SOLD ) + 1 );

    player->pay_money( MONEY_H->create_money_array( TO->query_cost( copy,
        player ), place ), place );

    tell_object( player, "You pay "+the_short()+" "+cost_string( copy,
        place, player )+".\n");
    tell_room( ENV(TO), player->the_short()+" gives "+the_short()+" some "
        "money.\n", player );

   if( copy->move( player ) ) {
       do_command("'Well, here's "+copy->the_short()+", but you can't carry "
           "it at the moment.  I'll put it on the ground.");
       copy->move( environment() );
       do_command(":puts "+copy->a_short()+" on the ground.");
   } else {
       tell_room( environment(), the_short()+" gives "+
           player->the_short()+" "+copy->a_short() +".\n");
   }

} /* sell_thing() */
