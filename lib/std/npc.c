/**
 * This is the main NPC inheritable.
 * @author Pinkfish
 * @change Modified to properly work on Divided Sky and
 * fixed skills/stats - Sandoz, 20th July 2001.
 * @see /std/npc/npc_base
 */

#include <player.h>
#include <route.h>
#include <skills.h>
#include <top_ten_tables.h>

inherit "/std/living/living";
inherit "/global/player/new_parse";
inherit "/std/living/response_mon";
inherit "/global/player/guild-race";

class chat_data {
    int chance;
    int total_w;
    int *weight;
    mixed chats;
    int last_chat;
}

nosave class chat_data chats, a_chats;

nosave int added_lang,
           follow_speed,
           chats_off,
           cannot_change_position,
           always_return_to_default_position,
           *move_after,
           aggressive,
           death_xp,
           death_xp_set,
           join_fight_type,
           _level,
           chase_id;

nosave string *move_zones,
              *following_route,
              *enter_commands,
              _home,
              join_fight_mess,
              *_greets,
              cur_lang;

nosave object *to_greet;

nosave mixed queued_commands, doing_story, combat_actions;

string find_rel( string word, int from );
void do_move_after( int running_away );
void do_route_move();
void add_language( string lang );
int query_chats_off();
int check_env();

/** @ignore yes */
int query_cols() { return 79; }

void create() {
    do_setup++;
    living::create();
    new_parse::create();
    response_mon::create();
    do_setup--;
    reset_get();

    follow_speed = 3;

    // Do they won't get silly commands like "rearrange".
    known_commands = ({ });

    living_commands();
    parser_commands();

    add_property("npc", 1 );
    add_property("no ambiguous", 1 );

    set_con(12);
    set_dex(12);
    set_int(12);
    set_str(12);
    set_wis(12);
    set_max_hp(10000);
    set_hp(10000);
    set_max_gp(1000);
    set_gp(1000);

    // Default to 5 seconds.
    always_return_to_default_position = 5;
    cur_lang = "common";

    // This is done only if the NPC doesn't have a language set after setup.
    // Check their env here as well.
    call_out( function() {
        if( !added_lang ) {
            add_language("common");
        if( clonep() && !ENV(TO) )
            call_out( function() {
                if( !ENV(TO) )
                    move("/room/rubbish");
            }, 60 );
        } }, 2);

    if( !do_setup ) {
        TO->setup();
        TO->reset();
    }

} /* create() */

/**
 * This method sets the unique propety on the NPC.
 * Unique NPCs give no xp when killed and will generate
 * informs when dying amongst other things.
 */
void set_unique() { add_property("unique", 1 ); }

/**
 * This method will fetch an item from the armoury and move it into the NPC.
 * @param the item to get
 * @param the condition of the item
 * @return the object of the new item
 */
object get_item( string item, int cond ) {
    object ob;

    if( !sizeof(item) || cond < 1 )
        error("No item name specified or condition less than 1.\n");

    if( !ob = ARMOURY_H->request_item( item, cond ) )
        error( sprintf("Failed to clone %s for %s (%s).\n", item, short(),
            file_name(TO) ) );

    ob->move(TO);

    return ob;

} /* get_item() */

/**
 * This method adds a language to the NPC.
 * @param lang the language to add
 */
void add_language( string lang ) {
    int lvl;
    string skill;

    if( !LANGUAGE_H->test_language( lang ) )
        return ;

    if( LANGUAGE_H->query_language_spoken( lang ) ) {
        skill = LANGUAGE_H->query_language_spoken_skill( lang );
        lvl = TO->query_skill( skill );
        TO->add_skill_level( skill, 100 - lvl );
    }

    if( LANGUAGE_H->query_language_written( lang ) ||
        LANGUAGE_H->query_language_magic( lang ) ) {
        skill = LANGUAGE_H->query_language_written_skill( lang );
        lvl = TO->query_skill( skill );
        TO->add_skill_level( skill, 100 - lvl );
    }

    cur_lang = lang;
    added_lang = 1;

} /* add_language() */

/**
 * This method sets the current language of the NPC.
 * @param lang the language to set as current
 */
int set_language( string lang ) {
    if( !LANGUAGE_H->test_language( lang ) )
        return 0;
    cur_lang = lang;
    return 1;
} /* set_language() */

/**
 * This method returns the current language of the NPC.
 * @return the current language
 */
string query_current_language() { return cur_lang; }

/**
 * This method allows you to control the NPC and get it to do
 * actions.  This can be used for NPC control and intelligence.
 * @param words the action to preform
 * @see init_equip()
 * @see init_command()
 * @example
 * ob = clone_object(NICE_HAIRY_APE);
 * ob->do_command("'I am a hairy ape!");
 * ob->do_command("emote apes around the room.");
 */
int do_command( string words ) {
    if( TO->query_property(PASSED_OUT) )
        return -1;

    if( stringp( words ) )
        return command( words );

    printf("Invalid parameter to do_command: %O for NPC %O in %O.\n",
           words, TO, ENV(TO) );
    return -1;

} /* do_command() */

/**
 * This method allows you submit delayed commands to the NPC via a call_out.
 * @param str the action to perform
 * @param tim the time delay after which to perform the action
 * @see do_command()
 * @see queue_command()
 * @see delay_command()
 */
varargs void init_command( string str, int tim ) {
    call_out("do_command", tim, str );
} /* init_command() */

/**
 * @ignore yes
 * No need to print any messages to an NPC.
 * Less functions will be called if we don't :)
 */
void add_message( string message, mixed things );

/** @ignore yes */
varargs string fix_string(string str, int width, int indent, mixed *args ...) {
     return "/global/player/events"->fix_string( str, width, indent, args ...);
} /* fix_string() */

/** @ignore yes */
string evaluate_message( mixed stuff ) {
    return "/global/player/events"->evaluate_message( stuff );
} /* evaluate_message() */

/** @ignore yes */
string fit_message( string message ) {
     return "/global/player/events"->fit_message( message );
} /* fit_message() */

/** @ignore yes */
string convert_message( string message ) {
    return "/global/player/events"->convert_message( message );
} /* convert_message() */

/** @ignore yes */
void comm_event( mixed thing, string type, string start, string rest,
                 string lang ) {
    if( !objectp( thing ) && !pointerp( thing ) )
        return;
    event( thing, type, start, rest, lang );
    call_other( INV(TO), "event_"+type, TO, start, rest, lang );
} /* comm_event() */

/**
 * @ignore yes
 * We need to be able to whisper as it generates event_whisper().
 */
void do_whisper( object ob, string event_type, string start, string type,
                string words, object *others, string lang, object me ) {
    event( ob, event_type, start, type, words, others, lang, me );
} /* do_whisper() */

/**
 * This method is used to expand the message strings used in the
 * npc messages.  It is used for chat strings and such things like
 * that.  The strings it expands are of the form:<br>
 * $lname$, $mname$, $aname$, $itheshort$ ...<br>
 * The first letter determines the type of object being referenced.
 * They are:
 * <dl>
 * <dt>m
 * <dd>Me!  The NPC itself.
 * <dt>l
 * <dt>A random living object in the NPC's environment.
 * <dt>i
 * <dt>A random interactive object (player) in the NPC's environment.
 * <dt>a
 * <dd>Chooses a random attacker from those attacking the NPC.
 * <dt>o
 * <dd>Choose a random object in the inventory of the NPC.
 * </dl>
 * After the first letter is a type of information being request.
 * <dl>
 * <dt>name
 * <dd>The name of the selected object.
 * <dt>cname
 * <dd>The capitalised name of the selected object.
 * <dt>gender
 * <dd>The gender string of the selected object (male, female, neuter).
 * <dt>poss
 * <dd>The possessive string of the selected object.
 * <dt>obj
 * <dd>The objective string of the selected object.
 * <dt>pronoun
 * <dd>The pronoun string of the selected object.
 * <dt>gtitle
 * <dd>The guild title of the selected object (only useful on livings).
 * <dt>ashort
 * <dd>The a_short() call.
 * <dt>possshort
 * <dd>The poss_short() call.
 * <dt>theshort
 * <dd>The the_short() call.
 * <dt>oneshort
 * <dd>The one_short() call.
 * </dl>
 * @see set_chat_string()
 * @see expand_mon_string()
 * @param in_str the input string
 * @param on the object to use for the 'o' matching
 */
