/**
 * Used for mapping deleted files to new ones when things are changed.
 * @changed Updated to a more tidy mechanism by Ceres 14/3/96
 * @changed Changed clean_up() to not save when there are no changes,
 * and fixed up some broken messages.
 * - Sandoz, Nov. 2002.
 */

#define SAVE_FILE "/save/cloner"

/* How long a mapping lasts, currently 3 months */
#define LIFE_TIME 7257600

inherit OBJECT_OBJ;

mapping changes;

/** This method returns the list of currently moved objects. */
mapping query_changes() { return copy(changes); }

/** Saves the current configuration. */
void save_file() { unguarded( (: save_object, SAVE_FILE :) ); }

void setup() {
   changes = ([ ]);
   if( file_exists(SAVE_FILE+".o") )
       unguarded( (: restore_object, SAVE_FILE :) );
} /* setup() */

/**
 * Try and clone the passed in object.  If the name exists in the changes
 * array then the new file name is cloned instead of the old one.
 * @param word the name of the file to clone
 * @return the cloned object
 * @see other_file()
 * @see list_mappings()
 */
object clone( string word ) {
    string new_file;
    object thing;

    if( changes[ word ] && new_file = changes[ word ][ 0 ] )
        word = new_file;

    catch( thing = clone_object( word ) );

    return thing;

} /* clone() */

/**
 * This method returns the mapped name of the object.  If no mapping
 * exists the passed in value is returned directly.
 * @param word the object name to get a mapping for
 * @return the file to use
 * @see clone()
 * @see list_mappings()
 */
string other_file( string word ) {
    if( changes[ word ] )
        return changes[ word ][ 0 ];
    return word;
} /* other_file() */

/**
 * Use this function to add a new mapping of one filename to another.
 * Means that if an obhject of type 'from' is attempted to be cloned
 * an object of type 'to' will be cloned instead.
 * @param from the old object name
 * @param to the new object name
 * @return 1 on success, 0 on failure
 * @see remove_mapping()
 * @see list_mappings()
 */
int add_mapping( string from, string to ) {
    if( to[0..2] == "/w/") {
        write("Mapping to creator directories not allowed.\n");
        return 0;
    }

    if( !file_exists(to) && !file_exists(to+".c") ) {
        write("Destination file does not exist.\n");
        return 0;
    }

    changes[from] = ({ to, time() });
    save_file();

    write("Mapping of "+from+" to "+to+" added.\n");
    log_file("CLONER", "Mapping of %s to %s added by %s at %s.\n",
        from, to, TP->query_cap_name(), ctime(time()) );

    remove_call_out("clean_up");
    call_out("clean_up", 120 + random(500) );

    return 1;

} /* add_mapping() */

/**
 * Removes a mapping from the object.  This looks to see if an mapping
 * exists for the object and removes it if it does.
 * @param from the object to remove the mapping from
 * @return 1 on success and 0 on failure
 * @see add_mapping()
 * @see list_mappings()
 */
int remove_mapping( string from ) {
    if( changes && changes[ from ] ) {
        map_delete( changes, from );
        save_file();
        write("Mapping of "+from+" removed.\n");
        log_file("CLONER", "Mapping of %s removed by %s at %s.\n",
            from, TP->query_cap_name(), ctime(time()) );
        return 1;
    }

    write("No mapping found for "+from+".\n");
    return 0;

} /* remove_mapping() */

/**
 * This method returns a list of all the current mappings.  If a
 * pattern is given, then only files which contain that string will
 * be returned.
 * @param str the pattern to search for
 * @return the list of mappings
 * @see add_mapping()
 * @see remove_mapping()
 */
string list_mappings( string str ) {
    string from, retval;

    retval = "";

    foreach( from in keys(changes) ) {
        if( !str || strsrch( from, str ) != -1 ||
            strsrch( changes[from][0], str ) != -1 )
            retval += from+" -> "+changes[from][0]+"\n";
    }

    return retval;

} /* list_mappings() */

/** @ignore yes */
void clean_up() {
    string from;
    mixed  to;
    int flag;

    foreach( from, to in changes ) {
        if( ( to[1] < time() - LIFE_TIME ) ||
            ( !file_exists(to[0]) && !file_exists(to[0]+".c") ) ) {
            map_delete( changes, from );
            flag = 1;
        }
    }

    if( flag )
        save_file();

} /* clean_up() */
