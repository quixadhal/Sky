/**
 * A simplified version of the standard unix command 'ls', built for
 * the NS mudlib but based off similar mud variants of the command.
 * @author Galileo, 10-26-98.
 * @changed Ported to DS from the Sorrows 1.8 Mudlib, Shaydz 14-7-2000.
 * @changed Rewrote partly to give more sensible fail messages and
 * made it a little faster, Sandoz 18-5-2002.
 */

#define DIR_COLOR     "%^BOLD%^%^BLUE%^"
#define C_COLOR       "%^GREEN%^"
#define H_COLOR       "%^CYAN%^"
#define LOADED_COLOR  "%^MAGENTA%^"
#define RESET         "%^RESET%^"

inherit COMMAND_BASE;

string directory_listing( string, string *, mapping );

/** @ignore yes */
string query_usage() {
  return "ls -adfhloFS <directory>\n\n"
         "  -a    Don't hide . and .. files\n"
         "  -d    List directory entries instead of files\n"
         "  -f    List files instead of directory entries\n"
         "  -h    Display this help and exit\n"
         "  -l    Use a long listing format\n"
         "  -o    Colorize entries\n"
         "  -F    Prepend an '*' to indicatate loaded\n"
         "  -Sn   Sort entries:\n"
         "          d - Date\n"
         "          e - Extension\n"
         "          n - Name\n"
         "          s - Size\n";

} /* query_usage() */

/** @ignore yes */
int cmd( string arg ) {
    int i;
    mapping flags;
    string *files, *parts;
    string mask, dir, tmp;

    if( arg )
        parts = explode( arg, " ") - ({ 0, "" });

    flags = ([ ]);
    mask = "";
    dir = ".";

    // Cycle through each part, if it starts with an '-' assume
    // it's a switch, if it doesn't start with a '-' set it as the
    // specified directory.  If we try and set the directory twice,
    // error out we've got a syntax problem.
    if( sizeof(parts) ) {
        foreach( tmp in parts ) {
            for( i = 0; i < sizeof(tmp); i++ ) {
                if( tmp[0] == '-') {
                    switch( tmp[++i] ) {
                      case 'a': flags["a"] = 1;
                      break;
                      case 'd': flags["d"] = 1;
                      break;
                      case 'f': flags["f"] = 1;
                      break;
                      case 'h': flags["h"] = 1;
                      break;
                      case 'l': flags["l"] = 1;
                      break;
                      case 'o': flags["o"] = 1;
                      break;
                      case 'F': flags["F"] = 1;
                      break;
                      case 'S': // Check for specified sort order
                        if( i++ < sizeof(tmp) ) {
                            if( tmp[i] == 'd' || tmp[i] == 'e' ||
                                tmp[i] == 'n' || tmp[i] == 's' ) {
                                flags["S"] = 1;
                                flags["n"] = tmp[i];
                            } else
                                return notify_fail("Invalid sort order.\n"
                                    "Type 'ls -h' for help.\n");
                        } else {
                            return notify_fail("No sort order was passed.\n"
                                "Type 'ls -h' for help.\n");
                        }
                      break;
                    }
                } else {
                    if( dir == ".") {
                        dir = tmp;
                        break;
                    } else {
                        return notify_fail("Only one directory may be "
                            "listed.\nType 'ls -h' for help.\n");
                    }
                }
            }
        }
    }

    // Check for flags we need to do immediately.
    if( flags["h"] )
        return notify_fail(query_usage());;

    tmp  = master()->make_path_absolute(dir);
    dir = tmp;
    mask = sizeof( explode( dir, "/") ) ? explode( dir, "/")[<1] : "*";

    if( !dir_exists(dir) ) {
        dir = implode( explode( dir, "/")[0..<2], "/");
        if( !dir_exists(dir) )
            return notify_fail("Directory '"+tmp+"' does not exist.\n");
    } else {
        mask = "*";
    }

    if( sizeof(dir) )
        if( dir[<1] != '/')
            dir += "/";

    if( !master()->valid_read( dir, geteuid(TP), "get_dir") )
        return notify_fail("$I$0=ls: "+dir+": Permission denied.\n");

    if( !files = get_dir( dir + mask ) )
        files = get_dir(dir);

    if( !sizeof(files) && !dir_exists(tmp) )
        return notify_fail("Directory '"+tmp+"' does not exist.\n");

    if( flags["a"] )
        files = filter( files, (: $1 != "." :) );
    else
        files = filter( files, (: $1[0] != '.' :) );

    if( !sizeof(files) || ( sizeof(files) == 1 && files[0] == "..") )
        return notify_fail("Directory is empty.\n");

    TP->more_string( directory_listing( dir, files, flags ), "ls ("+dir+")");
    return 1;

} /* cmd() */

/** @ignore yes */
string colorize_dir( string dir ) { return DIR_COLOR + dir + RESET; }

/** @ignore yes */
string colorize_file( string file ) {
    switch( file[<2..] ) {
      case ".c":
        return C_COLOR + file + RESET;
      case ".h":
        return H_COLOR + file + RESET;
    }

    return file;

} /* colorize_file() */

/** @ignore yes */
int date_sort( mixed arg1, mixed arg2, string dir ) {
    int *stat1 = stat( dir + arg1 );
    int *stat2 = stat( dir + arg2 );

    if( stat1[1] > stat2[1] )
        return -1;
    else if( stat1[1] < stat2[1] )
        return 1;

    return 0;

} /* date_sort() */

/** @ignore yes */
int extension_sort( mixed arg1, mixed arg2 ) {
    return strcmp( arg1[<2..], arg2[<2..] );
} /* extension_sort() */

