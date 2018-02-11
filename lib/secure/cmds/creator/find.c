/**
 * @change Added the ability to extract code from included files
 * - Sandoz, 2002.
 */

#include <creator.h>

#define WIZ          WIZ_PRESENT
#define EXTRACT_CODE 1
#define LINE_NUMBERS 2

inherit COMMAND_BASE;

/** @ignore yes */
string *get_includes( string file, string file_name ) {
    string *includes, *lines, line, name, *bits, stuff;
    int i;

    lines = ({ });

    foreach( line in explode( file, "\n") )
        if( line[0..8] == "#include ")
            lines += ({ line[9..] });

    includes = ({ });

    foreach( line in lines ) {
        if( line[0] == '\"' ) {
            bits = explode( file_name, "/");
            sscanf( line, "\"%s\"", name );
            stuff = read_file( implode( bits[0..<2], "/")+"/"+name );
            if( stuff )
                includes += ({ "/"+implode( bits[0..<2], "/")+"/"+name });
         } else if( line[0] == '<') {
            sscanf( line, "<%s>", name );
         }

         if( name[0] == '/' ) {
             stuff = read_file( name );
             if( stuff )
                 includes += ({ name });
         }

         bits = master()->define_include_dirs();
         while( !stuff && i < sizeof(bits) ) {
             stuff = read_file( sprintf( bits[i], name ) );
             if( stuff )
                 includes += ({ sprintf( bits[i], name ) });
             i++;
         }
    }

    return includes;

} /* get_includes() */

/** @ignore yes */
mixed extract_code( string thing, mixed fnd, int inc, int num ) {
    string str, func, ping, file, *includes, bit;
    int i, startline, j;
    mixed tmp;

    ping = ( !inc ? fnd[1] : fnd[1]+".c");
    func = fnd[0];


    if( !file_exists(ping) )
        return notify_fail("The file for "+thing+" doesn't exist.\n");

    file = read_file(ping);

    if( !strlen(file) )
        return notify_fail("Could not read file.\n");

    tmp = reg_assoc( file,  ({"\n[a-z_ \t]*[\\* ][ \t]*"+func+
        "[ \t]*\\([a-zA-Z0-9, _\\*\n\t]*(...)?[ \t]*\\)[ \t\n]*{",
        "\n}" }), ({ 1, 2 }) );

    while( tmp[1][j] != 1 ) {
        j++;
        // Must be an included function.
        if( j == sizeof(tmp[1]) ) {
            if( inc ) {
                includes = get_includes( file, fnd[1] );
                if( sizeof(includes) ) {
                    foreach( bit in includes )
                        if( str = extract_code( bit, ({ func, bit }), 0, num ) )
                            return str;
                }
            }
            return notify_fail("Could not extract code from an "
                "included function.\n");
        }
    }

    if( num )
        startline = sizeof( explode( implode( tmp[0][0..j], ""),
            "\n") );

    j++;

    str = "";

    if( !inc )
        str += "*** Extracted from "+ping+"\n";

    while( tmp[0][j][i] != '\n' ) {
        if( tmp[0][j][i] == '}' ) {
            tmp = tmp[0][j - 1][1..] + tmp[0][j][0..i];
            if( num )
                tmp = implode( explode( tmp, "\n"),
                    (: ({ $1[0]+1, $1[1]+"\n"+$1[0]+" "+$2 }) :),
                    ({ startline, ""}) )[1];
            return ( tmp[0] == '\n' ? tmp[1..] : tmp );
        }
        i++;
    }

    tmp = implode( tmp[0][j-1..j+1], "");

    if( num )
        tmp = implode( explode( tmp, "\n"),
            (: ({ $1[0]+1, $1[1]+"\n"+$1[0]+" "+$2 }) :),
            ({ startline, ""}) )[1];

    str += ( tmp[0] == '\n' ? tmp[1..] : tmp );

    return str;

} /* extract_code() */

/** @ignore yes */
int cmd( string str ) {
    string tmp, func, thing, s, ping;
    object ob, *obs, fish;
    mixed fnd;
    int flags;

    notify_fail("Usage: find [-d] [-n] function() <object(s)>\n");

    if( !str )
        return 0;

    while( sscanf( str, "-%s %s", s, str ) == 2 )
      switch( s ) {
      case "d":
        flags |= EXTRACT_CODE;
      break;
      case "n":
        flags |= (LINE_NUMBERS | EXTRACT_CODE);
      break;
      default:
        return notify_fail("Unknown option '"+s+"'.\n");
    }

    if( sscanf( str, "%s() %s", func, thing ) != 2 &&
        sscanf( str, "%s %s", func, thing ) != 2 )
        return 0;

    obs = WIZ->wiz_present( thing, TP );

    if( !sizeof(obs) )
        return notify_fail("Can't find "+thing+".\n");

    s = "";
    fnd = ({ });

    foreach( ob in obs ) {
        if( ping = function_exists( func, ob, 1 ) ) {
            s += "*** "+WIZ->desc_object(ob)+": "+
                 func+"() found in "+ping+"\n";
            fnd += ({ func, ping });
        } else {
            s += "*** "+WIZ->desc_object(ob)+": "+
                 func+"() not found.\n";
        }

        fish = ob;

        while( fish = shadow( fish, 0 ) )
            if( function_exists( func, fish, 1 ) ) {
                s += "      Shadowed by "+file_name(fish)+"\n";
                fnd += ({ func, base_name(fish) });
            }
    }

    /*
     * If we are going to extract a function, there are many different
     * possible formats it might be declared in:
     *   void foo*(*)*{
     * Take * as indicating where whitespace might split the header,
     * whitespace possibly being a space, a tab or a new line.
     */
    if( ( flags & EXTRACT_CODE ) && sizeof(fnd) ) {
        if( sizeof(fnd) > 2 )
            s += "Can only extract code from one object at a time.\n";
        else if( !tmp = extract_code( thing, fnd, 1,
            ( flags & LINE_NUMBERS ) ) )
            return 0;
        s += tmp;
    }

    TP->more_string(s);
    return 1;

} /* cmd() */
