
#include <soul.h>

inherit COMMAND_BASE;

/** @ignore yes */
int cmd( string str ) {
    string *filenames, err, fname, *loaded, *failed, *succ, ret, load_err;
    object ob;
    int size;

    if( !str )
        return notify_fail("Load what?\n");

    if( !sizeof( filenames = TP->get_cfiles(str) ) )
        return notify_fail("No such file.\n");

    loaded = ({ });
    succ = ({ });
    failed = ({ });

    load_err = "";

    foreach( fname in filenames ) {
        if( !file_exists(fname) && file_exists(fname)+".c") {
            failed += ({ fname });
            continue;
        }

        if( find_object( fname ) ) {
            loaded += ({ fname });
            continue;
        }

        if( sscanf( fname, SOUL_DIR+"%s", err ) == 1 ) {
            if( err = catch( SOUL_COMPILER->compile_file("/"+err ) ) )
                load_err += sprintf("Failed to load %s, error: %s",
                    fname, err );
            else
                succ += ({ fname+" (soul)"});
        } else {
            if( err = catch( ob = load_object( fname ) ) )
                load_err += sprintf("Failed to load %s, error: %s",
                    fname, err );
            else if( !ob )
                failed += ({ fname });
            else
                succ += ({ fname });
        }
    }

    ret = "";

    if( size = sizeof(loaded) )
        ret += query_multiple_short(loaded)+" "+( size == 1 ? "is" :
            "are")+" already loaded.\n";

    if( size = sizeof(failed) )
        ret += "The file"+( size == 1 ? " " : "s ")+
            query_multiple_short(failed)+" cannot be compiled.\n";

    if( strlen(load_err) )
        ret += load_err;

    if( sizeof(succ) )
        ret += sprintf("$I$5=$C$Loaded %s.\n", query_multiple_short(succ) );

    TP->show_message("$P$Load$P$"+ret );
    return 1;

} /* cmd() */
