/**
 * Keeps track of all the current playtesters.  This keeps track of all
 * the current playtesters.
 * @change Moved the routes code to here from badges - Sandoz, May 2002.
 * @change Added full support for submitting, accepting and rejecting
 * applications - Sandoz, May 2002.
 */

#define LORD(x)     CAP((string)DOMAIN_H->query_leaders(explode(x,"/")[1])[0])
#define REJECT_MAIL "/d/playtesters/REJECT_APP"
#define BADGE       "/d/playtesters/items/badge"
#define SAVE_FILE   "/save/playtesters"
#define LEVEL       0

/**
 * This the class is used to store playtester application data.
 * @member name the name of the applicant
 * @member why why they want to apply
 * @member areas the areas of the mud they would like to see developed
 * @member vouches their vouches
 * @member muds the muds they have played at
 * @member time the time they applied at
 */
class application_data {
    string name;
    string why;
    string areas;
    string vouches;
    string muds;
    int time;
}

int count, accept_apps;
string date, *months;
mapping playtesters, routes, applications;

/** @ignore yes */
void create() {
    string today, *bits;

    seteuid( (string)master()->creator_file( file_name(TO) ) );

    if( file_size( SAVE_FILE+".o" ) > 0 )
        unguarded( (: restore_object, SAVE_FILE :) );

    if( !months )
        months = ({ });
    if( !playtesters )
        playtesters = ([ ]);
    if( !routes )
        routes = ([ ]);
    if( !applications )
        applications = ([ ]);

    bits = explode( ctime( time() ), " ") - ({""});
    today = bits[ 1 ]+" "+bits[ 4 ];

    if( !date ) {
        months = ({ today });
        date = today;
    }

    if( date != today ) {
        call_out("check_playtesters", 5, date );
        months += ({ today });
        date = today;
    } else
        call_out("check_playtesters", 5 );

} /* create() */

/** @ignore yes */
void save_file() { unguarded( (: save_object, SAVE_FILE :) ); }

/**
 * Returns the raw playtester data for processing!
 */
mapping query_pt_data( string name ) { return playtesters[ name ]; }

/** @ignore yes */
protected int get_age( string name ) {
    int fu;

    fu = PLAYER_H->test_age(name);

    if( fu < 0 )
        fu = -fu;

    return fu;

} /* get_age() */

/**
 * This method queries whether or not the specified person is a
 * lord of the playtesters domain or not.
 * @param who the name or object of the player to test
 * @return 1 if we are a lord of playtesting, 0 if not
 */
int query_is_lord( mixed who ) {
    if( objectp(who) )
        who = who->query_name();

    if( stringp(who) )
        return DOMAIN_H->query_leader("playtesters", who );

    return 0;

} /* query_is_lord() */

/**
 * This method sets whether or not we are accepting applications.
 * @param i 1 to start accepting apps, 0 to stop
 */
void set_accept_applications( int i ) {
    if( !adminp(TP) && !query_is_lord(TP) ) {
        write("Sorry, only lords of playtesting can do that.\n");
        return;
    }

    accept_apps = i;
    save_file();

} /* set_accept_applications() */

/**
 * This method queries whether or not we are currently
 * accepting applications.
 * @return 1 if we are accepting applications, 0 if not
 */
int query_accept_applications() { return accept_apps; }

/**
 * This method returns a list of the current playtesters.  This is
 * a list of string names in an array.
 * @return the list of current playtesters
 */
string *query_playtesters() { return keys( playtesters ); }

/**
 * This method tests to see if the given name is a playtester.
 * @return 1 if they are a playtester, 0 if they are not.
 * @see add_playtester()
 * @see remove_playtester()
 * @see query_senior_playtester()
 */
int query_playtester( string name ) {
    return !undefinedp( playtesters[ name ] );
} /* query_playtester() */

/**
 * This method determines if they are a senior playtester.
 * @return 1 if they are a senior playteser, 0 if they are not
 * @see add_senior_playtester()
 * @see remove_senior_playtester()
 * @see query_playtester()
 */
int query_senior_playtester( string name ) {
    if( !playtesters[ name ] )
        return 0;
    return playtesters[ name ][ "senior" ];
} /* query_senior_playtester() */

/**
 * This method creates a PT badge and moves it to the target player.
 * @param player the player to create a badge for
 * @return 1 if successful, 0 if not
 */