string expand_string( string in_str, object on ) {
    string *str, ret;
    int i, add_dollar;
    object liv, *obs, ob;

    in_str = convert_message( in_str );
    str = explode( in_str, "$" );
    ret = "";
    for( i = 0; i < sizeof( str ); i++ ) {
        if( i % 2 == 0 ) {
            if( add_dollar )
                ret += "$";
            ret += str[i];
            add_dollar = 1;
            ob = 0;
        }
        else switch (str[i][0]) {
        case 'm' :
            ob = TO;
        case 'l' :
            if( !ob ) {
                if( !liv ) {
                    obs = INV( ENV(TO) ) - ({ TO });
                    if( sizeof( obs = filter( obs, (: living($1) :) ) ) )
                        liv = choice( obs );
                }
                if( !liv )
                    break;
                ob = liv;
            }
        case 'i' :
            if( !ob ) {
                if( !liv ) {
                    obs = INV( ENV(TO) ) - ({ TO });
                    if( sizeof( obs = filter( obs, (: interactive($1) :) ) ) )
                        liv = choice( obs );
                }
                if( !liv )
                    break;
                ob = liv;
            }
        case 'a' :
            if( !ob ) {
                if( !sizeof( obs = (object *)TO->query_attacker_list() ) )
                    break;
                ob = choice( obs );
            }
        case 'o' :
            if( !ob ) {
                if( !on ) {
                    obs = INV( ENV(TO) );
                    if( sizeof( obs = filter( obs, (: !living( $1 ) :) ) ) )
                        on = choice( obs );
                }
                ob = on;
            }
            switch (str[ i ][ 1 .. ]) {
            case "theshort" :
                ret += (string)ob->the_short();
                add_dollar = 0;
                break;
            case "ashort" :
                ret += (string)ob->a_short();
                add_dollar = 0;
                break;
            case "oneshort":
                ret += (string)ob->one_short();
                add_dollar = 0;
                break;
            case "possshort" :
                ret += (string)ob->poss_short();
                add_dollar = 0;
                break;
            case "name" :
                ret += (string)ob->query_name();
                add_dollar = 0;
                break;
            case "cname" :
                ret += (string)ob->query_cap_name();
                add_dollar = 0;
                break;
            case "gender" :
                ret += (string)ob->query_gender_string();
                add_dollar = 0;
                break;
            case "poss" :
                ret += (string)ob->query_possessive();
                add_dollar = 0;
                break;
            case "obj" :
                ret += (string)ob->query_objective();
                add_dollar = 0;
                break;
            case "gtitle" :
                ret += (string)ob->query_gender_title();
                add_dollar = 0;
                break;
            case "pronoun" :
                ret += (string)ob->query_pronoun();
                add_dollar = 0;
                break;
            default :
                if( add_dollar )
                    ret += "$";
                ret += str[i];
                add_dollar = 1;
                break;
            }
            ob = 0;
            break;
        default :
            if( add_dollar )
                ret += "$";
            ret += str[i];
            add_dollar = 1;
            ob = 0;
            break;
        }
    }

    if( strlen( ret ) && ret[ strlen( ret ) - 1 ] == '$' )
        return ret[0..strlen(ret)-2];

    return ret;

} /* expand_string() */

/**
 * This method executes the string passed in.  It handles all the
 * stuff which is needed from the chat_string stuff.
 *
 * If the input is a function pointer then it is evaluated with one
 * parameter, being the NPC.
 *
 * If the input is a string then the first letter determines what will
 * be done with it.  All these are passed through expand_string
 * so that exciting things can be done.
 * <ul>
 * <li># - A call_other will be generated.  The parameters are separated by
 * ':'s, so "#bing:fred:chicken" would call<pre>
 * TO->bing("fred", "chicken");
 * </pre>.
 * <ul>', ", : - These will generate a 'say', 'lsay' or 'emote'.
 * <ul>@ - This will run the passed in command.  Eg: "@frog" would cause the
 *     soul command frog to be used.
 * <ul>Anything else will be used as a message to be sent to everyone in the
 *     room.
 * </ul>
 * @param str the thing to execute
 * @see expand_string()
 */
void expand_mon_string( mixed str ) {
    string *args;

    if( functionp( str ) )
        evaluate( str, TO );
    else {
        if( !stringp( str ) && ENV( TO ) ) {
            tell_room( ENV(TO), "%^RED%^"+ TO->the_short()+" says: please "
                "bugreport me, I have a bad load_chat.%^RESET%^\n");
        }
        switch( str[ 0 ] ) {
          case '#' :
            args = explode( str[ 1..], ":" );
            switch( sizeof( args ) ) {
              case 1 :
                call_other( TO, args[0] );
              break;
              case 2 :
                call_other( TO, args[0], args[1] );
              break;
              case 3 :
                call_other( TO, args[0], args[1], args[2] );
              break;
              case 4 :
                call_other( TO, args[0], args[1], args[2], args[3] );
              break;
              default :
                call_other( TO, args[0], args[1], args[2], args[3], args[4] );
              break;
            }
          break;
          case ':' :
          case '\'' :
          case '"' :
            init_command( expand_string( str, 0 ), 1 );
            break;
          case '@' :
            init_command( expand_string( str[ 1 .. ], 0 ), 1 );
            break;
          default :
            tell_room( ENV(TO), expand_string( str, 0 ) +"\n" );
        }
    }
} /* expand_mon_string() */

/** @ignore yes */
void set_name( string name ) {
    if( query_name() && query_name() != "object" )
        return;
    ::set_name( name );
    add_plural( pluralize( name ) );
    set_short( name );
    set_long("This is a half-fabricate NPC.\n");
    set_living_name( name );
} /* set_name() */

/** @ignore yes */
int query_sp() { return 50; }

/** @ignore yes */
int adjust_sp( int number ) { return 50; }

/** @ignore yes */
void event_whisper( object thing, string start, string mess, object *obs,
                    string lang, object me) {
    response_mon::event_whisper( thing, mess, obs, lang, me);
} /* event_whisper() */

/**
 * This method sets up the basic abilities and race of the critter.
 * @see query_race()
 * @see query_level()
 * @see query_gender()
 * @param race this is the race of the character.  It should be
 * a race that's understood by the /std/race.c
 * @param level this is the level of the NPC.  The number is used
 * by the race object to set ability scores.
 * @param gender this is the gender of the NPC.  It can be either a
 * number ( 0 - neuter, 1 - male or 2 - female ) or a string ("neuter",
 * "male", "female")
 * @example
 * basic_setup("human", 200, "male");
 */
void basic_setup( string race, int level, mixed gender ) {
    RACE_OBJ->set_level( level, race );
    _level = level;
    set_gender( gender );
} /* basic_setup() */

/**
 * This method returns the level of the NPC.
 * @return the level of the NPC
 * @see basic_setup()
 */
int query_level() { return _level; }

/**
 * This method sets the message to use when joining into fights.
 * @param str the message to print when joining a fight
 * @see query_join_fights()
 * @see set_join_fight_type()
 */
void set_join_fights(string str) { join_fight_mess = str; }

/**
 * This method returns the message to use when joining into fights.
 * @return the message to print when joining a fight
 * @see set_join_fights()
 * @see set_join_fight_type()
 */
string query_join_fights() { return join_fight_mess; }

