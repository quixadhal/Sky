/*
 * This is all the autoloading stuff.
 * please don't remove again - lynscar.
 */

/** @ignore yes */
protected string *load_file( string file ) {
    if( !file = read_file("/secure/config/"+file ) )
        return ({ });

    return filter( explode( file, "\n"), (: $1[0] != '#' :) );

} /* load_file() */

/** @ignore yes */
string *epilog() {
    string *ret, file;

    ret = ({ });

    // Scan for domain loaders.
    foreach( file in get_dir("/d/") ) {
        file = "/d/"+file+"/loader";
        if( file_exists( file+".c") )
            ret += ({ file });
    }


    return ret + load_file("preload");

} /* epilog() */

/** @ignore yes */
void preload( string file ) {
    mixed err;

    printf("Preloading: "+file+".\n");

    if( err = catch( load_object(file) ) )
        printf("            "+err+"\n");

} /* preload() */
