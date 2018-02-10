/**
 * Documentation system for removing documentation from source files.
 * <p>
 * The documentation will be created in the directory /doc/autodoc.  Extra
 * documentation should be placed into the other help directories and will
 * be accessed via the help system.  The html documentation will be placed
 * into the /www/autodoc directory.  The files in theses directories will
 * be named with the '/'s replaced with '.'s.  ie: /obj/handlers/room_handler.c
 * would be in the file /doc/autodoc/obj.handlers.room_handler.c and
 * /www/autodoc/obj.handlers.room_handler.c.html.  An index of the methods
 * and object names will be generated from the input files and linked into the
 * help system.  An alphabetical index will also be produced that is linked
 * to the html stuff.
 * <p>
 * The first comment which follows the comment format below and occurs right
 * before a function will be used as the documentation for that function.  The
 * comment must start with two '*'s to distinguish it from a normal comment.
 * A special tag at the start of one of the first comments in the file will
 * signify that it is a comment for the class.
 * <p>
 * Only comments before a real function declaration will be used, comments
 * before a predeclaration will be ignored.
 * <p>
 * Any function which is not declared as private and does not have the @ignore
 * tag in its description will be placed into the resulting documentation
 * file.  This will be done even if no comment was found.
 * <p>
 * In the text part of the comment the main section is a free form
 * documentation, it is merely a description of the whole function.  After
 * this several tags can be added to the comment which allows other things
 * to be documented.  All the text after the tag is taken as part of that
 * tags information.  The main tag should never actually be used, it
 * is implied as being the tag associated with the first section of
 * comment.
 * <p>
 * HTML tags will be used in the comments to allow certain things to
 * happen.  The tags 'p', 'b', 'i', 'code', 'ul', 'ol', 'dl', 'dd', 'dt', 'pre'
 * and 'li' will be supported.
 * <p>
 * The sequences '\\/', '\\\\' and '\<' will be processed by the comment
 * handling code so that they do not get placed into the output code.
 * <p>
 * White space is ignored...  Both html and roff format files will be
 * created from the documentation maker.
 * <p>
 * The supported tags are:
 * <dl>
 * <dt>@param
 * <dd>Specify the parameter and what it does.  This should be one short
 *     sentence only.  There needs to be one of these for every
 *     parameter, the first word of the sentence is the name of the
 *     parameter and the rest is the description.  There should be no full
 *     stop on the end of the sentance.
 * <dt>@member
 * <dd>This is used with classes and defines the members of the class.  The
 *     definition is the same as for the @param element above.
 * <dt>@return
 * <dd>Specify what the return value is.  This should be one short
 *     sentence only.  Theres should only be one of these in every
 *     function.  There should be no full stop on the end of the sentence.
 * <dt>@see
 *   <dd>Specify an object or function to see.  The format must be one of:
 *   <ul>
 *   <li> object_name<br><i>@see /global/new_parse</i>
 *   <li> function_name()<br><i>@see frog()</i>
 *   <li> object_name->function_name()<br><i>@see /global/new_parse->add_command()</i>
 *   <li> efun::name()<br><i>@see efun::sort_array()</i>
 *   <li> help::name<br><i>@see help::effects</i>
 *   </ul>
 * <dt>@ignore
 *   <dd>Do not document this function, the whole comment is ignored.
 * <dt>@main
 *   <dd>The main documentation section.
 * <dt>@classification
 *    <dd>Used in the effects files to classify the effect.  So a tree
 *      of the classifications can be derived.
 * <dt>@index
 *        <dd>Adds in a new index reference on this name.  This should be one
 *            word only.
 * <dt>@change
 *   <dd>Placed in the class documentation to reference a change list.
 *            First line after the tag is the date and the author and
 *            then follows a description of the change.
 * <dt>@example
 *   <dd>If you wish to embed an example in the code.
 * <dt>@started
 *   <dd>When the file was started.
 * <dt>@author
 *   <dd>The author of the file.
 * </dl>
 *
 * The output format will look something like the API documentation for
 * java.  If you have not seen this before, go to http://www.javasoft.com
 * and look in the JDK and then the API section.
 * <p>
 * Comment format:
 * <pre>
 * /\**
 *  *  <text>
 *  *    ..
 *  *  <text>
 *  *
 *  *\/
 * </pre>
 *
 * ie: it starts with a /\** and ends with a *\/ on a line by themselves and
 * every intervening line has a * at the start.  By using a \\ you can
 * make characters escaped.  This means they are not processed by
 * the internal system, so to make a end comment appear inside a
 * the text you go : *\\/
 *
 * @example
 * inherit "/std/object";
 *
 * /\**
 *  *   This class is a nice shade of blue and allows all other shades of
 *  *   green and yellow to mix well.
 *  * @see yellow
 *  * @see blue
 *  * @change 12/3/97 Pinkfish
 *  *    Pushed some of the yellow stuff down a hole and ate a bagel.
 *  *\/
 *
 * /\**
 *  * This method mixes the green in with our blue.
 *  * @param  green   the green object to mix.
 *  *\/
 * void mixGreen(object green) {
 *   /\* Something wild and exciting! *\/
 * } /\* mixGreen() *\/
 *
 * /\**
 *  * This method mixes the yellow in with our blue.
 *  * @param  yellow the yellow object to mix.
 *  * @return returns a TRUE or FALSE depending on if the mix succeeded.
 *  * @example
 *  *   mixYellow(12);
 *  *\/
 * int mixYellow(object yellow) {
 * } /\* mixYellow() *\/
 *
 * @index autodoc
 * @see autodoc_file
 * @see autodoc_nroff
 * @see autodoc_html
 * @author Pinkfish
 * @started Tue Oct 28 13:25:09 EST 1997
 *
 */