/**
 * This method sets the flag which allows the NPC to join into fights.
 * If this is set to a non-zero value then the NPC will join into
 * fights in progress using the fight joining message.
 * @param i 1 if the npc is to join fights, 0 if not
 * @see set_join_fights()
 * @see query_join_fight_type()
 */
void set_join_fight_type( int i ) { join_fight_type = i; }

/**
 * This method returns the flag which allows the NPC to join into fights.
 * @return 1 if the NPC is to join fights, 0 if not
 * @see set_join_fights()
 * @see query_join_fight_type()
 */
int query_fight_type() { return join_fight_type; }

/**
 * This method check to see if the NPC should start attacking someone
 * when they enter the NPC's environment.  It is called from inside
 * init().  The NPC will only attack if the agressive is set and the
 * person is visible to be attacked.  The property
 * <pre>"no attack"</pre> can be set on the npc (or player) to
 * stop them being attacked.
 * @param who the person to potentially start attacking
 * @see set_aggressive()
 */
void start_attack( object who ) {
    if( !who || !aggressive )
        return;

    if( !who->query_visible(TO) || who->query_auto_loading() ||
        file_name(who) == DEATH || who->query_property("guest") ||
        ( userp(who) && !interactive(who) ) ||
        who->query_property("no attack") ) {
        return;
    }

    // Use a call_other() just in case there are shadows.
    if( aggressive > 1 || interactive(who) )
        TO->attack_ob( who );

} /* start_attack() */

/**
 * This method returns the current aggressive level of the NPC.
 * If the aggressive is set to 1, then the NPC will attack all players
 * that enter its environment.  If the aggressive is set to 2 then
 * the NPC will attack everything (including other NPCs).
 * <p>
 * See the function start_attack() for information about things you
 * can do to stop an aggressive NPC from attacking things.
 * @return the aggressive level of the NPC
 * @see set_aggressive()
 * @see start_attack()
 */
int query_aggressive() { return aggressive; }

/**
 * This method sets the current aggressive level of the NPC.
 * If the aggressive is set to 1, then the NPC will attack all players
 * that enter its environment.  If the aggressive is set to 2 then
 * the NPC will attack everything (including other NPCs).
 * <p>
 * See the function start_attack() for information about things you
 * can do to stop an aggressive NPC from attacking things.
 * @see query_aggressive()
 * @see start_attack()
 * @see set_join_fights()
 * @see set_throw_out()
 * @param level the new aggressive level
 */
void set_aggressive( int level ) {
    aggressive = level;
    // this added to make aggressive npcs join in fights if they aren't
    // already set that way.  This is needed coz otherwise you can sneak
    // into a room and fight the aggressive NPCs one at a time
    // - Ceres 10/97
    if( level && !join_fight_mess )
        join_fight_mess = TO->one_short()+" joins in the fight!";
} /* set_aggressive() */

/** @ignore yes */
void init() {
    set_heart_beat( 1 );

    if( !ENV(TO) || file_name( ENV(TO) )[1..4] == "room" )
        return;

    if( aggressive )
        call_out("start_attack", 1, TP );

    if( _greets && TP && !TP->query_property("dead") &&
        TP->query_visible(TO) && !TO->is_fighting(TP) ) {
        if( pointerp(to_greet) ) {
            if( member_array( TP, to_greet ) == -1 )
                to_greet += ({ TP });
        } else
            to_greet = ({ TP });
    }

} /* init() */

/**
 * This method makes the NPC to equip whatever they have.
 * @see do_command()
 */
void init_equip() { command("equip"); }

/** @ignore yes */
string long( string str, int dark ) {
    string s;

    if( dark < -1 )
        s = "You can only make out a rough shape in the gloom.\n";
    else if( dark > 1 )
        s = "You can only make out a rough shape in the glare.\n";
    else
        s = query_long();

    if( !dark ) {
        s += CAP(HE)+" "+health_string(TO,0)+".\n";
        s += CAP(HE)+" is " + query_position_short() + ".\n";
        s += ( query_property( PASSED_OUT ) ?
             CAP(HE)+" appears to be passed out.\n" : "" );
        s += calc_extra_look();
        s += query_living_contents(0);
    }

    return s;

} /* long() */

/**
 * This method returns 1 if it is ok to turn of the NPC's heart beat.
 * This can be overridden for times when the heart beat needs to be
 * kept on for some reason.
 * @return 1 if the heart beat should go off, 0 if it should stay on
 */
int query_ok_turn_off_heart_beat() {
    return 1;
} /* query_ok_turn_off_heart_beat() */

/** @ignore yes */
varargs int adjust_hp( int num, object attacker, object wep, string attack ) {
    set_heart_beat( 1 );
    return ::adjust_hp( num, attacker, wep, attack );
} /* adjust_hp() */

/** @ignore yes */
int adjust_gp( int number ) {
    set_heart_beat( 1 );
    return ::adjust_gp( number );
} /* adjust_gp() */

/**
 * This method can be temporarily overwritten in NPCs to stop them
 * wandering under certain circumstances.  For example if someone is
 * fighting in the room with the NPC, and we want the NPC to watch.
 * Alternatively to overwriting the function, you can set a "no wander"
 * property on the NPC.
 * @return should return 1 if we don't want to wander
 */
int do_not_wander() { return query_property("no wander"); }

/**
 * This method adds a move zone onto the NPC.  The move zones control
 * which areas the NPC will wander into, a move zone is set on the
 * room and the NPC will only enter rooms which have a matching
 * move zone.  If there is no move zone, then the NPC will enter
 * any room.
 * <p>
 * If the parameter is an array each of the elements of the array
 * will be added as a move zone.
 * @param zone the zone(s) to add
 * @see remove_move_zone()
 * @see query_move_zones()
 * @see set_move_after()
 * @see do_not_wander()
 */
void add_move_zone( mixed zone ) {
    if( !sizeof(zone) )
        return;

    if( !pointerp(move_zones) )
        move_zones = ({ });

    if( pointerp( zone ) ) {
        foreach( zone in zone )
            add_move_zone( zone );
    } else if( stringp(zone) && member_array( zone, move_zones ) == -1 )
        move_zones += ({ zone });

} /* add_move_zone() */

/**
 * This method removes a move zone from the NPC.
 * @param zone the zone(s) to remove
 * @see add_move_zone()
 * @see query_move_zones()
 * @see set_move_after()
 */
void remove_move_zone( mixed zone ) {
    if( !sizeof( move_zones ) )
        return;

    if( pointerp( zone ) ) {
        move_zones -= zone;
        return;
    }

    move_zones -= ({ zone });

} /* remove_move_zone() */

/**
 * This method returns the current list of move zones on the NPC
 * @return the current list of move zones
 * @see add_move_zone()
 * @see remove_move_zones()
 * @see set_move_after()
 */
string *query_move_zones() { return move_zones || ({ }); }

/**
 * This method sets the speed at which the NPC will randomly
 * wander around.  The NPC will wander at the speed:<pre>
 * speed = after + random(rand)
 * </pre>This is called every time the NPC sets up for its next move.
 * <p>
 * The move zones control which areas the NPC will wander into,
 * a move zone is set on the room and the NPC will only enter
 * rooms which have a matching move zone.  If there is no move
 * zone, then the NPC will enter any room.
 * @param after the fixed amount of time
 * @param rand the random amount of time
 * @see remove_move_zone()
 * @see query_move_zones()
 * @see add_move_zone()
 * @see do_not_wander()
 */
void set_move_after( int after, int rand ) {
    move_after = ({ after, rand });
    if( after != 0 && rand != 0 )
        do_move_after(0);
} /* set_move_after() */

/**
 * This method returns the current move after values.
 * It returns an array of the form:<pre>
 *    ({
 *      after,
 *      rand,
 *     })
 * </pre>
 * @return the move after values
 * @see set_move_after()
 */
