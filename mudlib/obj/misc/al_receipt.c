/*
 * The receipt given out for missing items.
 * Modified by Jeremy, 22-Feb-97
 *    - added "kiss" command and associated changes
 */

#define CLONER "/global/player/cloner"

inherit OBJECT_OBJ;

int timestamp;
string obname, obpath, virt_name;
mixed static_save, dynamic_save;

void setup() {
    set_name("receipt");
    set_short( "missing item receipt" );
    obname = "unknown object";
    obpath = "***";
    set_value(5);
    set_weight(1);
    add_property("writeable", 1 );
    add_property("sale_value", 1 );
    add_property("paper", 1 );
    add_property("nosteal", 1 );
    set_max_size(1000);
    timestamp = time();
} /* setup() */

void init() {
    add_command("kiss");
} /* init() */

void set_obname(string s) { obname = s; }
void set_object(string s) { obpath = s; }
void set_virtobname(string s) { virt_name = s; }
void set_dynamic_save(mixed m) { dynamic_save = m; }

void set_static_save( mixed m ) {
    mixed p;

    static_save = m;

    if( obname != "unknown object" )
        return;

    // Try to find the short in here somewhere
    p = m[0];
    while( mapp(p) ) {
        if( !undefinedp(p["short"]) ) {
            obname = p["short"];
            return;
        }
        p = p["::"];
    }

} /* set_static_save() */

void being_sold() {
    call_out("dest_me", 4 );
} /* being_sold() */

string long( string str, int light ) {
   return "This is a receipt for your missing " + obname + ".  You can either "
      "sell it for something which may be close to replacement value (and may not), "
      "or give it to a creator and ask very nicely to have it replaced.\n"
      "You can also try kissing it - who knows, you may get lucky.\n";
} /* long() */

mapping query_dynamic_auto_load() {
   return ([ "::" : ::query_dynamic_auto_load(),
             "obname" : obname,
             "obpath" : obpath,
             "obvalue" : value,
             "virt" : virt_name,
             "timestamp" : timestamp,
             "dynamic" : dynamic_save,
             "static" : static_save ]);
} /* query_dynamic_auto_load() */

void init_dynamic_arg(mapping arg) {
   obname = arg["obname"];
   if( stringp( obname ) )
       add_adjective( explode( lower_case( obname ), " " ) );
   obpath = arg["obpath"];
   set_value(arg["obvalue"]);
   virt_name = arg["virt"];
   timestamp = arg["timestamp"];
   static_save = arg["static"];
   dynamic_save = arg["dynamic"];
   ::init_dynamic_arg(arg["::"]);
} /* init_dynamic_auto_load() */

string query_obname() { return obname; }
string query_obpath() { return obpath; }
int query_timestamp() { return timestamp; }
string query_virt_obname() { return virt_name; }
mixed query_dynamic_save() { return dynamic_save; }
mixed query_static_save() { return static_save; }

mixed stats() {
    mixed stuff;

    stuff = ::stats() + ({
        ({ "obname", obname }),
        ({ "obpath", obpath }),
        ({ "timestamp", timestamp + " (" + ctime(timestamp) + ")" }),
        });

    if( virt_name )
        stuff += ({ ({ "virtname", virt_name }) });

    return stuff;

} /* stats() */

int do_kiss() {
    object thing;

    if( sizeof(static_save) < 2 ) {
        if( creatorp(TP) ) {
            write("This receipt has no auto-load information, possibly "
                  "because the object was broken when the player logged "
                  "out.  It will have to be replaced by hand.\n");
        } else {
            write("Sorry, but this object is too broken to repair with a kiss.  "
                  "You'll have to contact a creator.\n");
        }
        return 1;
    }

    if( query_cloned_by() != TP->query_name() && !creatorp(TP) ) {
        write("The receipt whispers: I don't belong to you!\n");
        return 1;
    }

    if( !catch( thing = (object)CLONER->clone( obpath ) ) ) {
        if( thing ) {
            thing->init_static_arg( static_save[0] );
            thing->init_dynamic_arg( static_save[1] );
            if( !thing->move( ENV(TO) ) || !thing->move( ENV(ENV(TO)) ) ) {
                write( the_short()+" suddenly transforms itself into " +
                    thing->a_short() + ".\n");
                tell_room( ENV(TP), TP->the_short()+" kisses a piece of paper, "
                    "which suddenly transforms into "+thing->a_short()+".\n",
                    ({ TP }));
                add_succeeded_mess("", ({ }) );
                move("/room/rubbish");
            } else {
                write("The receipt whispers: I'm going to need some creator "
                    "help.\n");
                thing->dest_me();
            }
        } else {
            write("The receipt whispers: Sorry, I'm still broken.  "
                  "Try again some other time, or contact a creator.\n");
        }
    } else {
        write("The receipt whispers: Sorry, I'm still broken.  "
              "Try again some other time, or contact a creator.\n");
    }

    return 1;

} /* do_kiss() */