int create_badge( string player ) {
    object ob, who;

    player = lower_case(player);

    if( !playtesterp(player) ) {
        add_failed_mess( CAP(player)+" isn't a playtester, sorry.\n");
        return 0;
    }

    if( !who = find_player(player) ) {
        add_failed_mess( CAP(player)+" doesn't appear to be logged on.\n");
        return 0;
    }

    if( !ob = clone_object(BADGE) ) {
        add_failed_mess("Bugger!  Failed to clone a PT badge!\n");
        return 0;
    }

    ob->set_drop();
    ob->move(who);
    ob->reset_drop();

    write("PT badge created for "+CAP(player)+".\n");
    tell_object( who, "You suddenly find "+ob->a_short()+" in your "
        "inventory.\n");
    return 1;

} /* create_badge() */

/**
 * This method determines if they are a valid playtester or not.
 * @param name the name of the player to test
 * @return 1 if they are, 0 if they are not
 * @see reason_invalid_playtester()
 */
int valid_playtester( string name ) {
    return ( rank( name ) && !creatorp( name ) &&
             PLAYER_H->test_level( name ) >= LEVEL );
} /* valid_playtester() */

/**
 * This method adds the playtester to the array of current play
 * testers.
 * @param name the name of the player to add
 * @return 1 if successful, 0 on failure
 * @see remove_playtester()
 * @see query_playtester()
 * @see add_senior_playtester()
 * @see valid_playtester()
 */
int add_playtester( string name ) {
    name = lower_case(name);

    if( playtesters[ name ] ) {
        add_failed_mess( CAP(name)+" is already a playtester.\n");
        return 0;
    }

    if( !valid_playtester( name ) ) {
        add_failed_mess( CAP(name)+" cannot be made a playtester.\n");
        return 0;
    }

    if( classp( applications[name] ) ) {
        write( CAP(name)+" has a playtester application pending, please "
            "use the accept application option instead.\n");
        return 1;
    }

    log_file("PLAYTESTERS", ctime( time() )+": "+name+" added as a "
        "playtester by "+( TP ? TP->query_cap_name() : "Someone")+".\n");

    playtesters += ([ name : ([ "age" : get_age(name) ]) ]);
    save_file();

    write( CAP(name)+" is now a playtester.\n");

    if( find_player(name) ) {
        tell_object( find_player(name), "You have just been promoted to "
            "playtester status by "+TP->query_cap_name()+".  "
            "Congratulations.\n");
        create_badge(name);
    }

    return 1;

} /* add_playtester() */

/**
 * This method adds the player as a senior playtester.  The playtester
 * needs to already be added to become a senior playtester.
 * @param name the name of the player to add
 * @return 1 on success, 0 on failure
 * @see remove_senior_playtester()
 * @see add_playtester()
 * @see query_senior_playtester()
 */
int add_senior_playtester( string name ) {
    name = lower_case(name);

    if( !playtesters[ name ] || playtesters[ name ][ "senior" ] ) {
        add_failed_mess( CAP(name)+" is either already a senior playtester, "
            "or isn't a playtester at all.\n");
        return 0;
    }

    log_file("PLAYTESTERS", ctime( time() )+": "+name+" promoted to senior "
        "playtester by "+( TP ? TP->query_cap_name() : "Someone")+".\n");

    playtesters[ name ][ "senior" ] = 1;
    save_file();

    write( CAP(name)+" is now a senior playtester.\n");

    if( find_player(name) )
        tell_object( find_player(name), "You have just been promoted to "
            "senior playtester by "+TP->query_cap_name()+".  "
            "Congratulations.\n");

    return 1;

} /* add_senior_playtester() */

/**
 * This method removes a senior playtester.
 * @param name the name of the senior playtester to remove
 * @param reason the reason for the removal
 * @return 1 if successfully removed, 0 if not
 */
int remove_senior_playtester( string name, string reason ) {
    name = lower_case(name);

    if( !playtesters[ name ] || !playtesters[ name ][ "senior" ] ) {
        add_failed_mess( CAP(name)+" either isn't a senior playtester or, "
            "isn't a playtester at all.\n");
        return 0;
    }

    log_file("PLAYTESTERS", ctime( time() )+": "+name+" demoted from senior "
        "playtester for "+reason+" by "+( TP ? TP->query_cap_name() :
        "Someone")+".\n");

    map_delete( playtesters[ name ], "senior" );
    save_file();

    write( CAP(name)+" is no longer a senior playtester.\n");

    if( find_player(name) )
        tell_object( find_player(name), "You have just been demoted from "
            "senior playtester status by "+TP->query_cap_name()+".  "
            "Congratulations.\n");

    return 1;

} /* add_senior_playtester() */

