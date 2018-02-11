/**
 * This handler keeps track of global defines that have been
 * defined as strings, and can be used to resolve them if needed.
 * Its main use is to make string_to_define() simul_efun a few
 * thousand times faster, amongst other things.
 * @author Sandoz
 */

#define RESOLVER "/tmp/def_tmp"

#include <runtime_config.h>

private mapping global_defs;
private string gif, *ids;

private void rehash_defines();
private void destruct_resolver();

private void create() {
    rehash_defines();
} /* create() */

private void rehash_defines() {
    int i;
    string tmp, dir, name;
    string *include_list, *define_list, *processed;

    global_defs = ([ ]);

    if( !sizeof(ids) ) {
        if( !( tmp = get_config( __INCLUDE_DIRS__ ) ) ||
            !( i = sizeof( ids = explode( tmp, ":") - ({ 0, ""}) ) ) )
            error("Oh no! We don't have any include dirs!\n");

        while( i-- )
            if( ids[i][<1] != '/' )
                ids[i] += "/";
    }

    if( !gif || !file_exists(gif) ) {
        if( !( tmp = get_config( __GLOBAL_INCLUDE_FILE__ ) ) ||
            tmp[0] != '<' || tmp[<1] != '>')
            error("Oh no! We don't have a global include file!\n");

        tmp = tmp[1..<2];
        foreach( dir in ids ) {
            if( file_exists( dir + tmp ) ) {
                gif = dir + tmp;
                break;
            }
        }

        if( !gif )
            error("Oh no! We don't have a global include file!\n");

    }

    include_list = ({ gif });
    define_list = processed = ({ });

    while( sizeof( include_list ) ) {
        processed += ({ dir = include_list[0] });
        include_list = include_list[1..];

        tmp = unguarded( (: read_file, dir :) );

        foreach( tmp in explode( tmp, "\n") - ({ 0, ""}) ) {
            if( sscanf( tmp, "#define %*s(%*s)%*s") == 3 ||
                sscanf( tmp, "#define %*s %*s()") == 2 )
                continue;

            if( sscanf( tmp, "#define %s %*s", name ) == 2 &&
                upper_case(name) == name ) {
                define_list += ({ name });
                continue;
            }

            if( sscanf( tmp, "#include <%s>%*s", name ) ) {
                foreach( dir in ids ) {
                    if( file_exists( dir + name ) &&
                        member_array( dir + name, processed ) == -1 &&
                        member_array( dir + name, include_list ) == -1 ) {
                        include_list += ({ dir + name });
                        break;
                    }
                }
            }
        }
    }

    global_defs = allocate_mapping( sizeof(define_list) );

    define_list = map( define_list,
        (: sprintf("#ifdef %s\n    ob->add_define(\"%s\", %s );\n#endif",
           $1, $1, $1 ) :) );

    tmp = "void create() {\n"
          "    object ob = find_object(\""+file_name(TO)+"\");\n\n"+
          implode( define_list, "\n")+"\n\n"
          "    destruct(TO);\n\n}\n";

    unguarded( (: write_file, RESOLVER+".c", tmp, 1 :) );

    call_out( (: destruct_resolver :), 1 );

    catch( load_object(RESOLVER) );

} /* rehash_defines() */

/** @ignore yes */
private void destruct_resolver() {
    object ob;

    if( ob = find_object(RESOLVER) )
        destruct(ob);

    if( file_exists(RESOLVER+".c") )
        unguarded( (: rm, RESOLVER+".c" :) );

} /* destruct_resolver() */

/** @ignore yes */
int force_update_defines() {
    if( !adminp( previous_object(-1) ) )
        return 0;

    write("Rehashing defines...");
    rehash_defines();
    write("Rehash completed...");

    return 1;

} /* force_update_defines() */

/** @ignore yes */
void add_define( string name, string value ) {
    if( member_array( TO, previous_object(-1) ) != -1 &&
        stringp(value) )
        global_defs[name] = value;
} /* add_define() */

/**
 * This method returns the mapping of all global defines.
 * @return the mapping of global defines
 */
mapping query_global_defines() { return copy(global_defs); }

/**
 * This method resolves a global define.
 * @param str the string to resolve
 * @return the resolved value, or 0
 */
string string_to_define( string str ) { return global_defs[str]; }

/** @ignore yes */
mixed stats() { return ({ ({ "global defines", sizeof(global_defs) }) }); }
