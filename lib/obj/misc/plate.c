/**
 * This is an inheritable for a plate, including cutlery.  You can put things
 * on it.  You can also eat food directly from the plate.  It was designed
 * for the pub_shop.c inheritable, for use with the meals that can be sold
 * there.
 * @author Lemming
 * @started 26/12/1999
 */

inherit "/std/surface";

/** @ignore */
int do_eat( object *obs ) {
    object ob, *food;

    if( sizeof( obs ) ) {
        food = filter( obs, (: $1->query_edible() :) );
    } else {
        add_failed_mess("There is nothing on $D.\n");
        return 0;
    }

    if( !sizeof( food ) ) {
        if( sizeof( obs ) )
            add_failed_mess("You can't eat $I from $D.\n", obs );
        else
            add_failed_mess("There is nothing edible on $D.\n");
        return 0;
    }

    // Use tell_object/room here because add_succeeded_mess's aren't always
    // printed in the correct order.
    tell_object( TP, "You start eating from "+the_short()+".\n");
    tell_room( ENV(TP), TP->the_short()+" $V$0=starts,start$V$ eating from "+
        the_short()+".\n", TP );

    add_succeeded_mess("");

    // Food items should give their own messages.
    foreach( ob in food )
        ob->do_eat();

    return 1;

} /* do_eat() */

/** @ignore */
void init() {
    add_command("eat",
        "<indirect:object:direct-obs> {from|on} <direct:object:me>",
        (: do_eat($1) :) );
    add_command("eat", "from <direct:object:me>", (: do_eat( INV(TO) ) :) );
} /* init() */

mapping query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return 0;
} /* query_static_auto_load() */
