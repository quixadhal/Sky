
inherit COMMAND_BASE;

#define CLONER "/global/player/cloner"

/** @ignore yes */
int do_list( string file ) {
    string *files, change;
    mapping changes;

    if( file && file != "")
        files = TP->get_files(file);

    changes = CLONER->query_changes();

    foreach( change in sort_array( keys(changes), 1 ) ) {
        if( files && member_array( change, files ) == -1 &&
            strsrch( change, file ) == -1 )
            continue;

        printf("%s:\n  %s until %s\n", change, changes[change][0],
               ctime(changes[change][1]) );
    }

    return 1;

} /* do_list() */

/** @ignore yes */
int do_move(string source, string dest) {
    string *sources, *tmp, destp;

    seteuid( geteuid(TP) );

    if( !sizeof( sources = TP->get_files(source) ) )
        return notify_fail("No source files found.\n");

    if( !dir_exists( destp = TP->get_path(dest) ) )
        return notify_fail("Destination must be a directory.\n");

    foreach( source in sources ) {
        tmp = explode( source, "/");
        dest = destp+"/"+tmp[<1];

        if( rename( source, dest ) )
            return notify_fail("Error renaming ["+source+"] to ["+dest+"]\n");

        CLONER->add_mapping( source, dest );

    }

    return 1;

} /* do_move() */

/** @ignore yes */
int do_add( string source, string dest ) {
    // Get rid of the .c extensions.
    if( source[<2..] == ".c")
        source = source[<2..];

    if( dest[<2..] == ".c")
        dest = dest[<2..];

    CLONER->add_mapping( source, dest );
    return 1;

} /* do_add() */

/** @ignore yes */
int do_remove( string source ) {
    CLONER->remove_mapping(source);
    return 1;
} /* do_remove() */

/** @ignore yes */
mixed *query_patterns() {
    return ({
        "list", (: do_list(0) :),
        "list <word'file'>", (: do_list( $4[0] ) :),
        "move <word'source'> <word'dest'>", (: do_move( $4[0], $4[1] ) :),
        "add <word'source'> <word'dest'>", (: do_add( $4[0], $4[1] ) :),
        "remove <word'source'>", (: do_remove( $4[0] ) :),
    });
} /* query_patterns() */
