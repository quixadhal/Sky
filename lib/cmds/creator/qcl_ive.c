/**
 * Command to toggle between QC and live rooms.
 * Armando, 22-02-2002
 */

inherit COMMAND_BASE;

int cmd() {
    string place;
    string dom;

    place = base_name( ENV(TP) );

    if( place[0..5] == "/d/qc/" ) {
        place = place[0..2]+place[6..<1];
        dom = CAP( explode( place, "/" )[1] );
        if( file_exists(place+".c") ) {
            tell_object( TP, "Transferring you from QC "+dom+" to live "+
                dom+".\n");
            TP->move_with_look( place, TP->query_mmsgin(), TP->query_mmsgout() );
            return 1;
        }
        add_failed_mess("This room doesn't exist in live "+dom+".\n");
        return 0;
    }

    if( place[0..2] == "/d/" ) {
        dom = CAP( explode( place, "/" )[1] );
        place = place[0..2]+"qc/"+place[3..<1];
        if( file_exists(place+".c") ) {
            tell_object( TP, "Transferring you from live "+dom+" to QC "+
                dom+".\n");
            TP->move_with_look( place, TP->query_mmsgin(), TP->query_mmsgout() );
            return 1;
        }
        add_failed_mess("This room doesn't exist in QC "+dom+".\n");
        return 0;
    }

    add_failed_mess("You are not in a domain room, this command will not "
        "work here.\n");
    return 0;

} /* cmd() */

mixed query_patterns() { return ({"", (: cmd() :) }); }
