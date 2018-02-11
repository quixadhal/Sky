/**
 * This inheritable deals with sending and receiving parcels.
 * Please call set_use_servant() and use the built-in 'servant'
 * variable if you decide to have a serving NPC in your post
 * office, so that some of the messaging could be done through
 * the servant for more interactivity.
 * @see /handlers/parcel_handler.c
 * @author Sandoz
 */

#define QMS(x,y)    query_multiple_short(x,y)

inherit ROOM_OBJ;

object shelf, servant;
string office_name;
int use_servant;

/** @ignore yes */
void create() {
    ::create();

    add_help_file("parcel_office");

    if( base_name(TO) == __FILE__[0..<3] )
        return;

    if( !office_name || office_name == "") {
        error("No office name set.\n");
        return;
    }

    if( !PARCEL_H->query_post_office( office_name ) ) {
        error("Invalid office name set.\n");
        return;
    }

} /* create() */

/**
 * This method queries whether or not we are a parcel office.
 * @return always return 1
 */
int query_parcel_office() { return 1; }

/**
 * This method sets the name for this parcel office that will
 * be used in notification mails etc.
 * @param name the name of the parcel office
 */
void set_office_name( string name ) { office_name = name; }

/**
 * This method returns the name of this parcel office.
 */
string query_office_name() { return office_name; }

/**
 * This function is used to set the location used for calculation
 * of which currency we accept.
 * @param location the name of the location
 */
void set_location( string location ) { add_property("place", location ); }

/**
 * This method returns the location of the post office.  This is used
 * for calculation of which currency we operate with.
 * @return the location of the place, or default if none is set
 */
string query_location() {
    string str;

    if( ( str = query_property("place") ) && str != "")
        return str;

    return "default";

} /* query_location() */

/**
 * This method is called by the parcel handler to give a message to
 * the player when they successfully send a parcel to someone.
 * Feel free to override this method in your parcel office, however
 * try to have the same checks in your override.
 * @param to the person we sent the parcel to
 * @param where the name of the post office we sent the parcel to
 * @param time the time when they should receive the parcel
 */
void event_parcel_sent( string to, string where, int when ) {
    if( where == office_name )
        where = "Thank you for the business!  We shall notify "+
            CAP(to)+" of the parcel immediately.";
    else
        where = "Thank you for the business!  The parcel should reach "+
            where+" on "+ctime(when)+", and they shall notify "+
            CAP(to)+" of its arrival immediately.";

    if( use_servant && servant && ENV(servant) && ENV(servant) == TO )
        servant->do_command("'"+where );
    else
        tell_object( TP, where+"\n");

} /* event_parcel_sent() */

/**
 * This method is called by the parcel handler to give a message to
 * the player when they successfully collected a parcel, and should
 * actually move the parcel into the player.
 * Feel free to override this method in your parcel office, however
 * you should be careful to have all the same checks in your
 * override.
 * @param obs the parcels we received
 * @param left the number of parcels left for the player
 */
void event_parcel_collected( object *obs, int left ) {
    object *failed;
    string str, them;

    str = "Thank you for the business!  ";

    switch( left ) {
      case 0 :
        str += "There are no more parcels";
      break;
      case 1 :
        str += "There is one more parcel";
      break;
      default :
        str += "There are "+query_num(left)+" more parcels";
    }

    str += " left for you to collect now.";

    if( use_servant && servant && ENV(servant) && ENV(servant) == TO )
        servant->do_command("'"+str );
    else
        tell_object( TP, "A postal worker says: "+str+"\n");

    them = ({"it", "all of them"})[query_group(obs)];
    str = ( servant ? servant->the_short() : "A postal worker")+" gives "+
        TP->the_short()+" "+QMS(obs, "a");

    tell_room( TO, str+".\n", TP );

    if( sizeof( failed = filter( obs, (: $1->move($2) :), TP ) ) ) {
        str += ", however you are too burdened to accept "+them+", so "+
            ( servant ? servant->HE : "he")+" places ";
        if( shelf ) {
            obs = filter( failed, (: $1->move($2) :), shelf );
            failed -= obs;
            if( sizeof(failed) )
                str += QMS(failed, "the")+" on "+shelf->the_short();
            if( sizeof( obs ) ) {
                obs->move(TO);
                if( sizeof(failed) )
                    str += " and ";
                str += QMS(obs, "the")+" "+( query_property("here") ||
                    "on the floor");
            }
        } else {
            failed->move(TO);
            str += QMS(obs, "the")+" "+( query_property("here") ||
                "on the floor");
        }
    }

    tell_object( TP, str+".\n");

    if( servant ) {
        servant->do_command("'I hope it's something good!");
        servant->do_command("beam");
    }

} /* event_parcel_collected() */