int *query_move_after() { return move_after; }

/**
 * This is called when the NPC decides it must continue down
 * a certain route.  This will be called by the wander handler
 * and can be used to force the NPC to wander along a route
 * faster.
 * @param running_away this is 1 if the npc is running away
 * @see set_move_after()
 * @see do_not_wander()
 */
void do_move_after( int running_away ) {
    if( sizeof( following_route ) )
        do_route_move();
    else
        WANDER_H->move_after( running_away );
} /* do_move_after() */

/** @ignore yes */
void dest_me() {
    WANDER_H->delete_move_after(TO);
    ::dest_me();
} /* dest_me() */

/** @ignore yes */
string expand_nickname(string str) { return str; }

/**
 * This method throws away any queued commands.
 * It doesn't remove the call_out, however if no
 * new commands are added there will be no effect.
 * @see init_equip()
 * @see init_command()
 * @see delay_command()
 * @see do_command()
 * @see queue_command()
 */
void delete_queued_commands() { queued_commands = 0; }

/**
 * This method returns the queued command list.
 * @see init_equip()
 * @see init_command()
 * @see delay_command()
 * @see do_command()
 * @see queue_command()
 */
mixed query_queued_commands() { return queued_commands || ({ }); }

/** @ignore yes */
private void next_queued_command() {
    mixed next;

    if( !sizeof( queued_commands ) )
        return;

    // The interval of the next command on queue.
    next = queued_commands[0];

    if( intp( next ) ) {
        queued_commands = queued_commands[1..];
        if( !sizeof( queued_commands ) )
            return;
        next = queued_commands[0];
    }
    while( stringp( next ) ) {
      if( TO->queue_commands() ) {
          call_out( (: next_queued_command :), 2 );
          return;
      }
      do_command( next );
      queued_commands = queued_commands[1..];
      if( !sizeof( queued_commands ) )
          return;
      next = queued_commands[0];
    }
    call_out( (: next_queued_command :), next );
} /* next_queued_command */

/**
 * This method allows you to control the NPC and get it to do
 * actions where they are queued as for players.   The command
 * is always delayed by delay even if there are no commands pending
 * unlike queue_command(). This function
 * is 100% compatible with queue_command() and init_command().
 * @param words the action to perform
 * @param interval to wait before the command.
 * @see queue_command()
 * @see query_queued_commands()
 * @see init_command()
 * @see do_command()
 * @example
 * ob = clone_object(NICE_HAIRY_APE);
 * ob->delay_command("'I am a hairy ape!", 10 );
 * ob->delay_command("emote apes around the room.", 2 );
 * ob->queue_command("emote get banana.", 3 );
 * ob->queue_command("emote get apple.");
 * After 10 seconds it says "I am a hariy ape",
 * 2 seconds after that it apes around the room,
 * immediately following that it gets a banana
 * and 3 seconds after that it gets an apple.
 */
int delay_command( string words, int interval ) {
    if( query_property( PASSED_OUT ) )
        return -1;
    if( stringp( words ) ) {
        // If there are no queued commands, start the unqueuing process.
        if( !sizeof( queued_commands ) )
            call_out( (: next_queued_command :), interval );
        queued_commands = query_queued_commands() + ({ interval, words });
        return 1;
    }
    printf( "Invalid parameter to delay_command: %O for monster %O in %O.\n",
            words, TO, ENV(TO) );
    return -1;
} /* delay_command() */

/**
 * This method allows you to control the NPC and get it to do
 * actions where they are queued as for players.  If there are no
 * commands pending the command is executed immediately.  This function
 * is 100% compatible with delay_command() and init_command().
 * @param words the action to perform
 * @param interval the time to wait before processing another command
 * If omitted defaults to 2 seconds as per players
 * @see delay_command()
 * @see query_queued_commands()
 * @see init_command()
 * @see do_command()
 * @example
 * ob = clone_object(NICE_HAIRY_APE);
 * ob->queue_command("'I am a hairy ape!");
 * ob->queue_command("emote apes around the room.", 5 );
 * ob->queue_command("get banana", 10 );
 * ob->delay_command("emote get apple.", 3 );
 * Right away it says "I am a hairy ape",
 * 2 seconds later it apes around the room,
 * 5 seconds after that it gets a banana
 * and 13 seconds (10+3) after that it gets an apple.
 */
varargs int queue_command( string words, int interval ) {
    if( query_property( PASSED_OUT ) )
        return -1;
    if( stringp( words ) ) {
        if( undefinedp( interval ) )
            interval = 2;
        /* if there are no queued commands, start the unqueuing process */
        if( !sizeof( queued_commands ) ) {
            queued_commands = ({ words, interval });
            next_queued_command();
            return 1;
        }
        /* there are queued commands, so just add them to the existing ones */
        queued_commands = queued_commands + ({ words, interval });
        return 1;
    }
    printf( "Invalid parameter to queue_command: %O for monster %O in %O.\n",
            words, TO, ENV(TO) );
    return -1;
} /* queue_command() */

/** @ignore yes */
varargs int move( mixed dest, string messin, string messout ) {
    int result;
    object before = ENV(TO);
    result = living::move( dest, messin, messout );
    if( !result )
        me_moveing( before );
    return result;
} /* move() */

/** @ignore yes */
void room_look() {
    ::room_look();

    if( !sizeof( enter_commands ) )
        return;

    foreach( string cmd in enter_commands ) {
        if( functionp( cmd ) ) {
            call_out( cmd, 1 + random( sizeof( enter_commands ) + 1 ), TO );
            continue;
        }
        if( cmd[ 0 .. 0 ] == "#" )
            call_out( cmd[1..], 1 + random( sizeof( enter_commands ) + 1 ) );
        else
            init_command( cmd, 1 + random( sizeof( enter_commands ) + 1 ) );
    }
} /* room_look() */

/**
 * This method is used to make the NPC run away.  This will be
 * called by the combat code for wimpy when the NPC is bellow the
 * number of points used to trigger the wimpy action.
 * @return 1 if successfuly ran away
 */
int run_away() {
    if( query_property( "run away" ) == -1 )
        return 0;
    /* just to make npcs a bit trickier... */
    do_command("lose all");
    become_flummoxed();
    if( sizeof( following_route ) ) {
        do_route_move();
        return 1;
    }
    if( query_property( "run away" ) ) {
        do_move_after( 1 );
        return 1;
    }
    return ::run_away();
} /* run_away() */

/** @ignore yes */
int rand_num( int min, int max ) {
    return min + random( max - min + 1 );
} /* rand_num() */

/**
 * This method sets the stats for the NPC to some exciting random
 * values.
 * @param min the minimum value of stats
 * @param max the maximum value of stats
 */
void set_random_stats( int min, int max ) {
    set_str( rand_num( min, max ) );
    set_dex( rand_num( min, max ) );
    set_int( rand_num( min, max ) );
    set_con( rand_num( min, max ) );
    set_wis( rand_num( min, max ) );
} /* set_random_stats() */

/**
 * This method adds a command to be called whenever the npc enters
 * a room.  If the command is a string, then it will be executed
 * as if they had typed it.  If it is a function then the function
 * will be evaluated and one argument (the NPC itself) will be passed
 * in.
 * @param str the enter commands to add
 * @see reset_enter_commands()
 * @see query_enter_commands()
 */
void add_enter_commands( mixed str ) {
    if( !enter_commands )
        enter_commands = ({ });

    if( stringp( str ) || functionp( str ) )
        enter_commands += ({ str });
    else if( pointerp( str ) )
        foreach( str in str )
            add_enter_commands( str );

} /* add_enter_commands() */

/**
 * This method returns the current array of enter commands.
 * @return the current array of enter commands
 * @see reset_enter_commands()
 * @see add_enter_commands()
 */
string *query_enter_commands() { return enter_commands || ({ }); }

