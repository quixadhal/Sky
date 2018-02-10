/**
 * Nanvaent Industries International
 * LPC: Search and Replace
 * Written by  : Weazel@Nanvaent Nov 92
 * Modified by : Bill@Nanvaent Jan 93
 * Modified by : Bill@Nanvaent+@Discworld Mar 93
 */

#include <creator.h>

inherit COMMAND_BASE;

/** @ignore yes */
int cmd( string str ) {
    string *files, file;
    string s1, s2, s3, s4;
    mixed things;

    seteuid( geteuid(TP) );

    if( !str )
        return notify_fail("Usage: sar <search_string> <replace_string> "
            "<files>\n");

    s4 = str[ 0 .. 0 ];

    if( sscanf( str, s4+"%s"+s4+" "+s4+"%s"+s4+" %s", s1, s2, s3 ) != 3 )
        if( sscanf( str, "%s %s %s", s1, s2, s3 ) != 3 ) {
            return notify_fail("Usage: sar <search_string> <replace_string> "
                "<files>\n");
    }

    if( s1 == s2 )
        return notify_fail("Search string is the same as the replace string, "
            "not replacing anything.\n");

    if( strsrch( s3, "*") == -1 ) {
        if( sizeof( things = WIZ_PRESENT->wiz_present( s3, TP ) ) ) {
            s3 = file_name(things[0]);
            sscanf( s3, "%s#%*d", s3 );
            if( !file_exists(s3) )
                s3 += ".c";
        }
    }

    files = (string *)TP->get_files(s3);

    if( !sizeof(files) )
        return notify_fail("File(s) "+s3+" not found.\n");

    s1 = replace( s1, "^M", "\n");
    s2 = replace( s2, "^M", "\n");

    foreach( file in files ) {
        if( !file_exists(file) )
            continue;
        s4 = read_file(file);
        if( s4 && s4 != "" ) {
            s4 = replace( s4, s1, s2 );
            rm(file);
            write_file( file, s4 );
            printf("Finished processing %s.\n", file );
        } else
            printf("Failed to process %s - no contents.\n", file );
    }

    return 1;

} /* cmd() */