/**
 * This method will remove the playtester from the current list of
 * play testers.
 * @param name the player to remove as a play tester
 * @param reason the reason for the removal
 * @return 0 on failure and 1 on success
 * @see add_playtester()
 * @see remove_senior_playtester()
 */
int remove_playtester( string name, string reason ) {
    string str;

    name = lower_case(name);

    if( !playtesters[ name ] ) {
        add_failed_mess( CAP(name)+" doesn't appear to be a playtester.\n");
        return 0;
    }

    str = ctime( time() )+": "+CAP(name)+" removed as a playtester by "+( TP ?
        TP->query_cap_name() : "Someone");

    if( reason )
        str += "; Reason: "+reason;

    log_file( "PLAYTESTERS", str+".\n");
    map_delete( playtesters, name );
    save_file();

    write( CAP(name)+" is no longer a playtester.\n");

    if( find_player(name) )
        tell_object( find_player(name), "You have just been demoted from "
            "playtester status by "+TP->query_cap_name()+".  "
            "Congratulations.\n");

    return 1;

} /* remove_playtester() */

/**
 * THis method returns a string reason why they are an invalid play
 * tester.
 * @param name the player to check
 * @return the string reason, 0 if none
 * @see valid_playtester()
 */
string reason_invalid_playtester( string name ) {
    if( !rank( name ) )
        return "doesn't exist";

    if( creatorp( name ) )
        return "currently a creator";

    if( PLAYER_H->test_level( name ) < LEVEL )
        return "level is too low";

    return 0;

} /* reason_invalid_playtester() */

/**
 * This goes through the list of playtesters and checks to make
 * sure they are all still valid.
 * @param last the last playtester to check
 * @see valid_playtester()
 * @see remove_playtester()
 */
void check_playtesters( string last ) {
    int age, bugs;
    string name;

    foreach( name in keys( playtesters ) )
        if( !valid_playtester(name) )
            remove_playtester( name, reason_invalid_playtester(name) );

    if( !stringp( last ) )
        return;

    /*
     * Checks to make sure their age and level are correct for them to
     * be a play tester.
     */
    foreach( name in keys( playtesters ) ) {
        bugs = playtesters[name][last];
        if( !intp( bugs ) )
            continue;
        age = get_age(name);
        playtesters[name][last] = ({ bugs, age - playtesters[name]["age"] });
        playtesters[name ]["age"] = age;
    }

    save_file();

} /* check_playtesters() */

/**
 * This is called when a playtester makes a bug report.
 * @param name the name of the playtester
 * @param type the type of report
 * @param the file the report is on
 * @param the text associated with the report
 */
void report_made( string name, string type, string file, string text ) {
    string *bits, right_now;

    if( !playtesters[ name ] )
        return;
    bits = explode( ctime( time() ), " ") - ({""});
    right_now = bits[1]+" "+bits[4];
    if( date != right_now )  {
        call_out("check_playtesters", 5, date );
        months += ({ right_now });
        date = right_now;
    }

    playtesters[ name ][ date ]++;
    count = ( count + 1 ) % 10;
    save_file();

} /* report_made() */

/**
 * This is a nice way of showing a list of the current playtesters.
 * I am not really sure what sort of results it shows, some comments
 * could be nice in here.
 */
string query_show_list() {
    string month, name, result;

    result = "                 ";
    foreach( month in months[ <5 .. ] )
        result += "    "+ month;

    result += "\n";

    foreach( name in sort_array( keys( playtesters ), 1 ) ) {
        result += ( playtesters[ name ][ "senior" ] ? "S " : "  ");
        result += sprintf( "%-15s", name );
        foreach( month in months[ <5 .. ] ) {
            if( month == date ) {
                result += sprintf("    % 3d % 4d",
                    playtesters[ name ][ month ], ( get_age( name ) -
                    playtesters[ name ][ "age" ] ) / 3600 );
                continue;
            }
            if( !playtesters[ name ][ month ] )
                result += "     --  ---";
            else
                result += sprintf( "    % 3d % 4d",
                    playtesters[ name ][ month ][ 0 ],
                    playtesters[ name ][ month ][ 1 ] / 3600 );
        }
        result += "\n";
    }

    return result;

} /* show_list() */