#include <autodoc.h>

/**
 * This defines the main save file of the handler.
 */
#define AUTODOC_SAVE_FILE      (AUTODOC_SAVE_DIR "main_rubbish")

/**
 * This defines the interval at which we force a save to store the file_pos.
 * Let's do this every 6 hours.
 */
#define SAVE_FILE_POS_INTERVAL (60*60*6)

string *files;
mapping help_map;
int file_pos, something_changed, last_created_index;

nosave int call_id, last_saved;
nosave mapping summary_map;
nosave mixed extra_indexes;
nosave string *exclude_methods;
nosave object *in_use, *recycled;

private void create_nroff_file( object ob );
private void create_html_index( mixed ob );
private void after_thingy( int no_index );
private void start_processing();
private void do_parse_next_file();
private void create_index();
private void add_to_index( object ob );
private void save();
string *query_files();
int remove_file( string fname );

/** @ignore yes */
void set_file_pos( int number ) { file_pos = number; }

void create() {
    seteuid(getuid());

    file_pos = 0;
    something_changed = 0;
    files = ({ });

    unguarded( (: restore_object( AUTODOC_SAVE_FILE ) :) );

    if( !mapp( help_map ) )
        help_map = ([ ]);

    exclude_methods = ({"create", "setup", "init", "reset", "dest_me"});
    in_use = recycled = ({ });

    last_saved = time();

    start_processing();

} /* create() */

/** @ignore yes */
private void create_nroff_file( object ob ) {
    string fname;

    // Now we create the nroff file.
    fname = replace( ob->query_file_name(), "/", ".");
    if( fname[0] == '.')
        fname = fname[1..];
    rm( NROFF_DOC_DIR + fname );
    event( users(), "inform", "Creating "+NROFF_DOC_DIR + fname, "autodoc");
    AUTODOC_NROFF->create_nroff_file( ob, NROFF_DOC_DIR + fname );

} /* create_nroff_file() */