/**
 * This method notifies the room that we are using a servant NPC.
 * This is needed for open/close checks and for some messages which
 * will be spoken through the NPC for more interactivity.
 */
void set_use_servant() { use_servant = 1; }

/**
 * This method queries whether or not we are using a servant NPC.
 * @return 1 if we are using a servant NPC, 0 if not
 */
int query_use_servant() { return use_servant; }

/**
 * This method sets the servant NPC for the room
 * @param ob the servant NPC to use
 */
void set_servant( object ob ) { servant = ob; }

/**
 * This method returns the servant NPC, if we have one.
 * @return the servant NPC, or 0 none is set, or has been killed
 */
object query_servant() { return servant; }

/**
 * Makes a shelf and moves it to the room, if there is a shelf
 * and a player is too burdened to accept a parcel, the items
 * collected will be moved to it, otherwise they just get moved
 * to the floor.
 * @param short the short desc of the shelf
 * @param long the long desc of the shelf
 * @return the created shelf object for further modifications
 */
object make_shelf( string short, string long ) {
    string *bits;

    shelf = clone_object("/std/surface");

    bits = explode( short, " ");

    shelf->set_name( bits[<1] );

    if( sizeof(bits) > 1 )
        shelf->add_adjective( bits[0..<2] );

    shelf->set_short( short );
    shelf->set_long( long );
    shelf->set_max_weight( 10000 );
    shelf->move(TO);
    shelf->reset_get();

    return shelf;

} /* make_shelf() */

/** @ignore yes */
int do_open_checks() {
    if( !use_servant || ( servant && ENV(servant) && ENV(servant) == TO ) )
        return 1;

    add_failed_mess("There is no-one here to serve you.\n");
    return 0;

} /* do_open_checks() */

/** @ignore yes */
int do_send( object *obs, string where, string to ) {
    if( do_open_checks() ) {
        where = PARCEL_H->query_real_office_name(where);
        tell_object( TP, "You enquire about sending "+QMS(obs, "the")+" to "+
            where+" for "+CAP(lower_case(to))+".\n");
        tell_room( TO, TP->the_short()+" $V$0=enquires,enquire$V$ about "
            "sending a parcel to "+where+".\n", TP );
        return PARCEL_H->do_send( obs, to, where, office_name,
            query_location(), servant );
    }

    return 0;

} /* do_send() */

/** @ignore yes */
int do_cost( object *obs, string where ) {
    if( do_open_checks() ) {
        where = PARCEL_H->query_real_office_name(where);
        tell_object( TP, "You enquire about the cost of sending "+
            QMS(obs, "the")+" to "+where+".\n");
        tell_room( TO, TP->the_short()+" $V$0=enquires,enquire$V$ about "
            "the cost of sending a parcel to "+where+".\n", TP );
        return PARCEL_H->do_cost( obs, office_name, where,
            query_location(), servant );
    }

    return 0;

} /* do_send() */

/** @ignore yes */
int do_collect() {
    if( do_open_checks() ) {
        tell_object( TP, "You enquire about collecting the parcels that have "
            "been sent to you.\n");
        tell_room( TO, TP->the_short()+" $V$0=enquires,enquire$V$ about "
            "collecting the parcels that have been sent to "+
            verbalize("them", TP->HIM )+".\n", TP );
        return PARCEL_H->do_collect( office_name, servant );
    }

    return 0;

} /* do_collect() */

/** @ignore yes */
int do_list_offices() {
    if( do_open_checks() ) {
        tell_object( TP, "You enquire about the list of valid post "
            "offices.\n");
        tell_room( TO, TP->the_short()+" $V$0=enquires,enquire$V$ about "
            "the list of valid post offices.\n", TP );
        return PARCEL_H->do_list( servant );
    }

    return 0;

} /* do_list() */

/** @ignore */
void init() {
    add_command("list", "offices", (: do_list_offices() :) );
    add_command("send",
        "<indirect:object:me> to <string'post office'> for <word'player'>",
        (: do_send( $1, $4[1], $4[2] ) :) );
    add_command("cost",
        "<indirect:object:me> to <string'post office'>",
        (: do_cost( $1, $4[1] ) :) );
    add_command("collect", "");
} /* init() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({
        ({"office name", office_name }),
        ({"location", query_location() }),
        ({"using servant", use_servant ? "yes" : "no" }),
        ({"servant", servant }),
        ({"shelf", shelf }),
    });
} /* stats() */
