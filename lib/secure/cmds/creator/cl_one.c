/* clone command, trial out by Turrican for a commands daemon. */

#include <creator.h>

inherit COMMAND_BASE;

/** @ignore yes */
int cmd( string str ) {
    object ob;
    string err, *filenames, obvarname;
    int mov;

    if( !str )
        return notify_fail("Clone what?\n");

    obvarname = (string)0;
    sscanf( str, "%s=%s", obvarname, str );

    filenames = TP->get_cfiles(str);
    if( !sizeof(filenames) )
        return notify_fail("No such file.\n");

    filenames = filter( filenames, (: file_exists($1) ||
                                      file_exists($1+".c") :) );

    if( !sizeof( filenames ) )
        return notify_fail("No such file(s).\n");

    foreach( str in filenames ) {
        ob = clone_object(str);
        if( obvarname )
            TP->set_obvar( obvarname, ob );
        if( ob ) {
            err = catch( ( mov = (int)ob->move(TP) ) );
            TP->handle_error( err, "move(TP)");

            if( err || mov ) {
                err = catch( ob->move( ENV(TP)) );
                TP->handle_error( err, "move(ENV(TP))");
            }

            printf("Ok.  Object %O cloned and put in "+
                ( ENV(ob) == TP ? "you" : ( ENV(ob) == ENV(TP) ? "here" :
                WIZ_PRESENT->desc_object(ENV(ob)) ) )+".\n", ob );
            say( (string)TP->the_short()+" fetches "+
                (string)ob->a_short()+" from another dimension.\n");
        } else {
            printf("Failed to clone.\n");
        }
    }

    return 1;

} /* cmd() */