/** @ignore yes */
private void create_html_index( mixed ob ) {
    string fname, letter;
    mixed stuff, tmp;
    mapping chars;

    // Create an index file.
    stuff = map( query_files(), function( string name ) {
                string *bits;

                bits = explode(name, "/");
                return ({ bits[sizeof(bits)-1], name, "", summary_map[name] });
            } );

    stuff = unique_array( ob + stuff, (: lower_case($1[0])[0] :));
    chars = ([ ]);

    foreach( tmp in stuff ) {
        letter = tmp[0][0][0..0];
        fname = "index_" + lower_case(letter) + ".html";
        AUTODOC_HTML->create_html_index_file( tmp, letter,
                                              HTML_DOC_DIR + fname );
        reset_eval_cost();
        chars[CAP(letter)] =  fname;
    }

    // The different sorts of indexes will be genertated in the html file.
    fname = HTML_DOC_DIR;;
    AUTODOC_HTML->create_main_index( chars, fname );

} /* create_html_file() */

/**
 * This method returns the autodoc parsers currently loaded.
 * @return the currently loaded autodoc parsers
 */
object *query_parsers_in_use() { return in_use; }

/**
 * This method returns the autodoc parsers currently recycled.
 * @return the recycled autodoc parsers
 */
object *query_parsers_recycled() { return recycled; }

/** @ignore yes */
private void recycle_parser( object ob ) {
    in_use -= ({ ob, 0 });

    // Maintain two parsers at most.
    if( sizeof( recycled -= ({ 0 }) ) > 2 ) {
        ob->dest_me();
    } else {
        reload_object(ob);
        recycled += ({ ob });
    }
} /* recycle_parser() */

/** @ignore yes */
private object get_parser() {
    object ob;

    if( sizeof( recycled -= ({ 0 }) ) ) {
        ob = recycled[0];
        recycled = recycled[1..];
    } else {
        ob = clone_object(AUTODOC_FILE);
    }

    in_use += ({ ob });

    return ob;

} /* get_parser() */

/** @ignore yes */
private void after_thingy( int no_index ) {
    if( member_array( PO->query_file_name(), files ) == -1 ) {
        call_out( (: recycle_parser :), 5, PO );
        return;
    }

    if( no_index )
        event( users(), "inform", "Autodoc: finished recreating the "
            "documentation for "+PO->query_file_name(), "autodoc");

    if( PO->query_changed() )
        something_changed = 1;

    event( users(), "inform", "Autodoc: \""+PO->query_file_name()+"\" "+
         ( PO->query_num_failed_tries() ? "failed to parse correctly!" :
         "parsed correctly." ), "autodoc");

    // This means that the file has been deleted...  Argh!  Auto remove it.
    // Delete PO->query_file_name(), not some file_pos - Sandoz.
    if( PO->query_num_failed_tries() > 1 )
        remove_file( PO->query_file_name() );
    else if( PO->query_changed() || no_index )
        call_out( (: create_nroff_file :), 0, PO );

    call_out( (: recycle_parser :), 5, PO );
    save();

} /* after_thingy() */

/**
 * This method queries whether or not the documentation
 * for the specified file has expired.
 * @param name the file name to check
 * @return 1 if an update is needed
 */
int query_update_needed( string name ) {
    int cur_change;
    string my_name, dotless;

    // No such file or directory.
    if( !sizeof( unguarded( (: stat($(name)) :) ) ) ) {
        remove_file( name );
        return 0;
    }

    // A directory - always update.
    if( dir_exists(name) )
        return 1;

    my_name = replace_string( name, "/", ".");
    dotless = ( my_name[0] == '.' ? my_name[1..] : my_name );

    if( my_name[<2..] == ".c" )
        my_name = my_name[0..<3];

    // No save file.
    if( !unguarded( (: file_exists($(AUTODOC_SAVE_DIR+my_name+".o")) :) ) )
        return 1;

    // No NROFF file.
    if( !unguarded( (: file_exists($(NROFF_DOC_DIR+dotless)) :) ) )
        return 1;

    cur_change = unguarded( (: stat($(name)) :) )[1];

    // The main file is outdated.
    if( cur_change > unguarded( (: stat($(AUTODOC_SAVE_DIR+my_name+".o")) :) )[1] )
        return 1;

    // The nroff docs are outdated.
    if( cur_change > unguarded( (: stat($(NROFF_DOC_DIR+dotless)) :) )[1] )
        return 1;

    return 0;

} /* query_update_needed() */

