
private string home_dir;
private nosave string current_path;

/**
 * This method expands the specified path to get a nice new path
 * based on the home directory or expanding the ~ stuff.
 * @param str the path to expand
 * @return the expanded path
 */
string get_path( string str ) {
    string *arr, *bits, tmp;
    int i;

    if( !str ) {
        if( !home_dir )
            home_dir = "/w/"+TP->query_name();
        str = home_dir;
    }

    if( str == "~") {
        str = "w/"+TP->query_name();
    } else {
        if( str[0] == '~') {
            if( str[1] == '/') {
                sscanf( str, "~%s", tmp );
                str = "/w/"+TP->query_name() + tmp;
           } else {
                string name;
                if( sscanf( str, "~%s/%s", name, str ) != 2 )
                    str = "w/"+str[1..];
                else
                    str = "w/"+name+"/"+str;
            }
        } else {
           if( str[0] != '/')
               str = current_path+"/"+str+"/";
        }

        if( str == "/")
            return "/";

        bits = allocate( sizeof( arr = explode( str, "/") - ({""}) ) );
        i = 0;

        foreach( tmp in arr ) {
            if( tmp == "..") {
                if( i )
                    bits[--i] = 0;
            } else if( tmp != ".")
                bits[i++] = tmp;
        }

        if( i )
            str = implode( bits, "/");
        else
            str = "";
    }

    return "/"+str;

} /* get_path() */

/**
 * This method expands the wildcard based file name and turns it
 * into a list of files
 * @param str the wildcard based file name
 * @return the array of file names
 */
string *get_files( string str ) {
    string path;
    string *names, *files, *ret;

    names = explode( str, " ") - ({""});

    if( !sizeof(names) )
        return ({ });

    ret = ({ });

    foreach( str in names ) {
        str = path = get_path( str );

        if( path == "/") {
            ret += ({"/"});
            continue;
        }

        if( sscanf( path, "%*s/%*s") == 2 )
            path = implode( ( explode( path, "/") - ({""}) )[0..<2], "/");

        if( files = get_dir(str) )
            ret += map( files, (: $2 + "/" + $1 :), path );

    }

    return map( ret, function( string str ) {
                if( str[0] != '/' )
                    return "/" + str;
                return str;
            } );

} /* get_files() */

/**
 * This method expands the wildcard based file name and turns it
 * into a list of c files.  This automaticaly adds a .c to any
 * files.
 * @param str the wildcard based file name
 * @return the array of file names
 * @changed changed to be *.* so as to handle virtual wombles as well
 */
string *get_cfiles( string str ) {
    return get_files( implode( map( explode( str, " ") - ({""}),
        function( string str ) {
            if( sscanf( str, "%*s.%*s") != 2 )
                str += ".c";
            return str;
        } ), " ") );
} /* get_cfiles() */

private string *process_recursive_files( string path, string *files ) {
    mixed data;

    foreach( data in get_dir( path, -1 ) ) {
        if( data[1] == -2 )
            files = process_recursive_files( path + data[0] + "/", files );
        else
            files += ({ path + data[0] });
    }

    return files;

} /* process_recursive_files() */

private string *process_recursive_dirs( string path, string *files ) {
    mixed data;
    string file;

    foreach( data in get_dir( path, -1 ) ) {
        if( data[1] == -2 ) {
            file = path + data[0] + "/";
            files = process_recursive_dirs( file, files ) + ({ file });
        }
    }

    return files;

} /* process_recursive_dirs() */

private string *process_recursive_all( string path, string *files ) {
    mixed data;
    string file;

    foreach( data in get_dir( path, -1 ) ) {
        if( data[1] == -2 ) {
            file = path + data[0] + "/";
            files = process_recursive_all( file, files ) + ({ file });
        } else {
            files += ({ path + data[0] });
        }
    }

    return files;

} /* process_recursive_all() */

/**
 * This method returns all the files (or directories if the dirs
 * flag is 1) in the subdirectories of the specified path.
 * If the dirs flag is set to anything higher than 1, then both
 * files and directories will be returned.
 * @param path the path to get the files for
 * @param dirs set to 1 to get all subdirs, instead of files
 */
string *get_files_recursive( string path, int dirs ) {
    path = get_path(path);

    if( path[<1] != '/')
        path += "/";

    if( !dir_exists( path ) )
        return 0;

    switch( dirs ) {
      case 0:
        return process_recursive_files( path, ({ }) );
      break;
      case 1:
        return process_recursive_dirs( path, ({ }) );
      break;
      default:
        return process_recursive_all( path, ({ }) );
    }

} /* get_files_recursive() */

/**
 * This method returns the current path.
 * @return the current path
 */
string query_current_path() { return current_path; }

/**
 * This method returns the home directory of the creator.
 * @return the home directory
 */
string query_home_dir() { return home_dir; }

/**
 * This method sets the current home directory of the creator.
 * @param dir the new home directory
 */
protected int set_home_dir(string dir) {
   home_dir = dir;
   return 1;
} /* set_home_dir() */

/**
 * This method sets the current path for the creator.
 * @param str the current path
 */
void set_current_path(string str) {  current_path = str; }