/**
 * This method resets the array of enter commands back to nothing.
 * @see add_enter_commands()
 * @see query_enter_commands()
 */
void reset_enter_commands() { enter_commands = 0; }

/** @ignore yes */
void event_enter( object dest, string mess, object from ) {
    // Stop massive spam in /room/rubbish and /room/void.
    if( ENV(TO) && file_name( ENV(TO) )[1..4] == "room" )
        return;
    living::event_enter( dest, mess, from );
} /* event_enter() */

/**
 * This method is used to make the NPC follow after attackers
 * when they leave the room.
 * @param who the person we are chasing
 * @param dir the direction to follow them in
 */
void do_follow_move( object who, string dir ) {
    chase_id = 0;

    // Only chase if we are not following a route,
    // and are not fighting anyone here.
    if( !sizeof(following_route) && !TO->query_fighting() )
        do_command(dir);

} /* do_follow_move() */

/** @ignore yes */
void event_exit( object who, string mess, object to ) {
    string fname;
    mixed exits;
    int i;

    new_parse::event_exit( who, mess, to );
    living::event_exit( who, mess, to );

    // Don't move if move_after isn't set, or if following a route.
    if( !to || !move_after || sizeof( following_route ) )
        return;

    // Chase the sucker, if they're on our attacker list.
    if( member_array( who, TO->query_attacker_list() ) == -1 )
        return;

    exits = ENV(TO)->query_dest_dir();
    fname = file_name(to);

    // No exit to destination.
    if( ( i = member_array( fname, exits ) ) == -1 )
        return;

    // Don't follow if we are only supposed to wander in move_zones and
    // don't have unrestricted follow set.
    if( !TO->query_property("unrestricted follow") &&
        !sizeof( to->query_zones() & query_move_zones() ) )
        return;

    if( chase_id )
        remove_call_out( chase_id );

    chase_id = call_out( (: do_follow_move :), 3 + random(follow_speed),
        who, exits[i-1] );

} /* event_exit() */

/**
 * This method will set up a 'home' location for the NPC.
 * This is the file name of the room the NPC
 * should return to whenever it is moved.
 * It can be used to make unique NPCs returns
 * to their default location etc.
 * @param loc the filename of the room to set as home
 */
void set_home_location( string loc ) {
    _home = loc;
} /* set_home_location() */

/**
 * This method will return the NPC's home location.
 * @return the home location
 */
string query_home_location() { return _home; }

/** @ignore yes */
void event_move_object( mixed from, mixed to ) {
    ::event_move_object( from, to );

    if( chase_id )
        remove_call_out( chase_id );

    if( _home && find_call_out("check_home_location") == -1 )
        call_out("check_home_location", 4 + random( 4 ) );

} /* event_move_object() */

/** @ignore yes */
void check_home_location() {
    string where;

    if( ENV(TO) && ( where = base_name(ENV(TO)) ) &&
        where[1..4] != "room" && where != _home &&
        !sizeof( TO->query_following_route() ) )
        TO->move_me_to( _home );

} /* check_home_location() */

/** @ignore yes */
int query_time_left() { return 1; }

/**
 * This function makes the more intelligent NPCs pick up their
 * weapons after being disarmed.
 * It can be overwritten if you don't want the default disarm
 * response to work for your NPC.
 * @param disarmer the person who disarmed us
 * @param weapon the weapon that was disarmed
 */
void event_disarm( object disarmer, object weapon ) {
    int i, time;

    if( !objectp(weapon) )
        return;

    i = (int)TO->query_int();
    if( ( time = 10 - sqrt( i ) ) < 4 )
        time = 4;

    if( i >= 20 || i > random( 60 ) ) {
        if( ENV(weapon) == ENV(TO) )
            init_command("get "+file_name(weapon), time++ );
        init_command("hold "+file_name(weapon), time );
    }

} /* event_disarm() */

/**
 * This method queries the greetings of the NPC,
 * if there are any set.
 * @return the greetings array
 */
string *query_greetings() { return _greets; }

/**
 * This method sets up the greeting strings to use when
 * someone enters the room the NPC is in.
 * This is useful for shopkeepers and more unique NPCs.
 * $ob$ in greeting strings will be replaced with
 * the people that are being greeted.
 *
 * To use a greeting in form of a soul command, please
 * use an "@" in front of the greeting string. Soul greetings
 * will be called on each person individually, and it is
 * possible to set up random souls by separating different
 * soul commands by a ":" in the greeting string (see example).
 *
 * It is also possible to use a function instead of a string
 * greeting.  The function can either be a function
 * pointer or a string of the following format -
 * "#file_name:function", and will then be called with
 * the following arguments - the thing (object) doing
 * the greeting, and the array of objects to greet.
 *
 * NOTE : The NPC will only greet those livings he can see,
 * and that are not ghosts or otherwise dead.
 *
 * @param str the greeting or array of greetings to use
 * @example set_greeting( ({"Greetings, $ob$!", (: do_greet :) }) );
 * @example set_greeting( ({"'Hello $ob$!", "#"+PUB+"pub:do_greet"}) );
 * @example set_greeting("@salute $ob$:bow $ob$:bing $ob$");
 */
void set_greeting( mixed str ) {
    string *tmp;

    if( !_greets )
        _greets = ({ });

    if( stringp(str) && sizeof(str) ) {
        if( str[0] == "#" ) {
            str = str[1..];
            if( sizeof( tmp = explode( str, ":" ) ) < 2 ) {
                write("Greeting function missing a function name in "+
                      file_name(TO)+".\n");
                return;
            }
            if( !find_object(tmp[0]) && !file_exists( tmp[0]+".c" ) ) {
                write("Greeting function pointing to a non-existent "
                      "file ("+tmp[0]+".c) in "+file_name(TO)+".\n");
                return;
            }
        }
        _greets += ({ str });
    } else if( pointerp(str) )
        _greets += str;
    else if( functionp(str) )
        _greets += ({ str });

} /* set_greeting() */

/** @ignore yes */
object *query_to_greet() { return to_greet; }

/**
 * The main heart beat function.  This is called by the driver
 * every 2 seconds.  Does all the maintenance stuff like fixing
 * up hps, greeting people and stuff like that.
 */
