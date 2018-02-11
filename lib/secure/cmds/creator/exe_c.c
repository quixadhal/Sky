/**
 * @changed Added basic support for defines and includes.
 * - Sandoz, Spring 2002.
 * @changed Added time taken and eval cost.
 * - Sandoz, 26th Sept. 2002.
 */

inherit COMMAND_BASE;

#define LOG_FILE "/secure/log/EXEC"

#define ERROR  0
#define COST   1
#define TIME   2
#define RETURN 3

/** @ignore yes */
mixed do_exec( string str ) {
    string file, wiz_dir, *tmp, pre;
    mixed ret;
    object ob;

    ret = allocate( 4 );

    if( !TP ) {
        ret[ERROR] = "fail";
        return ret;
    }

    wiz_dir = "/w/" + TP->query_name();
    if( file_size(wiz_dir) != -2 ) {
        ret[ERROR] = "fail";
        notify_fail("Directory: " + wiz_dir + " does not exist.\n");
        return ret;
    }

    file = wiz_dir + "/exec_tmp";
    if( ob = find_object(file) ) {
        file->dest_me();
        if( ob )
            destruct(ob);
    }

    if( file_size(file + ".c") > 0 )
        rm( file+".c" );

    unguarded( (: write_file( LOG_FILE, ctime(time()) + " - " +
                              TP->query_name() + ": " + $(str) +
                              "\n") :) );

    tmp = explode( str, ";" );
    str = "";
    pre = "";

    foreach( string line in tmp ) {
        if( line[0..6] == "#define" || line[0..7] == "#include" )
            pre += line+"\n";
        else
            str += line+";";
    }

    write_file( file + ".c", ( sizeof(pre) ? pre : "" ) +
                "void dest_me() { destruct(TO); }\n"
                "mixed do_call() {\n"+str+";\n}\n");

    if( ret[ERROR] = catch( ob = load_object(file) ) ) {
        if( ob )
            destruct(ob);
        rm( file+".c");
        return ret;
    }

    ret[ERROR] = catch {
        ret[COST] = eval_cost();
        ret[TIME] = time_expression( ret[RETURN] = file->do_call() );
        ret[COST] -= eval_cost();
    };

    if( ob = find_object(file) )
        ob->dest_me();

    rm( file+".c");
    return ret;

} /* do_exec() */

/** @ignore yes */
int cmd( string str ) {
    mixed ret = do_exec( str );

    if( ret[ERROR] == "fail")
        return 0;

    if( !ret[ERROR] ) {
        TP->more_string( sprintf("\nTime taken: %i Eval cost: %i\n"
            "Returns: %O", ret[TIME], ret[COST], ret[RETURN] ), "Exec results");
    } else {
        printf("Exec failed: %s", ret[ERROR] );
    }

    return 1;

} /* cmd() */

/** @ignore yes */
mixed query_patterns() { return ({"<string'code'>", (: cmd($4[0]) :)  }); }