/*********************** Here starts the routes code. ***********************/

/**
 * The method returns the raw routes mapping.
 */
mapping query_routes() { return routes; }

/**
 * This method adds a transit route into the handler,
 * that PTs can use with their badges.
 * @param start the location this route can be used from
 * @param dest the file name of the destination of the route
 * @return 1 if the destination room exists, 0 if not
 */
int add_route( string start, string dest ) {
    object ob;

    if( !start || start == "") {
        add_failed_mess("You need to enter a proper starting location for "
            "the route.\n");
        return 0;
    }

    if( !dest || dest == "") {
        add_failed_mess("You need to enter a proper destination for the "
            "route.\n");
        return 0;
    }

    if( sizeof(routes[start]) && member_array( dest, routes[start] ) != -1 ) {
        add_failed_mess("A route from "+start+" to "+dest+" already "
            "exists.\n");
        return 0;
    }

    catch( ob = load_object(dest) );

    if( !ob ) {
        add_failed_mess("Destination room either doesn't exist, or doesn't "
            "load.\n");
        return 0;
    }

    if( routes[start] )
        routes[start] += ({ dest });
    else
        routes[start] = ({ dest });

    save_file();

    write("Successfully added route from "+start+" to "+dest+".\n");
    return 1;

} /* add_route() */

/**
 * This method removes a transit route from the handler.
 * @param start the starting location of the route to remove
 * @param dest the destination file name of the route to remove
 * @return 1 if successfully removed, 0 if destination didn't exist
 */
int remove_route( string start, string dest ) {
    if( !routes[start] ) {
        add_failed_mess("There are no routes starting from "+start+".\n");
        return 0;
    }

    if( member_array( dest, routes[start] ) == -1 ) {
        add_failed_mess("There are no routes from "+start+" to "+dest+".\n");
        return 0;
    }

    routes[start] -= ({ dest });

    if( !sizeof(routes[start]) )
        map_delete( routes, start );

    save_file();

    write("Successfully removed the route from "+start+" to "+dest+".\n");
    return 1;

} /* remove_route() */

/**
 * This method prints the routes that can be used from TP's current
 * location to TP.  This is used by the PT badge.
 */
int list_destinations() {
    int i;
    object there;
    string here, where, *dest, *theres;

    if( !playtesterp(TP) ) {
        add_failed_mess("Sorry, only playtesters can do that.\n");
        return 0;
    }

    if( !sizeof( routes ) ) {
        add_failed_mess("There are no transit points at the moment.\n");
        return 0;
    }

    theres = ({ });
    here = file_name( ENV(TP) );

    foreach( where, dest in routes ) {
        if( here[0..sizeof(where) - 1] == where )
            theres += dest;
    }

    theres -= ({ here });

    if( !sizeof ( theres ) ) {
        add_failed_mess("You cannot use the badge to move from here.\n");
        return 0;
    }

    here = "From here you can use the badge to move to:\n";

    foreach( where in theres ) {
        if( !( there = find_object( where ) ) ) {
            if( !file_exists( where+".c") ) {
                here += sprintf("%c: %s cannot be found; please contact %s.\n",
                    65 + i++, where, LORD(where) );
                continue;
            }
            catch( there = load_object(where) );
            if( !there ) {
                here += sprintf("%c: %s will not load, please contact %s.\n",
                    65 + i++, where, LORD(where) );
                continue;
            }
        }

        if( where[0..2] == "/d/" || where[0..2] == "/p/") {
            here += sprintf("%c: %s (%s)\n", 65 + i++, there->a_short(),
                CAP( explode( where, "/")[1] ) );
        } else {
            here += sprintf("%c: %s (%s)\n", 65 + i++, there->a_short() );
        }
    }

    write(here);
    return 1;

} /* list_destinations() */

/**
 * This method prints all routes to this_player().
 */