void heart_beat() {
    int i, j, d;
    mixed using;
    string *tmp;

    living::heart_beat();

    if( hp < 0 || query_property("dead") )
        d = 1;

    i = check_env();

    if( !d && i && _greets && sizeof( to_greet ) && sizeof( to_greet = filter(
        to_greet, (: $1 && ENV($1) == ENV(TO) :) ) ) ) {
        if( functionp( using = choice(_greets) ) ) {
            evaluate( using, TO, to_greet );
        } else if( sizeof( to_greet = filter( to_greet,
            (: !TO->is_fighting($1) :) ) ) ) {
            switch( using[0] ) {
              case '#' :
                tmp = explode( using[1..], ":");
                call_other( tmp[0], tmp[1], TO, to_greet );
              break;
              case '@' :
                tmp = explode( using[1..], ":");
                foreach( object ob in to_greet )
                    do_command( replace( choice(tmp), "$ob$",
                        ob->query_name() ) );
              break;
              default :
                do_command( replace( using, "$ob$",
                    query_multiple_short( map( to_greet,
                        (: $1->short() :) ) ) ) );
            }
        }
    }

    to_greet = 0;

    if( hp == max_hp && gp == max_gp && TO->query_ok_turn_off_heart_beat() ) {
        if( !i || ( !chats && !a_chats ) ) {
            set_heart_beat( 0 );
            return;
        }
    }

    if( !d && !query_chats_off() && i ) {
        if( TO->query_fighting() ) {
            if( sizeof(doing_story) && sizeof( doing_story[ 1 ] ) ) {
                expand_mon_string( doing_story[ 1 ][ 0 ] );
                doing_story[ 1 ] = doing_story[ 1 ][ 1 .. ];
            } else if( a_chats && random( 1000 ) < a_chats->chance &&
                sizeof( a_chats->weight ) ) {
                i = random( a_chats->total_w + 1 );
                while( ( i -= a_chats->weight[ j ] ) > 0 )
                    j++;
                if( j == a_chats->last_chat )
                    j = ( j + 1 ) % sizeof( a_chats->weight );
                a_chats->last_chat = j;
                if( pointerp( a_chats->chats[ j ] ) ) {
                    expand_mon_string( a_chats->chats[ j ][ 0 ] );
                    if( !doing_story )
                        doing_story = allocate( 2, ({ }) );
                    doing_story[ 1 ] = a_chats->chats[ j ][ 1 .. ];
                } else
                    expand_mon_string( a_chats->chats[ j ] );
            }
        } else if( chats ) {
            if( sizeof(doing_story) && sizeof( doing_story[ 0 ] ) ) {
                expand_mon_string( doing_story[ 0 ][ 0 ] );
                doing_story[ 0 ] = doing_story[ 0 ][ 1 .. ];
            } else if( chats && random( 1000 ) < chats->chance &&
                sizeof( chats->weight ) ) {
                i = random( chats->total_w + 1 );
                while( ( i -= chats->weight[ j ] ) > 0 )
                    j++;
                if( j == chats->last_chat )
                    j = ( j + 1 ) % sizeof( chats->weight );
                chats->last_chat = j;
                if( pointerp( chats->chats[ j ] ) ) {
                    expand_mon_string( chats->chats[ j ][ 0 ] );
                    if( !doing_story )
                        doing_story = allocate( 2, ({ }) );
                    doing_story[ 0 ] = chats->chats[ j ][ 1 .. ];
                } else
                    expand_mon_string( chats->chats[ j ] );
            }
        }
    }
} /* heart_beat() */

/**
 * This method queries the speed at which the NPC will follow
 * after a player, when they leave combat.
 * @return the current follow speed
 * @see set_follow_speed()
 */
int query_follow_speed() { return follow_speed; }

/**
 * This method sets the speed at which the NPC will follow
 * after a player, when they leave combat.
 * @param speed the new follow speed
 * @see query_follow_speed()
 */
void set_follow_speed( int speed ) { follow_speed = speed; }

/**
 * This method checks to see if there are any players in the environment
 * of the NPC.  This should be used to determine if chats should
 * be turned off and other things which should only work in the
 * presence of players.
 * @return 1 if there is a player in the room, 0 if not
 */
int check_env() {
    object thing;

    if( !ENV(TO) || file_name( ENV(TO) )[1..4] == "room")
        return 0;

    if( ENV(TO)->query_linked() )
        return 1;

    foreach( thing in INV( ENV(TO) ) )
      if( interactive( thing ) || thing->query_slave() )
          return 1;

    return 0;

} /* check_env() */

/**
 * This method causes the NPC to move in the given direction.
 * @param move the direction to move in
 */
void do_move( string move ) {
    command( move );
} /* do_move() */

/**
 * This method gets the next direction to go in the route which
 * is currently being followed and will remove this direction
 * from the array.
 * @return the next direction to go to
 * @see query_following_route()
 * @see do_route_move()
 */
string get_next_route_direction() {
    string direc;
    if( !sizeof( following_route ) )
        return 0;
    direc = following_route[0];
    following_route = following_route[1..];
    return direc;
} /* get_next_route_direction() */

/**
 * This method returns the current array of directions we are following
 * as a route.
 * @see get_next_route_direction()
 * @see do_route_move()
 */
string *query_following_route() { return following_route || ({ }); }

/**
 * This method moves the NPC one more location along the route it
 * is following.
 * @see query_following_route()
 * @see get_next_route_direction()
 */
void do_route_move() {
    if( !sizeof( following_route ) )
         return;
    do_command( get_next_route_direction() );
} /* do_route_move() */

/**
 * This method is called by the move_me_to function after the
 * route handler has successfuly discovered the route to follow.
 * @param route the route to follow
 * @param delay the delay to follow it with
 * @param dest route destination
 * @see move_me_to()
 */
protected void got_the_route( string *route, int delay, string dest ) {
    following_route = route;
    if( sizeof(route) ) {
        WANDER_H->move_me_please( delay, dest );
        do_route_move();
    } else
        TO->stopped_route();
} /* got_the_route() */

/**
 * This method will move the NPC to the specified destination.  The
 * NPC will walk from where they currently are to the destination using
 * the time delay specified between the movements.
 * <p>
 * If the location is reached then the function "stopped_route" will
 * be called on the npc.  The first argument to the function will
 * be 0 if the npc did not reach its destination and 1 if it did.
 * @param dest the destination to go to
 * @param delay the time delay between each move
 * @example
 * inherit "/std/npc";
 *
 * void go_home() {
 *    move_me_to(HOME_LOCATION, 2);
 * } /\* go_home() *\/
 *
 * void stopped_route(int success) {
 *    if (success) {
 *       do_command("emote jumps for joy.");
 *    } else {
 *       do_command("emote looks sad and lost.");
 *    }
 * } /\* stopped_route() *\/
 * @see get_next_route_direction()
 * @see got_the_route()
 * @see query_following_route()
 * @see do_route_move()
 */
varargs void move_me_to( string dest, int delay ) {
    string *dest_dir, *start_dir;

    if( !ENV(TO) )
        return ;

    // Very likely to have no route.
    if( !MAP_H->static_query_short( dest ) ||
        !MAP_H->static_query_short( file_name( ENV(TO) ) ) ) {
        // One of the rooms has no exits.
        if( !sizeof( dest_dir = dest->query_dest_dir() ) ||
            !sizeof( start_dir = ENV(TO)->query_dest_dir() ) ) {
            call_out("stopped_route", 1 );
            return;
        }
        call_out("stopped_route", 1 );
        return;
    }

    ROUTE_HANDLER->get_route( dest, file_name( ENV(TO) ),
                (: got_the_route( $1, $(delay), $(dest) ) :) );

} /* move_me_to() */

/** @ignore yes */
int clean_up( int parent ) {
    if( parent || query_property("unique") ||
        query_hp() < query_max_hp() || check_env() )
        return 1;
    move("/room/rubbish");
    return 1;
} /* clean_up() */

/**
 * This method sets the value of the unable to change position flag.
 * This flag will be checked by the soul, and by anything else which
 * deliberatly changes someones position.
 * @param flag the unable to change position flag
 * @see /std/living/living->set_default_position()
 */
void set_cannot_change_position( int flag ) {
    cannot_change_position = flag;
} /* set_cannot_change_position() */

/**
 * This method returns the current value of the unable to change
 * position flag.
 * @return the unable to change position flag
 * @see /std/living/living->set_default_position()
 */
int query_cannot_change_position() {
    return cannot_change_position;
} /* query_cannot_change_position() */

/**
 * This method overrides the position code, so that if the position
 * is changed we are changed back to the default position.
 * @param new_pos the new position
 * @ignore yes
 */
void set_position( string new_pos ) {
    if( always_return_to_default_position && !query_property(PASSED_OUT) &&
        new_pos != query_position() && TP != TO )
        call_out( (: return_to_default_position(0) :),
                     always_return_to_default_position );
    ::set_position( new_pos );
} /* set_position() */

/**
 * This method sets the status of the flag that makes the NPC return
 * to the default position if its position is changed.  The flag
 * specifies the length of time to wait before causing the
 * default position to be restored.
 * @param tim the time to wait before the position is restored
 * @see /std/living/living->return_to_default_position()
 * @see query_always_return_to_default_position()
 */
void set_always_return_to_default_position( int tim ) {
    always_return_to_default_position = tim;
} /* set_always_return_to_default_position() */