/** @ignore yes */
private void do_parse_next_file() {
    // Give it a minute to do this file.
    call_id = call_out( (: start_processing :), 60 );

    if( file_pos >= sizeof(files) ) {
        file_pos = 0;
        return;
    }

    if( query_update_needed(files[file_pos]) )
        get_parser()->parse_file( files[file_pos], (: after_thingy(0) :) );
    else
        event( users(), "inform", "Autodoc: Docs for \""+
            files[file_pos]+"\" already up to date", "autodoc");

    if( time() > last_saved + SAVE_FILE_POS_INTERVAL )
        save();

    file_pos++;

} /* do_parse_next_file() */

/** @ignore yes */
private void start_processing() {
    if( !sizeof(files) )
        return;

    // Ok, now we skip onto the next file and zoom it.
    if( last_created_index + SAVE_INDEX_DELAY < time() ) {
        if( something_changed )
            call_out( (: create_index :), 2 );
        something_changed = 0;
        save();
    }

    call_id = call_out( (: do_parse_next_file :), 180 );

} /* start_processing() */

/** @ignore yes */
private string query_short_args_def( mixed args ) {
    string ret;
    int i;

    ret = "";

    for( i = 0; i < sizeof(args); i += 2 ) {
       if( i != 0 )
           ret += ", ";
       ret += implode( args[AUTO_ARGS_TYPE], " ");
    }

    return "(" + ret + ")";

} /* query_short_args_def() */

/**
 * This method queries whether or not a function with the specified
 * name should be documented or not.
 * This prevents create, setup, reset, init and dest_me from
 * being documented.
 */
int exclude_method( string name ) {
    return member_array( name, exclude_methods ) != -1;
} /* exclude_method() */

/**
 * @ignore yes
 * This will add it into the help array stuff too.
 */
private string *process_stuff( string name, string fname, string fn,
                               mapping docs ) {
    string blue;
    int i, end;
    mixed ret;
    mapping fluff;

    if( !exclude_method(name) ) {
        if( help_map[name] )
            help_map[name] += ({ fn });
        else
            help_map[name] = ({ fn });
    }

    if( docs[name] ) {
        if( arrayp(docs[name]) ) {
            // Its a function!
            fluff = docs[name][AUTO_DOCS];
            ret =  ({ name, fname, query_short_args_def(docs[name][AUTO_ARGS]) });
        } else {
            ret = ({ name, fname, "" });
            fluff = docs[name];
        }
    } else {
        ret = ({ name, fname, "" });
        if( mapp(docs) ) {
            fluff = docs;
        } else {
            fluff = ([ ]);
        }
    }

    if( fluff["main"] ) {
        blue = implode( fluff["main"], "\n");
        end = strlen(blue);
        i = strsrch( blue, ".");
        if( i > 0 )
            end = i;
        i = strsrch( blue, "!");
        if( i > 0 && i < end )
            end = i;
        i = strsrch( blue, "?");
        if( i > 0 && i < end )
            end = i;
        blue = blue[0..end];
    }

    ret += ({ blue });

    if( fluff["index"] ) {
        foreach( blue in fluff["index"] ) {
            blue = replace( blue, ({ " ", "", "\n", "" }));
            if( help_map[blue] ) {
                help_map[blue] += ({ fn });
            } else {
                help_map[blue] = ({ fn });
            }
            extra_indexes += ({ ({ blue, fname, "", ret[AUTO_INDEX_SUMMARY] }) });
       }
    }

    return ret;

} /* process_stuff() */