int list_transits() {
    string *theres, start, ret;
    object there;

    if( !playtesterp(TP) ) {
        add_failed_mess("Sorry, only playtesters can do that.\n");
        return 0;
    }

    if( !sizeof( routes ) ) {
        add_failed_mess("There are no transit points at the moment.\n");
        return 0;
    }

    theres = keys( routes );
    ret = "You can use the badge to move from:\n";

    foreach( start in theres ) {
        if( !( there = find_object( start ) ) ) {
            if( file_size( start ) == -2 ) {
                string pre;

                switch( start ) {
                    case "/d":
                    case "/d/":
                      pre = "   Any domain room to:\n        ";
                    break;
                    case "/p":
                    case "/p/":
                      pre = "   Any project room to:\n        ";
                    break;
                    default:
                      pre = "   Anywhere in "+start+" to:\n        ";
                }

                ret += pre + implode( map( routes[start], (: $1->a_short() +
                    ( $1[0..2] == "/d/" || $1[0..2] == "/p/" ? " ("+CAP(
                    explode( $1, "/")[1] )+")" : "") :) ), "\n        ")+"\n";
                continue;
            } else if( !file_exists( start+".c") ) {
                ret += sprintf("%s cannot be found; please contact %s.\n",
                    start, LORD(start) );
            }
            catch( there = load_object(start) );
            if( !there ) {
                ret += sprintf("%s will not load, please contact %s.\n",
                    start, LORD(start) );
                continue;
            }
        }
        ret += sprintf("   $C$%s to:\n        ", there->a_short() +
            ( start[0..2] == "/d/" || start[0..2] == "/p/" ? " ("+CAP(
            explode( start, "/")[1] )+")" : "") )+
            implode( map( routes[start], (: $1->a_short() +
            ( $1[0..2] == "/d/" || $1[0..2] == "/p/" ? " ("+CAP(
            explode( $1, "/")[1] )+")" : "") :) ), "\n        ")+"\n";
    }

    write(ret);
    return 1;

} /* list_transits() */

/**
 * This method is used by the PT badge to move PTs from
 * one location to another.
 * @param destination the destination to go to
 */
int do_goto( string destination ) {
    int i;
    string here, where, *theres, *dest;
    object there;

    if( !playtesterp(TP) ) {
        add_failed_mess("Sorry, only playtesters can do that.\n");
        return 0;
    }

    if( !sizeof( routes ) ) {
        add_failed_mess("There are no transit points at the moment.\n");
        return 0;
    }

    if( TP->query_fighting() ) {
        add_failed_mess("You cannot use goto while fighting.\n");
        return 0;
    }

    i = destination[ 0 ] - 65;

    if( i < 0 || i > 25 ) {
        add_failed_mess("The destination label needs to be a capital letter "+
            "between A and Z.\n" );
        return 0;
    }

    here = file_name( ENV(TP) );
    theres = ({ });

    foreach( where, dest in routes )
        if( here[0..sizeof(where) - 1] == where )
            theres += dest;

    theres -= ({ here });

    if( !sizeof( theres ) ) {
        add_failed_mess("You cannot use the badge to move from here.\n");
        return 0;
    }

    if( i >= sizeof( theres ) ) {
        add_failed_mess("That is not a valid destination label from here.\n");
        return 0;
    }

    where = theres[i];
    there = find_object( where );

    if( !there ) {
        if( !file_exists( where+".c" ) ) {
            write( where+" cannot be found; please contact "+LORD(where)+
                ".\n");
            return 1;
        }
        catch( there = load_object(where) );
        if( !there ) {
            write( where+" will not load; please contact "+LORD(where)+".\n");
            return 1;
        }
    }

    write("Moving you to "+(string)there->the_short()+"...\n");
    TP->move_with_look( there,
        "$N appear$s in a gout of green fire.",
        "$N disappear$s in a puff of yellow smoke.");
    return 1;

} /* do_goto() */

/**
 * This method is used by the PT badge and gives all the transit
 * commands to this_player().
 * @param badge the badge that is giving out the commands
 */
void do_init( object badge ) {
    if( !badge || !TP )
        return;

    if( !playtesterp(TP) ) {
        write("You are not a playtester.  Your blue playtester's badge "
            "disappears.\n");
        badge->set_drop();
        badge->move("/room/rubbish");
        return;
    }

    add_command("destinations", "", (: list_destinations() :) );
    add_command("transits", "", (: list_transits() :) );
    add_command("goto", "<string>", (: do_goto($4[0]) :) );

} /* do_init() */

