/*
 * Detabber Command
 * Based on the SAR command
 * Shaydz
 */

#include <creator.h>

inherit COMMAND_BASE;

/** @ignore yes */
int cmd( string str ) {
    string *files, file;
    string s3, s4;
    mixed things;

    seteuid( geteuid(TP) );
    s3 = str;

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

    foreach( file in files ) {
        if( !file_exists(file) )
            continue;
        s4 = read_file(file);
        if( s4 || s4 == "" ) {
            s4 = replace( s4, "\t", "    ");
            rm(file);
            write_file( file, s4 );
            printf("Finished processing %s.\n", file );
        } else
            printf("Failed to process %s - no contents.\n", file );
    }

    return 1;

} /* cmd() */

/** @ignore yes */
mixed query_patterns() {
    return ({"<string'files'>", (: cmd($4[0]) :) });
} /* query_patterns() */
