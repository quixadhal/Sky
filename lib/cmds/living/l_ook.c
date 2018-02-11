/**
 * Look command, trial out by Turrican for a commands daemon.
 * @change Modified to let one look at a detail on an item
 * - Shaydz 3/04/2001.
 * @change Rewrote 'look around' code to include adjacent rooms
 * without a linker and made it a tad faster
 * - Shiannar and Sandoz, 2002.
 */

#include <dirs.h>
#include <player.h>
#include <room.h>
#include <weather.h>

inherit COMMAND_BASE;

string *ords;

/** @ignore yes */
void create() {
    ::create();
    ords = keys(ABS_FACING);
} /* create() */

/** @ignore yes */
int cmd_look_room() {
    object room;
    int dark, *coords;

    if( !room = ENV(TP) ) {
        add_failed_mess("You are in limbo and cannot look at anything.\n");
        return 0;
    }

    if( !TP->query_property("dead") )
        dark = TP->check_dark( room->query_light() );

    if( creatorp(TP) ) {
        coords = room->query_co_ord();
        write( "%^CYAN%^"+ file_name(room)+( sizeof(coords) ?
               "  ("+coords[0]+","+coords[1]+","+coords[2]+")" :
               "  (unset)" )+"%^RESET%^\n" );
    }

    write("$P$Look$P$"+room->long( 0, dark ) );

    if( dark && creatorp(TP) )
        write("%^YELLOW%^As a creator, you can see:%^RESET%^\n"+
            room->long( 0, 0 ) );

    return 1;

} /* cmd_look_room() */

/**
 * This method returns the 'look' description of the object.  This should
 * be used for anyone want to do look like capabilities for their
 * objects.
 * @param thing the object to look at
 * @param player the player doing the looking
 * @param dark the darkness modifier
 * @param verb the verb to use when telling the player their item is being
 * looked at
 * @param arg the string used for the look
 * @return the look string
 */
string query_look_thing( object thing, object player, int dark, string verb,
                         string arg ) {
    string ret, other;
    object env;

    ret = "";

    if( living( thing ) ) {
        if( thing == player ) {
            ret += "Looking at yourself again?  What a narcissist!\n";
        } else {
            tell_object( thing, player->one_short()+" $V$0=looks,look$V$ "
                "at you.\n");
        }
    } else {
        env = ENV(thing);
        while( env && !living(env) )
            env = ENV(env);
    }

    if( env && env != player ) {
        tell_object( env, player->the_short()+" "+verb+" at your "+
            thing->pretty_short()+".\n");
        if( other = thing->query_long() )
            ret += other;
        if( other = thing->query_long_details( arg, dark, player ) )
            ret += other;
    } else {
        ret += thing->long( arg, dark );
    }

    return ret;

} /* query_look_thing() */

/** @ignore yes */
string weather_long( string str ) {
    object env;

    env = ENV(TP);

    if( env->query_property("location") == "inside")
        return "You are not outside.\n";

    switch( str ) {
      case "sun" :
        if( WEATHER->query_day() )
            return "Ouch that hurts.\n";
        else
            return "The sun is not up, sorry.\n";
      case "moon" :
        if( WEATHER->query_day() )
            return "The moon is not up, try at night.\n";
        if( WEATHER->query_moon_string(env) )
            return WEATHER->query_moon_string(env);
        return "The moon is not up at the moment, try again later.\n";
      case "cloud" :
      case "clouds" :
        if( strsrch( str = WEATHER->weather_string(env), "cloud") != -1 )
            return str+".\n";
        return "The sky appears to be quite clear right now.\n";
      case "lightning" :
        if( strsrch( str = WEATHER->weather_string(env), "lightning") != -1 )
            return str+".\n";
        return "You should probably wait for more thunderous weather to "
            "look at lightning.\n";
    }

    return "You look up at the sky.  "+WEATHER->weather_string(env)+".\n";

} /* weather_long() */

