/**
* A faller room. Used for such varied acts as falling, jumping, diving, and
* other such activities over long distances.
* @author Windy October 2001
*/

#define PERCENT_DAMAGE 1
#define NUMERICAL_DAMAGE 2


inherit ROOM_OBJ;

private string *begmess;
private string apexmess;
private string *endmess;
private string *dest;
private string messin;
private string messout;
private int duration;
private int damage_amount;
private int damage_type;

void print_beginning( int which );
void print_apex();
void print_end( int which );
void add_beg_message( mixed arr );
void set_apex_message( string str );
void add_end_message( mixed arr );
void set_duration( int i );
int * query_damage();
void set_damage( int type, int amount );
void handle_landing();

/** @ignore yes */
void create()
{
    begmess = ({ });
    apexmess = "";
    endmess = ({ });
    dest = ({ });
    messin = TP -> the_short() +" falls in from above.";
    messout = TP -> the_short() +" suddenly disappears.";
    damage_amount = 0;
    damage_type = 0;
    duration = 3;
    ::create();
}

/**
* Adds a message to the list of beginning messages.
* @param arr either an array of messages or an individual one
*/
void add_start_message( mixed arr )
{
    if( pointerp( arr ) )
        foreach( string str in arr )
        add_start_message( str );
    if( stringp( arr ) )
        begmess += ({ arr });

}

/**
* Return an array containing all of the beginning messages.
* @return an array of beginning messages
*/
string *query_start_messages() { return begmess; }

/**
* Set the apex message of the fall.
* @param str the apex message
*/
void set_apex_message( string str ) { apexmess = str; }

/**
* Get the apex message for the fall.
* @return the apex message
*/
string query_apex_message() { return apexmess; }

/** @ignore yes */
string query_name() { return "falling from a high place"; }

/**
* Adds a message to the array of end messages.
* @param arr either an array of messages or a single message
*/
void add_end_message( mixed arr )
{
    if( pointerp( arr ) )
        foreach( string str in arr )
        add_end_message( str );
    if( stringp( arr ) )
        endmess += ({ arr });
}

/**
* Sets the destination room(s). Upon landing, a room will be randomly selected
* from this list. If more than one player is using the faller and there are
* multiple destinations, each player has the possibility of landing in a
* different room.
* @param my_dest either a destination filename or an array of filenames
*/
void add_destination( mixed my_dest )
{
    if( pointerp( my_dest ) )
        foreach( string str in my_dest )
        add_destination( str );
    else
    if( file_size( my_dest+".c" ) > 0 )
        dest = uniq_array( dest + ({ my_dest }) );

    dest -= ({ "/room/void" });
}

/**
* Returns the filename of a random dest room.
* Note that this is not necessarily the destination that will be chosen upon
* landing, it's just a random dest chosen from the list.
* @return the destination room's filename
*/
object query_fall_destination() {
    string d;

    if( !sizeof( dest ) )
        return 0;

    d = choice( dest );

    return load_object( d );

} /* query_fall_destination() */

/**
* Sets the message that everyone in the destination room receives when
* someone 'falls in' from the faller
* @param str the message to be sent
*/
void set_messin( string str ) { messin = str; }

/**
* Sets the message that everyone in the original room receives when someone
* enters the faller room.
* @param str the message to be sent
*/
void set_messout( string str ) { messout = str; }

/**
* Returns the messin message
* @return the messin message
*/
string query_messin() { return messin; }

/**
* Returns the messout message.
* @return the messout message
*/
string query_messout() { return messout; }

/**
* Sets the duration between messages. For example, a duration of '4' means that
* there will be a four second delay between each message. You can control the
* length of the fall this way.
* @param i The number of seconds between each message
*/
void set_duration( int i ) { duration = i; }

/**
* Returns the number of seconds between fall messages
* @return the number of seconds between messages
*/
int query_duration() { return duration; }

/**
* Returns the type and amount of damage (if any) done to the player as a result
* of the fall.
* @return an integer array. The first element is the damage type, the second
* element is the amount.
*/
int *query_damage()
{
    return ({ damage_type, damage_amount });
}

/**
* Sets the damage type and amount to be dealt on the fall.
* @param type the type of damage. use 1 for an actual number
* (i.e. do 100 damage to the player no matter what), or use 2 to do
* a percentage of the player's max health to them.
* @param amount the amount of damage
*/
void set_damage( int type, int amount )
{
    damage_type = type;

    if( amount < 0 )
        damage_amount = -amount;
    else
        damage_amount = amount;
}

/** @ignore yes */
void print_beginning( int which )
{
    if( !sizeof( begmess ) )
        return;

    tell_room( TO, begmess[which] );

    if( which < sizeof( begmess ) - 1 )
        call_out( "print_beginning", duration, which + 1 );
    else
        call_out( "print_apex", duration );
}

/** @ignore yes */
void print_apex()
{
    if( apexmess != "" )
        tell_room( TO, apexmess );

    call_out( "print_end", duration, 0 );
}

/** @ignore yes */
void print_end( int which )
{
    if( !sizeof( endmess ) )
        return;

    tell_room( TO, endmess[which] );

    if( which < sizeof( endmess ) - 1 )
        call_out( "print_end", duration, which + 1 );
    else
        handle_landing();
}

/** @ignore yes */
void event_enter( object player, int notused, object notusedeither ) {
    if( living( player ) )
        call_out( "print_beginning", 3, 0 );
}

/** @ignore yes */
void handle_landing() {
    object d, *tmp, *obs;
    int i;

    if( !d = query_fall_destination() )  {
        tell_room( TO, "There are no destinations. Please notify a creator.\n");
        d = find_object("/room/void");
    }


    foreach( object ob in INV( TO ) ) {
        if( !living( ob ) )
            continue;

        ob -> add_property("death_message", query_name() );

        if( damage_type == PERCENT_DAMAGE )
            i = ( ob -> query_max_hp() ) * ( damage_amount / 100.0 ) + 1;
        else
            i = damage_amount;

        if( ( ob -> query_hp() ) - i <= 0 )
            ob -> do_death( TO );
        else
            ob -> adjust_hp( - i, TO );

        ob -> remove_property("death_message");
        ob -> move_with_look( d, messin, messout );

        if( i > 0 )
            tell_object( ob, "Ouch! That hurt!\n");
    }

    obs = INV( TO );

    tmp = filter( obs, (: !living($1) :) );
    tmp -> move( d );
    obs -= tmp;

    if( !sizeof( INV( TO ) ) )
        dest_me();
}
