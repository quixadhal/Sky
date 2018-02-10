/* command, trial out by Turrican for a commands daemon. */


#include <player.h>

inherit COMMAND_BASE;
/*
inherit "/cmds/speech";
*/


#define BEEP sprintf("%c",7)
#define CHAN_OK 1
#define CHAN_FORCED 2
#define CHAN_EMOTED 4
#define CHAN_QUERY 8

string _channel;

/**
 * This method sets the name of the channel this command is for.
 * @param name the name of the channel
 * @see query_channel()
 */
protected void set_channel(string name) {
    _channel = name;
} /* set_channel() */

/**
 * This method returns the name of the channel the command is currently
 * set for.
 * @return the name of the channel
 * @see set_channel()
 */
string query_channel() {
    return _channel;
} /* query_channel() */

/**
 * This method handles the command line and returns the type of command that
 * is represented by the command line.
 * @param mess the message to parse
 * @return the flags to determine what sort of message it is
 */
protected int handle_command_line(string mess) {
    int retval;
    function map_func;
    string channel;

    notify_fail("Syntax: "+query_channel()+"[@|?] [text]\n");
    if (!mess) {
        return 0;
    }

    switch (mess[0]) {
    case '!' :
        retval |= CHAN_FORCED;
        break;
    case '@' :
        retval |= CHAN_EMOTED;
        break;
    case '?' :
        if (mess == "?") {
            int i;
            object *people, *off_line;

            people = users();
            off_line = ({ });
            for (i=0;i<sizeof(people);i++) {
                switch (query_channel()) {
                case "cre?":
                case "cre":
                case "intercre":
                case "intergossip":
                case "dwcre":
                case "dwchat":
                    if (!creatorp(people[i]) ||
                      !people[i]->query_visible(this_player())) {
                        people = delete(people, i, 1);
                        i--;
                    }
                    break;
                case "lord":
                    if (!master()->query_lord( (string)people[i]->query_name() )) {
                        people = delete(people, i, 1);
                        i--;
                    }
                    break;
                default:
                    break;
                }
                if (i >= 0) {
                    switch(query_channel()) {
                    case "cre":
                    case "lord":
                        if (people[i]->check_earmuffs(query_channel())
                          /* ||
                             sizeof(people[i]->query_ignoring(({this_player()}))) */) {
                            off_line += ({ people[i] });
                            people = delete(people, i, 1);
                            i--;
                        }
                        break;
                    default:
                        if (people[i]->check_earmuffs("intermud-all")) {
                            off_line += ({ people[i] });
                            people = delete(people, i, 1);
                            i--;
                        }
                        break;
                    }
                } else if (i >= 0 && people[i]->check_earmuffs(query_channel())) {
                    off_line += ({ people[i] });
                    people = delete(people, i, 1);
                    i--;
                }
            }
            people = sort_array(people, (: strcmp($1->query_name(), $2->query_name()) :) );
            off_line = sort_array(off_line, (: strcmp($1->query_name(), $2->query_name()) :) );
            map_func = function (object ob) {
                string str;

                if (ob->query_invis()) {
                    if (ob->query_invis() == 2) {
                        str = "({" + $1->query_cap_name() + "})";
                    } else {
                        str = "(" + $1->query_cap_name() + ")";
                    }
                } else {
                    str = $1->query_cap_name();
                }
                if (query_idle(ob) > 120) {
                    int hours, mins, secs;

                    secs = query_idle(ob);
                    mins = secs / 60;
                    secs %= 60;
                    hours = mins / 60;
                    mins %= 60;
                    if ( !hours )  {
                        str += sprintf(" [%02dm]", mins);
                    } else {
                        str += sprintf(" [%02dh:%02dm]", hours, mins);
                    }
                }
                return str;
            };
            people = map(people, map_func);
            off_line = map(off_line, map_func);
            if (sizeof(people)) {
                write(sprintf( "On line: %-=" + (this_player()->query_cols() - 10) + "s", query_multiple_short( people ) +".\n"));
            }
            if (sizeof(off_line)) {
                write(sprintf( "Off line: %-=" + (this_player()->query_cols() - 11) + "s", query_multiple_short( off_line ) +".\n"));
            }
            retval |= CHAN_QUERY;
        }
        break;
    default :
        return CHAN_OK;
        break;
    }
    return retval;
} /* handler_command_line() */