/**
 * This method prints all routes to TP, if TP is a creator.
 */
int list_routes() {
    string *theres, start, ret;
    object there;

    if( !creatorp(TP) ) {
        add_failed_mess("Sorry, only creators can do that.\n");
        return 0;
    }

    if( !sizeof( routes ) ) {
        add_failed_mess("There are no routes at the moment.\n");
        return 0;
    }

    theres = keys( routes );
    ret = "The current routes are:\n";

    foreach( start in theres ) {
        if( !( there = find_object( start ) ) ) {
            if( file_size( start ) == -2 ) {
                ret += sprintf("   From anywhere in %s to:\n        ", start )+
                    implode( map( routes[start], (:
                    $1->a_short()+" ("+$1+")" :) ), "\n        ")+"\n";
                continue;
            } else if( !file_exists( start+".c") ) {
                ret += sprintf("%s cannot be found; please contact %s.\n",
                    start, LORD(start) );
            }
            catch( there = load_object(start) );
            if( !there ) {
                ret += sprintf("%s will not load, please contact %s.\n",
                    start, LORD(start) );
                continue;
            }
        }

        ret += "   From "+start+" to:\n";

        foreach( string place in routes[start] ) {
            catch( there = load_object(place) );
            if( !there )
                ret += "        BROKEN LOCATION ("+place+")\n";
            else
                ret += "        "+there->a_short()+" ("+place+")\n";
        }
    }

    write(ret);
    return 1;

} /* list_routes() */

/******************* Here starts the PT application code. *******************/

/**
 * This method makes a nice string out of the PT application data.
 * @param data the class to process
 * @param cols the cols to wrap at
 * @param flag if flag is set, the line about whose app
 * it is will not be added
 * @return the application in a nice string format
 */
protected varargs string compile_app( class application_data data, int cols,
                                      int flag ) {
    string ret;

    if( !cols )
        cols = 79;

    ret = "";

    if( !flag )
        ret += "Application by "+CAP(data->name)+" submitted on "+
            ctime(data->time)+".\n";

    ret += " * Why I should be hired : "+
        indent( sprintf("%22' '+=s", "")+data->why, 6, cols )[28..]+"\n";

    ret += " * Areas I wish to see developed : "+
        indent( sprintf("%30' '+=s", "")+data->areas, 6, cols )[36..]+"\n";

    ret += " * Vouches : "+indent( sprintf("%8' '+=s", "")+
        data->vouches, 6, cols )[14..]+"\n";

    if( data->muds )
        ret += " * Muds I've played on : "+indent( sprintf("%20' '+=s", "")+
            data->muds, 6, cols )[26..]+"\n";

    return ret;

} /* compile_app() */

/** @ignore yes */
void do_application_notify( object who ) {
    int i;

    if( !who || !query_is_lord(who) )
        return;

    if( i = sizeof(applications) ) {
        string *names;
        names = asort( map( keys(applications), (: CAP($1) :) ) );
        tell_object( who, "%^YELLOW%^Playtester application"+( i > 1 ?
            "s" : "")+" by "+query_multiple_short(names)+" "+( i > 1 ?
            "are" : "is")+" currently undecided.%^RESET%^\n");
    }

} /* do_application_notify() */

/** @ignore yes */
private void inform_lords( string str ) {
    if( str && str != "")
        map( filter( users(), (: query_is_lord($1) :) ),
            (: tell_object( $1, $2 ) :), str );
} /* inform_lords() */

/** @ignore yes */
void submit_app( string str, class application_data data ) {
    if( str[0] != 'y' && str[0] != 'Y' ) {
        write("You have chosen other than yes, aborting...\n");
        return;
    }

    data->time = time();
    applications[TP->query_name()] = data;
    save_file();

    write("\nSubmitting your application, thank you for your time.\n"
        "You will receive a response inside two weeks, most likely within "
        "five days.\n");

    inform_lords("%^YELLOW%^"+TP->query_cap_name()+" has submitted an "
        "application for a playtester position.%^RESET%^\n");

} /* submit_app() */

/** @ignore yes */
void do_muds( string str, class application_data data ) {
    if( str && str != "")
        data->muds = str;

    write("\nYour application reads as follows:\n"+
        compile_app( data, TP->query_cols(), 1 )+"\n"
        "Do you wish to submit the application? [y|n] > ");

    input_to( (: submit_app :), data );

} /* do_muds() */