/** @ignore yes */
int check_exists( mixed file ) {
    if( stringp(file) )
        return ( objectp( find_object(file) ) || file_size(file+".c") > 0 );
    return 0;
} /* check_exists() */

/**
 * This method returns the contents of a room visible with 'look around'.
 * @param room the filename of the room to get the contents for
 * @return the contents of the room
 */
object *get_contents( mixed room ) {
    object *contents;

    if( !stringp(room) || !( room = find_object(room) ) ||
        TP->check_dark( room->query_light() ) )
        return 0;

    contents = filter( INV(room) - ({ TP }),
        (: living($1) && $1->query_visible($2) :), TP );

    return contents;

} /* get_contents() */

/** @ignore yes */
string look_around() {
    int i;
    string prep, room, *rooms, terrain;
    object place, tmp, *contents;
    mixed dest_other;
    mapping locations;

    place = ENV(TP);

    if( TP->check_dark( (int)place->query_light() ) )
        return "You can't see well enough to look around.\n";

    terrain = place->query_terrain();
    rooms = ( place->query_linked() ? place->query_rooms() : ({ }) );
    locations = ([ ]);

    say( TP->one_short()+" $V$0=looks,look$V$ around.\n");

    if( !sizeof(rooms) && sizeof( dest_other = place->query_dest_other() ) ) {
        for( i = 0; i < sizeof(dest_other); i += 2 ) {
            if( ( terrain || dest_other[i+1][ROOM_OBV] ) &&
                member_array( dest_other[i], ords ) != -1 ) {
                place->set_destination( dest_other[i] );
                tmp = place->query_door_control( dest_other[i] );
                if( !tmp || tmp->query_open() ) {
                    contents = get_contents(
                        place->query_destination( dest_other[i] ) );
                    if( sizeof(contents) )
                        locations["to the "+dest_other[i]] = contents;
                }
            }
        }
    }

    foreach( room in rooms ) {
        if( !tmp = find_object( room ) )
            continue;

        if( TP->check_dark( tmp->query_light() ) )
            continue;

        contents = filter( INV( tmp ) - ({ TP }),
            (: living($1) && $1->query_visible(TP) :) );

        if( sizeof( contents ) ) {
            if( !prep = (string)room->query_static_preposition() )
                prep = "";
            else if( prep != "" )
                prep += " ";

            prep += TP->convert_message( room->the_short() );

            if( undefinedp( locations[prep] ) )
                locations[prep] = contents;
            else
                locations[prep] += contents;
        }
    }

    contents = filter( INV(place) - ({ TP }),
        (: living($1) && $1->query_visible(TP) :) );

    if( !sizeof( contents ) )
        terrain = "";
    else
        terrain = place->process_living( contents )[0];

    if( sizeof(locations) ) {
        rooms = ({ });

        foreach( room, contents in locations )
            rooms += ({ query_multiple_short(contents) + " " + room });

        if( sizeof( rooms ) == 1 )
            prep = "you spot "+rooms[ 0 ]+".\n";
        else
            prep = "you spot "+query_multiple_short(rooms)+".\n";

        if( terrain != "")
            return terrain[0..<3] + ", while " + prep;

        return CAP(prep);

    } else {
        if( terrain == "")
            return "You do not see anyone else about.\n";
        return terrain;
    }

} /* look_around() */