/** @ignore yes */
private void create_index() {
    mixed index_stuff, rabbit;
    string fname, fn, *bits, file;
    object parse;

    // Build up the list.
    index_stuff = ({ });
    help_map = ([ ]);
    extra_indexes = ({ });
    summary_map = ([ ]);

    parse = get_parser();

    foreach( file in files ) {
        // Do not process it.  Merely load it from disk.
        parse->parse_file( file, 0, 1 );
        fname = parse->query_file_name();
        if( fname ) {
            fn = replace( fname, "/", ".");
            if( fn[0] == '.' )
                fn = fn[1..];
            fn = NROFF_DOC_DIR + fn;
            bits = explode( fname[0..strlen(fname)-3], "/");
            rabbit = process_stuff( bits[sizeof(bits) - 1], "", fn,
                                    parse->query_main_docs() );
            if( rabbit[AUTO_INDEX_SUMMARY] ) {
                summary_map[fname] = rabbit[AUTO_INDEX_SUMMARY];
            } else {
                map_delete( summary_map, fname );
            }

            // We loaded it...
            index_stuff += map( keys( parse->query_public_functions() ),
                                (: process_stuff( $1, $(fname), $(fn),
                                $(parse)->query_public_functions() ) :) );
            index_stuff += map( keys( parse->query_protected_functions() ),
                                (: process_stuff( $1, $(fname), $(fn),
                                $(parse)->query_protected_functions() ) :) );
            index_stuff += map( keys( parse->query_class_docs() ),
                                (: process_stuff( $1, $(fname), $(fn),
                                $(parse)->query_class_docs() ) :) );
            if( sscanf( fname, "%*s.h" ) == 1 ) {
                index_stuff += map( keys( parse->query_define_docs() ),
                                (: process_stuff( $1, $(fname), $(fn),
                                $(parse)->query_define_docs() ) :) );
            }
        }
        reset_eval_cost();
    }

    if( parse )
        recycle_parser(parse);

    index_stuff += extra_indexes;
    extra_indexes = ({ });

    // Don't call this out.  We would be copying huge arrays around.  Eeek.
//    catch( create_html_index( index_stuff ) );

    summary_map = ([ ]);
    last_created_index = time();
    save();

    event( users(), "inform", "Autodoc: Index generation successful",
        "autodoc");

} /* create_index() */

/**
 * Recreate documentation for one field immediately.  This does not update
 * the index, it merely creates the base files for this file.  It does it
 * at a random delay of up to 30 seconds.
 * @param fname the file name to update
 */
int recreate_documentation( string fname ) {
    if( member_array( fname, files ) != -1 ) {
        get_parser()->parse_file( fname, (: after_thingy(1) :) );
        return 1;
    }

    return 0;

} /* recreate_documentation() */

/**
 * Regenerate the index files.
 */
void recreate_indexes() {
     unguarded( (: create_index() :) );
} /* recreate_indexes() */

/**
 * Adds a file into the list of files to process for autodocumentation.
 * This file will be processed every time around in the autodocumentation
 * loop to check to see if it has been changed and the results will be
 * placed into the index.  It automaticly calls recreate_documentation
 * in random(60) seconds.
 * @see recreate_documentation()
 * @param fname the name of the file to add
 * @return 1 if the add succeeds, 0 otherwise.
 */
int add_file( string fname ) {

    fname = "/" + implode( explode( fname, "/" ) - ({ "" }), "/" );

    if( fname[0..2] == "/w/" || fname[0..2] == "/d/" )
        event( users(), "inform", "Autodoc: Adding \""+
            fname+"\" to autodoc handler!", "autodoc");

    if( member_array( fname, files ) == -1 ) {
        if( sizeof( unguarded( (: stat($(fname)) :) ) ) ) {
            files += ({ fname });
            save();
            if( sizeof(files) == 1 )
                start_processing();
            else
                call_out( (: recreate_documentation :), 20, fname );
            return 1;
        }
    }

    return 0;

} /* add_file() */

/**
 * Removes a file from the list of files to be processed
 * for autodocumentation.
 * @param fname the name of the file to remove
 * @return 1 if the remove succeeds, 0 otherwise.
 */
