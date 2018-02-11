
#include <type.h>
#include <runtime_config.h>

/**
 * This method returns the owner of a file (for error logging).
 * @param file the file to get the owner for
 * @return the owner of the file
 */
string get_wiz_name( mixed file ) {
    if( !PO )
        return "root";

    if( objectp(file) )
        file = file_name(file);

    file = explode( file, "/") - ({ "" });

    if( file[0] == "w" )
        return ( file_size("/w/" + file[1] ) != -2 ? "root" : file[1] );

    if( file[0] == "d" )
        return "dom";

    return "root";

} /* get_wiz_name() */

/** @ignore yes */
string get_dom_name( mixed file ) {
    if( objectp(file) )
        file = file_name(file);

    file = explode( file, "/") - ({""});

    if( file[0] == "d" && sizeof(file) >= 2 )
        return file[1];

} /* get_dom_name() */

/** @ignore yes */
int different( string fn, string pr ) {
    sscanf( fn, "%s#%*d", fn );
    fn += ".c";
    return ( fn != pr ) && ( fn != ( "/" + pr ) );
} /* different() */

/** @ignore yes */
string trace_line( object obj, string prog, string file, int line ) {
    string ret;
    mixed objfn;

    objfn = obj ? file_name(obj) : "<none>";
    ret = objfn;

    if( different( objfn, prog ) )
        ret += sprintf(" (%s)", prog );

    if( file != prog )
        ret += sprintf(" in %s:%d\n", file, line );
    else
        ret += sprintf(" at line %d\n", line );

    if( obj && stringp( objfn = obj->query_name() ) )
        ret += sprintf("Object name: %O\n", objfn );

    return ret;

} /* trace_line() */

/** @ignore yes */
string printable_arg( mixed arg ) {
    switch( typeof(arg) ) {
      case ARRAY:
      case MAPPING:
      case CLASS:
      case STRING:
        return "(" + typeof(arg) + ") " + save_variable(arg);
      default:
        return sprintf("(%s) %O", typeof(arg), arg);
    }
} /* printable_arg() */

/** @ignore yes */
string trace_args( mixed *args ) {
    string *tmp;
    int max;

    if( !sizeof(args) )
        return " (void)";

    max = ( (int)get_config( __MAX_STRING_LENGTH__ ) || 10000 ) / sizeof(args);

    tmp = map( args, (: printable_arg($1)[0..$(max)] :) );

    return "\n(\n    "+implode( tmp, ",\n    ")+"\n)";

} /* trace_args() */

/** @ignore yes */
string trace_locals( mixed args ) {
    string *tmp;
    int max;

    if( !sizeof(args) )
        return " none.";

    max = ( (int)get_config( __MAX_STRING_LENGTH__ ) || 10000 ) / sizeof(args);

    tmp = map( args, (: printable_arg($1)[0..$(max)] :) );

    return "\n    " + implode( tmp, ",\n    ");

} /* trace_locals() */

/** @ignore yes */
varargs string standard_trace( mapping mp, int flag ) {
    string ret;
    mixed trace;
    int i, n, max;

    ret = mp["error"] + "Object: " + trace_line( mp["object"], mp["program"],
          mp["file"], mp["line"] );
    ret += "Arguments were:" + trace_args(mp["arguments"]) + "\n"
           "Locals were:" + trace_locals(mp["locals"]) + "\n";
    ret += "\n";

    trace = mp["trace"];
    n = sizeof(trace);

    max = ( (int)get_config( __MAX_STRING_LENGTH__ ) || 10000 ) / n;

    for( i = 0; i < n; i++ ) {
        ret += sprintf("(%d) '%s' in %s", i + 1, trace[i]["function"],
               trace_line(trace[i]["object"], trace[i]["program"],
               trace[i]["file"], trace[i]["line"]));
        if( !flag ) {
            ret += "Arguments were:"+trace_args(trace[i]["arguments"])[0..max]+"\n"
                   "Locals were:"+trace_locals(trace[i]["locals"])[0..max]+"\n";
        }
    }

    return ret;

} /* trace_mapping() */

#define MAX_SIZE 50000