/** @ignore yes */
mixed cmd_string( string arg ) {
    string ret;
    object room;
    int dark, retval;
    mixed other, func;

    if( !room = ENV(TP) ) {
        add_failed_mess("You are in limbo and cannot look at anything.\n");
        return 0;
    }

    switch( arg ) {
      case "sun":
      case "moon":
      case "moonlight":
      case "weather":
      case "sky":
      case "cloud":
      case "clouds":
      case "lightning":
        write( weather_long(arg) );
        return 1;
      case "around" :
        write( look_around() );
        return 1;
    }

    if( !TP->query_property("dead") )
        dark = TP->check_dark( room->query_light() );

    if( dark == 2 ) {
        add_failed_mess("It is way too bright to see anything at all.\n");
        return 0;
    }

    if( dark == -2 ) {
        add_failed_mess("It is way too dark to see anything at all.\n");
        return 0;
    }

    if( LENGTHEN[arg] )
        arg = LENGTHEN[arg];

    // Added, so that you could look through aliased exits.
    arg = room->expand_alias(arg);

    if( room->query_relative( arg ) )
        return 0;

    if( !room->query_exit(arg) )
        arg = TP->find_abs(arg);

    if( room->query_exit( arg ) ) {
        // This is for terrains or exits using function pointers.
        room->set_destination( arg );
        other = room->query_destination( arg );

        if( functionp(other) )
            other = evaluate( other, TP );

        ret = room->query_look(arg);

        if( ret && ret != "" ) {
            write(ret+"\n");
            retval = 1;
        }

        func = room->query_look_func(arg);

        if( pointerp(func) && sizeof(func) ) {
            if( call_other( func[0], func[1], 1 ) )
                retval = 1;
        } else if( functionp(func) ) {
            if( evaluate( func, 1 ) )
                retval = 1;
        }

        if( !retval ) {
            if( check_exists(other) ) {
                object door;

                door = room->query_door_control(arg);

                if( room->query_door_open(arg) || door->query_transparent() ) {
                    other->force_load();
                    TP->set_looked( find_object( other ) );
                    dark = TP->check_dark( other->query_light() );
                    write( other->long( 0, dark ) );
                    retval = 1;
                } else {
                    add_failed_mess( door->the_short()+" "+({"is", "are"})
                        [query_group(door)]+" closed and not transparent.\n");
                }
            } else {
                add_failed_mess("You cannot look in that direction.\n");
            }
        }
    }

    return retval;

} /* cmd_string() */

/** @ignore yes */
varargs mixed cmd_object( object *obs, string arg, int flag ) {
    object thing, room;
    int dark, retval;
    string ret;

    if( !room = ENV(TP) ) {
        add_failed_mess("You are in limbo and cannot look at anything.\n");
        return 0;
    }

    if( !TP->query_property("dead") )
        dark = TP->check_dark( room->query_light() );

    if( dark == 2 ) {
        add_failed_mess("It is way too bright to see anything at all.\n");
        return 0;
    }

    if( dark == -2 ) {
        add_failed_mess("It is way too dark to see anything at all.\n");
        return 0;
    }

    ret = "";

    // Check details on items first.
    if( flag ) {
        ret = "";

        foreach( thing in obs )
            if( thing->query_detail(arg) )
                ret = thing->query_detail(arg);

        if( ret == "") {
            add_failed_mess("Can't find "+arg+". No match.\n");
            return 0;
        }
    } else {
        foreach( thing in obs )
            ret += query_look_thing( thing, TP, dark, "looks", arg );
    }

    if( ret != "") {
        write("$P$Look$P$"+ret );
        retval = 1;
    }

    // Doors are a direction.
    //if( room->query_exit( arg ) )
        //cmd_string(arg);

    return 1;

} /* cmd_object() */

/** @ignore yes */
mixed query_patterns() {
    return ({
        "", (: cmd_look_room() :),
        "<string'direction'>", (: cmd_string( $4[0] ) :),
        "<indirect:object>", (: cmd_object( $1, $4[0] ) :),
        "at <indirect:object>", (: cmd_object( $1, $4[0] ) :),
        "[at] <string> {in|on} <indirect:object>",
        (: cmd_object( $1, $4[0], 1 ) :),
        "in <indirect:object>", (: cmd_object( $1, $4[0] ) :),
        "inside <indirect:object>", (: cmd_object( $1, $4[0] ) :),
        "around", (: cmd_string("around") :),
    });
} /* query_patterns() */
