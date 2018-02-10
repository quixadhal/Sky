/*
 * Inventory checker made by Funtime to periodically check players inventories
 * 5/5/1994
 * rewritten a bit and added playtesting stuff Shaydz 5/06/2001 8:58PM
 * heavily rewritten by Olorin to actually remove the illegal objects
 * Modified so it runs independantly, starting the inv checking process in create() and calling itself out
 * Terano, 18/1/98
 */
int do_seek();

string to_log;

void dest_me() {
    destruct( TO );
} /* dest_me() */

void create() {
    seteuid( "Admin" );
    do_seek();
} /* create() */

void log_it() {
    if( sizeof( to_log ) > 0 ) {
        log_file( "ILLEGAL_OBJECT",
            "Illegal objects removed %s\n%s\n", ctime( time() ), to_log );
    }
} /* log_it() */

void check_sub_inv( object *things, string owner, object owner_ob ) {
    string *pre_filename, filename, *switcher;
    object *obs, thing;

    obs = ({ });
    foreach( thing in things ) {
        if( thing->query_property( "virtual name" ) ) {
            filename = thing->query_property( "virtual name" );
        } else {
            pre_filename = explode( file_name( thing ),"#" );
            filename = pre_filename[ 0 ];
        }

        switcher = explode( filename, "/" );

        if( switcher[0] == "w") {
            to_log += sprintf("%s in %s, cloned by %s\n",
                      filename, owner, (string)thing->query_cloned_by() );
            obs += ({ thing });
        }

        if( switcher[0] == "d" ) {
            if( switcher[1] == "qc" && !playtesterp( owner ) ) {
                to_log += sprintf("%s in %s, cloned by %s\n",
                          filename, owner, (string)thing->query_cloned_by() );
                obs += ({ thing });
            } else if( switcher[1] == "qc" && !file_exists(filename) &&
                !file_exists(filename+".c") ) {
                to_log += sprintf("%s in %s, cloned by %s\n",
                          filename, owner, (string)thing->query_cloned_by() );
                obs += ({ thing });
            }
            if( switcher[1] == "playtesters" && !playtesterp( owner ) ) {
                to_log += sprintf("%s in %s, cloned by %s\n",
                          filename, owner, (string)thing->query_cloned_by() );
                obs += ({ thing });
            }
        }

        if( sizeof( switcher ) > 2 && switcher[ 0 ] == "obj" &&
            switcher[ 1 ] == "misc" ) {
            switch( switcher[2] ) {
            case "mailer_ob":
                if( !playtesterp( owner ) &&
                    !owner->query_property("allowed mailer") ) {
                    to_log += sprintf("%s in %s, cloned by %s\n",
                           filename, owner, (string)thing->query_cloned_by() );
                    obs += ({ thing });
                }
            break;
            case "board_mas":
                to_log += sprintf("%s in %s, cloned by %s\n",
                          filename, owner, (string)thing->query_cloned_by() );
                obs += ({ thing });
            break;
            }
        }
    }

    if( sizeof( obs ) ) {
        tell_object( owner_ob, "A small official looking dwarf walks up to "
            "you and says, \"In accordance with paragraph 5, sub-paragraph "
            "7 of the Domestic & Domesticated Animals (Care & Protection) "
            "Act, 1673, I hereby remove from you: "+
            (string)query_multiple_short( obs, "one" )+"\"\n"
            "He scribbles something on a clipboard, then walks away.\n");
        obs->move("/room/rubbish");
        /* dest them soonest */
        call_out("check_obs", 7, obs );
    }

} /* check_sub_inv() */

void check_obs( object *obs ) {
    object thing;

    foreach( thing in obs )
        if( thing )
            thing->dest_me();

} /* check_obs() */

void check_inv( object player ) {
    object *things;

    things = deep_inventory( player );
    check_sub_inv( things, (string)player->query_name(), player );

} /* check_inv() */

int do_seek() {
    object user;

    to_log = "";

    foreach( user in users() ) {
        if( !user->query_property( "guest" ) &&
            !user->query_property( "no score" ) && !creatorp(user) )
            check_inv( user );
    }

    log_it();
    call_out( "do_seek", 300 ); // Every 5 minutes

    return 1;

} /* do_seek() */

int query_theft_command() { return -1; }