/** @ignore yes */
void error_handler( mapping error, int caught ) {
    string file, ret, path, name, obname;
    object ob, oName;
    string uName, cStr, verb;

    if( member_array("error_handler", call_stack(2), 1 ) != -1 )
        return;

    ret = "--------------------\n"+ctime(time()) + ":  " + standard_trace(error);

    file = ( caught ? "catch" : "runtime" );

    sscanf( error["error"], "%*sError in loading object '%s'", obname );

    if( !obname ) {
        ob = error["object"];
        obname = ( !ob ? error["program"] : file_name(ob) );
    }

    if( obname == "/secure/cmds/creator/grep")
        return;

    name = get_wiz_name(obname);

    switch( name ) {
      case "root":
        path = "/log/" + file;
      break;
      case "dom":
        path = "/d/"+get_dom_name(obname)+"/"+file;
      break;
      default:
        path = "/w/"+name+"/"+file;
        tell_creator( name, "A runtime error occured in the file %O, logged "
            "to %s.\n", obname, path );
    }

    // Check to see if the autoload stuff is involved in this error.
    if( member_array("init_dynamic_arg", call_stack(2) ) != -1 ||
        member_array("query_dynamic_auto_load", call_stack(2) ) != -1 ) {
        path = "/secure/log/auto_" + file;
    }

    if( path[0..2] != "/w/" ) {
        if( oName = find_living(obname) )
            uName = oName->query_name() || obname;
        else
            uName = obname;

        if( this_player(1) ) {
            verb = query_verb() || "no verb";
            uName = this_player(1)->query_name()+" ("+verb+")";
        }

        cStr = ( caught ? "[c]" : "" );

        if( verb != "update" && verb != "goto" )
            event( users(), "inform", sprintf("Error: %s(%s) %s",
                cStr, path, uName ), "error");
    }

    if( file_size(path) > MAX_SIZE ) {
        unguarded( (: rm, path+".old" :) );
        unguarded( (: rename, path, path+".old" :) );
    }

    unguarded( (: write_file, path, ret :) );

    if( this_player(1) && find_object("/secure/simul_efun") ) {
        this_player(1)->set_last_error(error);
        if( !caught ) {
            if( creatorp( this_player(1)) ) {
                tell_object( this_player(1),
                    error["error"]+"Object: "+trace_line(error["object"],
                    error["program"], error["file"], error["line"] )+
                    "\nTrace written to "+path+"\n");
            } else {
                tell_object( this_player(1), "A runtime error occurred.\n"
                    "Please use the \"bug\" command to report it, "
                    "describing what you tried to do when it happened.\n");
            }
        }
    }

} /* error_handler() */

/*
 * Write an error message into a log file. The error occurred in the object
 * 'file', giving the error message 'message'.
 */
void log_error( string file, string message ) {
    string name, efile, epath, colour;

    if( sscanf( message, "%*sWarning:%*s") == 2 ) {
        efile = "warnings";
        colour = "%^CYAN%^";
    } else {
        efile = "error-log";
        colour = "%^RED%^";
    }

    if( this_player(1) && creatorp(this_player(1)) )
        tell_object( this_player(1), colour+message+"%^RESET%^");

    name = get_wiz_name(file);

    switch( name ) {
      case "root":
        epath = "/log/" + efile;
        break;
      case "dom":
        epath = "/d/"+get_dom_name(file)+"/"+efile;
        break;
      default:
        epath = "/w/"+name+"/"+efile;
        break;
    }

    if( file_size(epath) > MAX_SIZE ) {
        unguarded( (: rm, epath+".old" :) );
        unguarded( (: rename, epath, epath+".old" :) );
    }

    unguarded( (: write_file, epath, message :) );

} /* log_error() */

/** @ignore yes */
void do_log( string person, string text ) {
    if( file_name(PO) != "/std/smart_log" || !dir_exists("/w/"+person ) )
        return;

    unguarded( (: rm, "/w/"+person+"/"+PLAYER_ERROR_LOG :) );
    unguarded( (: write_file, "/w/"+person+"/"+PLAYER_ERROR_LOG, text :) );

} /* do_log() */

/** @ignore yes */
void forward_error( string file, string text ) {
    if( file_name(PO) != "/secure/cmds/creator/errors" &&
        file_name(PO) != "/www/secure/errors")
        return;

    unguarded( (: write_file, file, text :) );

} /* forward_error() */