/**
 * This method returns the status of the flag that makes the NPC return
 * to the default position if its position is changed.  The flag
 * specifies the length of time to wait before causing the
 * default position to be restored.
 * @return the time to wait before the position is restored
 * @see /std/living/living->return_to_default_position()
 * @see set_always_return_to_default_position()
 */
int query_always_return_to_default_position() {
    return always_return_to_default_position;
} /* query_always_return_to_default_position() */

/**
 * This method can be used to turn the chats of the NPC on/off.
 * @param i 1 to turn the chats off, 0 to turn them on
 */
void set_chats_off( int i ) { chats_off = i; }

/**
 * This method queries whether or not the chats of the NPC
 * have been turned off.
 * @return 1 if the chats are off, 0 if chats are on
 */
int query_chats_off() { return chats_off; }

/**
 * This method loads up the chat strings for the NPC.  This will be
 * an array containing pairs of elements, the first pair is the
 * weighting of the chat and the second is the chat to use.
 * <p>
 * All the weights in the array are added up and then a random
 * number is chosen in the weighting.  Then that element is looked
 * up in the array.  This way you can control a chat and make it
 * rare.
 * <p>
 * If the chat string is an array then this a story, the story will be
 * executed one after another and no other chats will be executed
 * in between.  The chat chance is still used to determine when the
 * chats will occur. Special strings can be used which will replace with
 * object names, see expand_mon_string() for further information.
 * <p>
 * The chat chance is a chance (in 1000) of the chat occuring.  You
 * will need to play with this yourself to see which frequency of
 * chatting you wish for your NPC.
 * @example
 * load_chat(60,
 *    ({
 *       1, "'I am a chicken!",
 *       // Make this one more likely to occur.
 *       2, ":clucks like a chicken."
 *       1, ":pecks at $lpossshort$ foot."
 *     }));
 * @example
 * load_chat(100,
 *    ({
 *       1, "'I am a simple farmer."
 *       1, ":waves $mposs$ pitchfork around."
 *       // A story, they will always occur in this order
 *       1, ({
 *            "'Once upon a time there was a rabbit.",
 *            "'It was a nice rabbit and hung around in bars.",
 *            "'It sung little songs about fruit.",
 *           }),
 *      }));
 * @see expand_mon_string()
 * @param chance the chance in 1000 of a chat working every 2 seconds
 * @param data the chats and chat weights to use
 * @see load_a_chat()
 * @see query_chat_chance()
 * @see query_chat_string()
 */
void load_chat( int chance, mixed data ) {
    int i;

    if( chance < 1 )
        error("Invalid chat chance in load_chat().\n");

    if( !i = sizeof(data) / 2 )
        error("Invalid chats in load_chat().\n");

    chats = new( class chat_data, chance : chance,
            chats  : allocate( i ), weight : allocate( i ) );

    while( i-- ) {
        chats->total_w += data[i*2];
        chats->weight[i] = data[i*2];
        chats->chats[i] = data[i*2+1];
    }

} /* load_chat() */

/**
 * This method sets the current chat chance for the NPC.
 * @param i the chat chance
 * @see load_chat()
 * @see query_chat_chance()
 */
void set_chat_chance( int i ) {
    if( !chats )
        error("Trying to specify a chat chance with no chats.\n");
    chats->chance = i;
} /* set_chat_chance() */

/**
 * This method returns the current chat chance for the NPC.
 * @return the current chat chance
 * @see set_chat_chance()
 * @see load_chat()
 */
int query_chat_chance() {
    if( chats )
        return chats->chance;
    return 0;
} /* query_chat_chance() */

/**
 * This method queries the current chat strings for the NPC.
 * The return value is an array of three elements where the
 * first member is the sum of the weights of all chats, the
 * second member contains the weights of chats, and the third
 * member contains the chat strings.
 * chat string is formatted.
 * @return the current chat strings
 * @see load_chat()
 * @see query_chat_string()
 */
mixed query_chat_string() {
    if( chats )
        return ({ chats->total_w, chats->weight, chats->chats });
    return 0;
} /* query_chat_string() */

/**
 * This method adds an array of chats or a single chat string into
 * the current list of chat strings.  See load_chat() for a longer
 * description of the chat string.
 * @param weight the weight of the chat, or an array of weights and chats
 * @param chat the new chat string
 * @see load_chat()
 * @see remove_chat_string()
 */
varargs void add_chat_string( mixed weight, mixed chat ) {
    int i;

    if( !chats )
        error("Cannot use add_chat_string() on an NPC with no chats.\n");

    if( pointerp( weight ) ) {
        i = sizeof( weight ) / 2;
        while( i-- )
            add_chat_string( weight[i*2], weight[i*2+1] );
    } else if( member_array( chat, chats->chats ) == -1 ) {
        chats->total_w += weight;
        chats->weight += ({ weight });
        chats->chats += ({ chat });
    }

} /* add_chat_string() */

/**
 * This method attempts to remove the given chat string from the
 * current list of chat strings.  The chat message is checked
 * to see if it exists in the array, the weighting of the
 * string is ignored.
 * @param chat the chat string to remove
 * @see add_chat_string()
 * @see load_chat()
 */
void remove_chat_string( mixed chat ) {
    int i;

    if( !chats )
        return;

    if( pointerp( chat ) ) {
        map( chat, (: remove_chat_string($1) :) );
    } else if( ( i = member_array( chat, chats->chats ) ) != -1 ) {
        chats->total_w -= chats->weight[i];
        chats->weight = delete( chats->weight, i, 1 );
        chats->chats = delete( chats->chats, i, 1 );
    }

}  /* remove_chat_string() */

/**
 * This method loads up the set of chat strings to use while in combat.
 * @param chance the chance of the chat occuring
 * @param data the chat strings and weights to use
 * @see load_chat()
 * @see query_achat_chance()
 * @see query_achat_string()
 */
void load_a_chat( int chance, mixed data ) {
    int i;

    if( chance < 1 )
        error("Invalid chat chance in load_a_chat().\n");

    if( !i = sizeof(data) / 2 )
        error("Invalid chats in load_a_chat().\n");

    a_chats = new( class chat_data, chance : chance,
              chats  : allocate( i ), weight : allocate( i ) );

    while( i-- ) {
        a_chats->total_w += data[i*2];
        a_chats->weight[i] = data[i*2];
        a_chats->chats[i] = data[i*2+1];
    }

} /* load_a_chat() */

/**
 * This method sets the current aggressive chat chance the NPC.
 * @param i the attack message chat chance
 * @see load_chat()
 * @see load_achat()
 * @see query_achat_chance()
 */
void set_achat_chance( int i ) {
    if( !a_chats )
        error("Trying to specify an achat chance with no achats.\n");
    a_chats->chance = i;
} /* set_achat_chance() */

/**
 * This method returns the current attack chat chance for the NPC.
 * @return the current chat chance
 * @see set_achat_chance()
 * @see load_chat()
 * @see load_a_chat()
 */
int query_achat_chance() {
    if( a_chats )
        return a_chats->chance;
    return 0;
} /* query_achat_chance() */

/**
 * This method queries the current aggressive chat strings for the NPC.
 * The return value is an array of three elements where the
 * first member is the sum of the weights of all chats, the
 * second member contains the weights of chats, and the third
 * member contains the chat strings.
 * chat string is formatted.
 * @return the current chat strings
 * @see load_chat()
 * @see query_chat_string()
 */
mixed query_achat_string() {
    if( a_chats )
        return ({ a_chats->total_w, a_chats->weight, a_chats->chats });
    return 0;
} /* query_achat_string() */

/**
 * This method adds an array of attack chats or a single attack chat
 * string into the current list of achat strings.
 * See load_chat() for a longer description of the chat string.
 * @param weight the weight of the chat, or an array of weights and chats
 * @param chat the new chat string
 * @see load_chat()
 * @see load_a_chat()
 * @see remove_achat_string()
 */