/** @ignore yes */
void do_vouch( string str, class application_data data ) {
    string *bits, *bad;
    int i;

    if( !str || str == "") {
        write("Sorry, you must enter at least something.  Aborting...\n");
        return;
    }

    if( str == "q" || str == "Q") {
        tell_object( TP, "Aborting...\n");
        return;
    }

    str = lower_case(str);
    bits = explode( implode( explode( str, " ") - ({ 0, "" }), "" ), ",");

    if( sizeof( bits ) < 2 ) {
        write("\nSorry, you must enter at least two names.\n\n"
            "Please try again, or type 'q' to quit : ");
        input_to( (: do_vouch :), data );
        return;
    }

    bad = map( filter( bits, (: !rank($1) :) ),
        (: CAP($1) :) );
    if( i = sizeof( bad ) ) {
        write("\nSorry, but "+query_multiple_short( asort(bad) )+" "+
            ( i > 1 ? "don't" : "doesn't")+" appear to have "+
            ( i > 1 ? "characters" : "a character")+" on "+mud_name()+".\n\n"
            "Please re-enter the names, or type 'q' to quit : ");
        input_to( (: do_vouch :), data );
        return;
    }

    bad = map( filter( bits, (: query_is_lord($1) :) ), (: CAP($1) :) );
    if( i = sizeof( bad ) ) {
        write("\nSorry, but "+query_multiple_short( asort(bad) )+" "+
            ( i > 1 ? "are lords" : "is a lord")+" of the playtesters "
            "domain, and therefore cannot vouch for you.\n\n"
            "Please re-enter the names, or type 'q' to quit : ");
        input_to( (: do_vouch :), data );
        return;
    }

    data->vouches = query_multiple_short( map( bits, (: CAP($1) :) ) );
    write("\nThis final question is optional.  Please list any other "
        "muds you've played on, and what names you played under.\n");
    TP->do_edit( 0, (: do_muds :), 0, 0, data );

} /* do_vouch() */

/** @ignore yes */
void do_areas( string str, class application_data data ) {
    if( !str || str == "") {
        write("Sorry, you must enter at least something.  Aborting...\n");
        return;
    }

    data->areas = str;
    write("\nPlease list up to three people who have agreed to vouch for "
        "you.\nList all three on one line, separated by commas : ");
    input_to( (: do_vouch :), data );

} /* do_areas() */

/** @ignore yes */
void do_why( string str ) {
    class application_data data;

    if( !str || str == "") {
        write("Sorry, you must enter at least something.  Aborting...\n");
        return;
    }

    data = new( class application_data );
    data->name = TP->query_name();
    data->why = str;

    write("\nWhich parts of the mud are you most interested in seeing "
        "developed?\n");
    TP->do_edit( 0, (: do_areas :), 0, 0, data );

} /* do_why() */

/** @ignore yes */
void new_application( string str ) {
    if( str[0] != 'y' && str[0] != 'Y' ) {
        write("You have chosen other than yes, aborting...\n");
        return;
    }

    write("\nPlease describe in 100 words or less what you would bring to "
        "the playtesting community.\n");
    TP->do_edit( 0, (: do_why :) );

} /* new_application() */

/** @ignore yes */
int do_apply() {
    if( !query_accept_applications()  ) {
        write( "The playtesters domain is not currently accepting "
            "applications, sorry.\n");
        return 1;
    }

    if( !valid_playtester( TP->query_name() ) ) {
        write("Sorry, you cannot become a playtester right now, because you "
            "do not pass the requirements for becoming one.\n");
        return 1;
    }

    if( playtesterp(TP) ) {
        write("You are already a playtester, silly!\n");
        return 1;
    }

    if( applications[TP->query_name()] ) {
        write("You have already submitted an application, and it is being "
            "reviewed.\n");
        return 1;
    }

    write("Are you certain you wish to apply to become a playtester? "
        "[y|n] > ");
    input_to( (: new_application :) );
    return 1;

} /* do_apply() */

/**
 * This method prints the list of pending applications.
 */