int remove_file( string fname ) {
    string fn, dfn, *our_files;
    int i;

    if( member_array( fname, files ) != -1 ) {
        files -= ({ fname });
        file_pos = 0;


        fn = replace_string( fname, "/", ".");
        dfn = ( fn[0] == '.' ? fn[1..] : fn );

        if( fn[<2..] == ".c" )
            fn = fn[0..<3];

        fn = AUTODOC_SAVE_DIR+fn+".o";

        // Delete the save file.
        unguarded( (: rm($(fn)) :) );

        // Delete the nroff files.
        unguarded( (: rm( NROFF_DOC_DIR+$(dfn) ) :) );

        // Delete the nroff single function files.
        fn = NROFF_DOC_SINGLE+( fname[0] == '/' ? fname[1..<3] :
                                                  fname[0..<3] )+"/";
        our_files = get_dir(fn);

        if( sizeof(our_files) )
            foreach( dfn in our_files )
                unguarded( (: rm($(fn+dfn)) :) );

        // Remove the directory if it exists.
        if( dir_exists(fn) )
            unguarded( (: rmdir($(fn)) :) );

        // Try to remove the next upper directory as well, if it's empty.
        if( ( i = strsrch( fn[0..<2], "/", -1 ) ) != -1 ) {
            fn = fn[0..i];
            if( dir_exists(fn) && !sizeof( get_dir(fn) ) )
                unguarded( (: rmdir($(fn)) :) );
        }

        save();

        if( !sizeof(files) )
            remove_call_out(call_id);

        event( users(), "inform", "Autodoc: removing \""+
            fname+"\""+( !file_exists(fname) ? " (doesn't exist)" :
            "")+" from the autodoc handler", "autodoc");

        return 1;

    }

    if( TP )
        tell_object( TP, "There is no such file in the autodoc "
            "handler.\n");
    return 0;

} /* remove_file() */

private void save() {
    last_saved = time();
    unguarded( (: save_object( AUTODOC_SAVE_FILE ) :) );
    event( users(), "inform", "Saving the Autodoc handler.", "autodoc");
} /* save() */

/**
 * Returns the list of files we are current processing.
 * @return an array of strings being the file names
 */
string *query_files() { return files; }

/**
 * Returns the help mapping.  This is the mapping from function names to
 * files.  Each element in the mapping referes to an array of file
 * names which contain the function or define.
 * @return a mapping of arrays of files
 */
mapping query_help_map() { return help_map; }

/**
 * Returns the number of functions in the help file mapping.
 * @return the number of functions
 */
int query_help_map_size() { return sizeof(help_map); }

/**
 * This method returns the array of help files with the
 * specified function in it.
 * @param func the name of the function to get the help files for
 * @return array of file names
 */
string *query_help_on_func( string func ) { return help_map[func]; }

/**
 * This method returns the file name of the help for the specified
 * function in the specified file.
 * @param file the file name to find the help in
 * @param func the function to look for help on in the file
 * @return the full path to the help file, 0 if it does not exist
 */
string query_help_on( string file, string func ) {
    if( file[0] == '/' )
        file = file[1..];
    if( sscanf(file, "%*s.c") == 1 )
        file = file[0..<3];
    file = NROFF_DOC_SINGLE + file + "/" + func;
    if( file_size(file) > 0 )
        return file;
    return 0;
} /* query_help_on() */

/**
 * Returns the mapping of file names to a summary.  This is only
 * valid during the index creation cycle of the documentation generation
 * system.  It is used to create a more useful index page.
 * @return the mapping of file names to summarys
 */
mapping query_summary_map() { return summary_map; }

/**
 * This method tells us if the file is currently in the autodoc set.
 * @return 1 if the file is found, 0 if not
 */
int is_autodoc_file(string name) {
    return member_array( name, files ) != -1;
} /* is_autodoc_file() */

/** @ignore yes */
mixed stats() {
    string str;
    int next, to_go;

    next = SAVE_FILE_POS_INTERVAL + last_saved;
    to_go = next - time();

    if( to_go <= 0 )
        str = "any time now";
    else
        str = sprintf("%s (%s)", ctime(next), time_string( to_go, 2 ) );

    return ({
        ({"parsers in use", sizeof(in_use) || "none"}),
        ({"parsers recycled", sizeof(recycled) || "none"}),
        ({"number of files", sizeof(files) }),
        ({"help map size", sizeof(help_map) }),
        ({"something changed", something_changed ? "yes" : "no"}),
        ({"current file", files[file_pos] }),
        ({"last created index", ctime(last_created_index) }),
        ({"last saved", ctime(last_saved) }),
        ({"next saving", str }),
    });

} /* stats() */