varargs void add_achat_string( mixed weight, mixed chat ) {
    int i;

    if( !a_chats )
        error("Cannot use add_achat_string() on an NPC with no achats.\n");

    if( pointerp( weight ) ) {
        i = sizeof( weight ) / 2;
        while( i-- )
            add_achat_string( weight[i*2], weight[i*2+1] );
    } else if( member_array( chat, a_chats->chats ) == -1 ) {
        a_chats->total_w += weight;
        a_chats->weight += ({ weight });
        a_chats->chats += ({ chat });
    }

} /* add_achat_string() */

/**
 * This method attempts to remove the given chat string from the
 * current list of attack message chat strings.  The chat message
 * is checked to see if it exists in the array, the weighting of
 * the string is ignored.
 * @param chat the chat string to remove
 * @see add_achat_string()
 * @see load_chat()
 * @see load_achat()
 */
void remove_achat_string( mixed chat ) {
    int i;

    if( !a_chats )
        return;

    if( pointerp( chat ) ) {
        map( chat, (: remove_achat_string($1) :) );
    } else if( ( i = member_array( chat, a_chats->chats ) ) != -1 ) {
        a_chats->total_w -= a_chats->weight[i];
        a_chats->weight = delete( a_chats->weight, i, 1 );
        a_chats->chats = delete( a_chats->chats, i, 1 );
    }

}  /* remove_achat_string() */

/**
 * This method adds a combat action to the NPC.  This is an action which
 * has a chance of occuring during combat.  The name is an identifier
 * which can be used to remove the action with later.  The action
 * itself can be a string, then that command will be executed.  If
 * the action is a function pointer then it will be evaluated with
 * two arguments, the first being the attacker, the second being the
 * target.
 * <p>
 * If the action is an array, if it is one element then the function
 * specified will be called on the attacked with the same arguments
 * as above.  If the size of the array is two then the function will
 * be called on the specified object with the arguments as above.
 * @see remove_combat_action()
 * @see query_combat_actions()
 * @see /std/effects/fighting/combat.c
 */
void add_combat_action( int chance, string name, mixed action ) {
    int i;

    if( !combat_actions )
        combat_actions = ({ 0 });

    if( ( i = member_array( name, combat_actions ) ) == -1 ) {
        combat_actions += ({ chance, name, action });
        combat_actions[ 0 ] += chance;
    } else {
        combat_actions[ 0 ] += chance - combat_actions[ i - 1 ];
        combat_actions[ i - 1 ] = chance;
        combat_actions[ i + 1 ] = action;
    }

} /* add_combat_action() */

/**
 * This method will remove the combat action with the specified name.
 * @return 1 if it is found and removed, 0 if not
 * @see add_combat_action()
 * @see query_combat_actions()
 * @see /std/effects/fighting/combat.c
 */
int remove_combat_action( string name ) {
    int i;

    if( !combat_actions )
        return 0;

    if( ( i = member_array( name, combat_actions ) ) == -1 )
        return 0;

    combat_actions[ 0 ] -= combat_actions[ i - 1 ];
    combat_actions = delete( combat_actions, i - 1, 3 );

    return 1;

} /* remove_combat_action() */

/**
 * This method returns the current array of combat actions on the
 * NPC.
 * <p>
 * The array will have the format of:
 * <pre>
 *    ({
 *        total_chance,
 *        action1_chance,
 *        action1_name,
 *        action1_action,
 *        ...
 *     })
 * </pre>
 * @return the combat action array
 * @see add_combat_action()
 * @see remove_combat_actions()
 * @see /std/effects/fighting/combat.c
 */
mixed query_combat_actions() { return copy( combat_actions ) || ({ 0 }); }

/**
 * This method returns the amount of death experiecne that would be
 * gained by killing the NPC.
 * @return the amount of death experience for the npc
 */
int query_death_xp() {
    int amount;

    if( TO->query_property("dead") || TO->query_property("unique") )
        return 0;

    if( death_xp_set )
        return death_xp;

    amount = (int)TOP_TEN_HANDLER->calculate_rating( TO );
    amount += (int)TO->query_max_hp();

    return amount / 4;

} /* query_death_xp() */

/**
 * This method sets the amount of death experience that would be
 * gained by killing the NPC.
 */
int set_death_xp( int amount ) {
    death_xp = amount;
    death_xp_set = 1;
} /* set_death_xp() */

/**
 * This event is called when a fight is in progress.  It will
 * be used for things like joining into currently running
 * fights and initiating combat with spell casters.
 * @param me the person initiating the attack
 * @param him the person being attacked
 */
void event_fight_in_progress( object me, object him ) {
    if( !me || ! him )
        return;

    if( him == TO && TO->query_property("see_caster") && !userp(me) &&
        random( him->query_property("see_caster") ) < him->query_int() )
        TO->attack_ob( me->query_owner() );

    if( !join_fight_mess || him == TO )
        return;

    // Only attack NPCs if join_fight_type is not 0.
    if( !join_fight_type && !interactive( him ) )
        return;

    if( member_array( him, (object *)TO->query_attacker_list() ) == -1 &&
        !him->query_property("no attack") ) {
        if( join_fight_mess )
            expand_mon_string( join_fight_mess );
        TO->attack_ob( him );
    }
} /* event_fight_in_progress() */

/** @ignore yes */
object *query_ignoring( object *people ) { return ({ }); }

/** @ignore yes */
object *query_ignored_by( object *people ) { return ({ }); }

/** @ignore yes */
mixed stats() {
    mixed zones;
    int i;

    zones = allocate( i = sizeof(zones) );

    while( i-- )
        zones[i] = ({"move zone "+i, move_zones[i] });

    if( !query_move_after() ) {
        return living::stats() + ({
          ({ "death xp", query_death_xp() }),
          ({ "aggressive", query_aggressive() }),
          ({ "join fights", query_join_fights() }),
          ({ "race", query_race() }),
          ({ "race ob", query_race_ob() }),
          ({ "level", query_level() }),
          ({ "follow speed", query_follow_speed() }),
          ({ "home", query_home_location() }),
          ({ "chats off", query_chats_off() }),
          ({ "chat chance", query_chat_chance() }),
          ({ "achat chance", query_achat_chance() }),
          ({ "route", ( sizeof( following_route ) ?
                        implode( following_route, ", ") :
                        "not going anywhere" ) }),
          }) + zones;
    }
    return living::stats() + ({
      ({ "death xp", query_death_xp() }),
      ({ "aggressive", query_aggressive() }),
      ({ "join fights", query_join_fights() }),
      ({ "race", query_race() }),
      ({ "race ob", query_race_ob() }),
      ({ "level", query_level() }),
      ({ "follow speed", query_follow_speed() }),
      ({ "home", query_home_location() }),
      ({ "chats off", query_chats_off() }),
      ({ "chat chance", query_chat_chance() }),
      ({ "achat chance", query_achat_chance() }),
      ({ "move after-fix", query_move_after()[0] }),
      ({ "move after-rand", query_move_after()[1] }),
      ({ "route", ( sizeof( following_route ) ?
                    implode( following_route, ", " ) :
                    "not following anyone") }),
      }) + zones;

} /* stats() */

#if !efun_defined(add_action)
/** @ignore yes */
protected string _process_input( string str ) {
    object ob;

    if( !str || str == "" )
        return 0;

    ob = TP;
    set_this_player(TO);
    _notify_fail(0);

    if( !sizeof( explode( str, " " ) - ({ "", 0 }) ) || str[0] == ',' ) {
        set_this_player(ob);
        return 0;
    }

    if( !TO->drunk_check(str) && !TO->exit_command(str) && !TO->cmdAll(str) &&
        !TO->new_parser(str) && !TO->lower_check(str) ) {
        set_this_player(ob);
        return 0;
    }

    set_this_player(ob);
    return "bing";

} /* _process_input() */
#endif