/** @ignore yes */
int size_sort( mixed arg1, mixed arg2, string dir ) {
    int size1 = file_size( dir + arg1 );
    int size2 = file_size( dir + arg2 );

    if( size1 > size2 )
        return -1;
    else if( size1 < size2 )
        return 1;

    return 0;

} /* size_sort() */

/** @ignore yes */
int name_sort( mixed arg1, mixed arg2 ) { return strcmp( arg1, arg2 ); }

/** @ignore yes */
string short_print( string *info, int max ) {
    int cols, rows, entry, i, j, size;
    string ret;

    ret = "";
    cols = TP->query_cols() / ( max + 5 );
    rows = ( ( sizeof(info) % cols ) ? ( sizeof(info) / cols ) + 1 :
               sizeof(info) / cols );

    for( i = 0; i < rows; i++ ) {
        entry = i;

        for( j = 0; j < cols; j++ ) {
            if( j >= ( size = sizeof(info) ) )
                return ret+"\n";

            if( entry < size ) {
                if( ( size % cols ) < ( j + 1 ) && ( i + 1 ) == rows &&
                    ( size % cols ) )
                    break;

                ret += info[entry] + sprintf("%:"+( max + 5 -
                       sizeof( strip_colours( info[entry] ) ) )+"s", "");
            }

            if( rows == 1 )
                entry++;
            else
                if( size % cols )
                    entry += ( ( size % cols ) > j ? rows : rows - 1 );
                else
                    entry += rows;
        }
        ret += "\n";
    }

    return ret;

} /* short_print() */

/** @ignore yes */
string directory_listing( string dir, string *lfiles, mapping flags ) {
    string r1, r2, file;
    string *files, *dirs;
    int size, max_dir_length, max_file_length;

    files = ({ });
    dirs = ({ });

    foreach( file in lfiles ) {
        size = file_size( dir + file );

        if( size == -2 ) {
            dirs += ({ file });
            if( sizeof(file) > max_dir_length )
                max_dir_length = sizeof(file);
        } else if( size != -1 ) {
            files += ({ file });
            if( sizeof(file) > max_file_length )
                max_file_length = sizeof(file);
        }
    }

    if( !sizeof(files) && flags["f"] )
        return "No files in '"+dir+"'.\n";

    if( !sizeof(dirs) && flags["d"] )
        return "No directories in '"+dir+"'.\n";

    if( flags["S"] ) {
        switch( flags["n"] ) {
          case 'd':
            files = sort_array( files, "date_sort", TO, dir );
          break;
          case 'e':
            files = sort_array( files, "extension_sort", TO );
          break;
          case 'n':
            dirs  = sort_array( files, "name_sort", TO );
            files = sort_array( dirs, "name_sort", TO );
          break;
          case 's':
            files = sort_array( files, "size_sort", TO, dir );
          break;
        }
    }

    if( flags["l"] ) {
        r1 = "\n";

        foreach( file in dirs ) {
            r1 += sprintf("drwx------   %-3d %-10.10s %-10.10s %8d %12s %s\n",
                sizeof( get_dir( dir + file + "/") ), "user", "user", 1024, "",
                ( flags["o"] ? colorize_dir(file)+"/" : file+"/" ) );
        }

        if( flags["d"] )
            return dir+":\n"+r1;

        r2 = "\n";

        foreach( file in files ) {
            r2 += sprintf("-rw-rw----   %-3d %-10.10s %-10.10s %8d %12s %s%s\n",
                ( size = file_size( dir + file ) ) / 1000 + 1, "user", "user",
                size, ctime( stat( dir + file )[1] )[4..15],
                ( flags["F"] ? ( find_object( dir + file ) ?
                ( flags["o"] ? LOADED_COLOR+"*"+RESET : "*") : "") : ""),
                ( flags["o"] ? colorize_file(file) : file ) );
        }

        if( flags["f"] )
            return dir+":\n"+r2;

        return dir+":\n"+( sizeof(dirs) ? r1 : "")+( sizeof(files) ? r2 : "");

    } else {
        if( flags["o"] ) {
            dirs = map( dirs, (: sprintf("%3d %s/",
                sizeof( get_dir($2+$1+"/") ),
                colorize_dir($1)+RESET ) :), dir );

            if( flags["d"] )
                return dir+":\n"+short_print( dirs, max_dir_length );

            if( flags["F"] )
                files = map( files, (: sprintf("%3d %s%s",
                    file_size($2+$1) / 1000 + 1,
                    ( find_object($2+$1) ? LOADED_COLOR+"*"+RESET : ""),
                    colorize_file($1) ) :), dir );

            else
                files = map( files, (: sprintf("%3d %s",
                    file_size($2+$1) / 1000 + 1,
                    colorize_file($1) ) :), dir );

            if( flags["f"] )
                return dir+":\n"+short_print( files, max_file_length );

        } else {
            dirs = map( dirs, (: sprintf("%3d %s/", sizeof( get_dir($2+$1+"/") ),
                $1 ) :), dir );

            if( flags["d"] )
                return dir+":\n"+short_print( dirs, max_dir_length );

            if( flags["F"] )
                files = map( files, (: sprintf("%3d %s%s",
                    file_size($2+$1) / 1000 + 1,
                    ( find_object($2+$1) ? "*" : ""), $1 ) :), dir );
            else
                files = map( files, (: sprintf("%3d %s",
                    file_size($2+$1) / 1000 + 1, $1 ) :), dir );

            if( flags["f"] )
                return dir+":\n"+short_print( files, max_file_length );
        }

        return dir+":\n"+( sizeof(dirs) ? short_print( dirs,max_dir_length)+
            ( sizeof(files) ? "\n" : "") : "")+( sizeof(files) ?
            short_print( files, max_file_length ) : "");
    }

    return "Directory is empty.\n";

} /* directory_listing() */