int do_list_apps() {
    string ret, name, *names;

    if( !adminp(TP) && !query_is_lord(TP) ) {
        write("Sorry, but only lords of playtesting can view "
            "playtester applications.\n");
        return 1;
    }

    if( !sizeof(applications) ) {
        write("There are currently no playtester applications pending.\n");
        return 1;
    }

    ret = "";
    names = asort( keys(applications) );

    foreach( name in names )
        ret += " * "+CAP(applications[name]->name)+" (submitted on "+
            ctime(applications[name]->time)+")\n";

    TP->more_string("The following application"+( sizeof(applications) > 1 ?
        "s are" : " is")+" currently undecided:\n"+ret, "applications");
    return 1;

} /* do_list_apps() */

/**
 * This method prints the specified player's playtester application.
 * @param name the name of the player to get the application for
 */
int do_show_app( string name ) {
    if( !adminp(TP) && !query_is_lord(TP) ) {
        write("Sorry, but only lords of playtesting can view "
            "playtester applications.\n");
        return 1;
    }

    if( !sizeof(applications) ) {
        write("There are currently no playtester applications pending.\n");
        return 1;
    }

    if( !classp( applications[name] ) ) {
        write( CAP(name)+" hasn't submitted an application.\n");
        return 1;
    }

    write( compile_app( applications[name], TP->query_cols() ) );
    return 1;

} /* do_show_app() */

/**
 * This method deletes an application from the mapping, logs it,
 * and sends out the appropriate mail to the applicant.
 * @param name the name of the applicant whose application to delete
 * @param accept whether or not we accepted the application
 */
protected void log_application( string name, int accept ) {
    if( applications[name] ) {
        if( !accept && REJECT_MAIL && file_exists(REJECT_MAIL) ) {
            string mail;
            mail = read_file(REJECT_MAIL);
            mail = replace( mail, "$cre$", TP->query_cap_name() );
            MAIL_H->do_mail_message( name, TP->query_name(),
                "Your application for a playtester position.", 0, mail );
        }
        log_file("/secure/log/PT_APPS", "%s * %s by %s on %s.\n%79'-'+=s",
            compile_app( applications[name] ), ( accept ? "ACCEPTED" :
            "REJECTED"), TP->query_cap_name(), ctime(time()), "\n");
        map_delete( applications, name );
    }

    save_file();

} /* log_application() */

/**
 * This method will accept an application by the specified player.
 * @param name the name of the player whose application to accept
 */
int do_accept_app( string name ) {
    if( !query_is_lord(TP) ) {
        write("Sorry, but only lords of playtesting can accept playtester "
            "applications.\n");
        return 1;
    }

    name = lower_case(name);

    if( !classp( applications[name] ) ) {
        write( CAP(name)+" hasn't submitted an application.\n");
        return 1;
    }

    if( playtesters[ name ] ) {
        write( CAP(name)+" is already a playtester.\n");
        return 1;
    }

    if( !valid_playtester( name ) ) {
        write( CAP(name)+" cannot be made a playtester.\n");
        return 1;
    }

    log_file("PLAYTESTERS", ctime( time() )+": "+name+" added as a "
        "playtester by "+( TP ? TP->query_cap_name() : "Someone")+".\n");
    inform_lords("%^YELLOW%^"+TP->query_cap_name()+" has accepted "+
        CAP(name)+"'s playtester application.%^RESET%^\n");

    playtesters += ([ name : ([ "age" : get_age(name) ]) ]);
    // log_application() saves as well.
    log_application( name, 1 );

    write( CAP(name)+" is now a playtester.\n");

    if( find_player(name) ) {
        tell_object( find_player(name), "You have just been promoted to "
            "playtester status by "+TP->query_cap_name()+".  "
            "Congratulations.\n");
        create_badge(name);
    }

    return 1;

} /* do_accept_app() */

/**
 * This method will reject an application by the specified player.
 * @param name the name of the player whose application to reject
 */
int do_reject_app( string name ) {
    if( !query_is_lord(TP) ) {
        write("Sorry, but only lords of playtesting can reject playtester "
            "applications.\n");
        return 1;
    }

    name = lower_case(name);

    if( !classp( applications[name] ) ) {
        write( CAP(name)+" hasn't submitted an application.\n");
        return 1;
    }

    write("You have rejected "+CAP(name)+"'s application.\n");

    inform_lords("%^YELLOW%^"+TP->query_cap_name()+" has rejected "+
        CAP(name)+"'s playtester application.%^RESET%^\n");

    // log_application() saves as well.
    log_application( name, 0 );
    return 1;

} /* do_reject_app() */
