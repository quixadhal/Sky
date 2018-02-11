/**
 * This is a folder inherit, for putting documents, leaflets, etc. in.
 * The two things you need to set when creating a folder, are the the
 * maximum weight of a single paper that fits in the folder (so we can't
 * put huge papers in small folders that are supposed to only fit heaps
 * of small ones), and the total number of sheets of that maximum size
 * that should fit in the folder.
 *
 * NOTE: If the size is more than 1, then the folder will fit double the
 * amount of papers that are half or less the maximum sheet size.
 *
 * @author Mahray 20/04/02
 * @changed Changed it to work with other documents as well -
 * Sandoz, 10. Sept. 2002.
 *
 * @example
 *     setup_folder( 1, 20 ); // Max size 1, total number 20.
 */

inherit BAGGAGE_OBJ;

int sheets, sheet_size;

/** @ignore yes */
void create() {
    do_setup++;
    baggage::create();
    do_setup--;

    sheets = 10;
    sheet_size = 1;

    set_unlockable( 1 );
    add_extra_look(TO);

    if( !do_setup )
        TO->setup();

} /* create() */

/**
 * This method sets the number of documents this folder can hold,
 * and the maximum sheet size.  Be reasonable.  The maximum number
 * is capped at 100, and the maximum size is capped at 3, no paper
 * should weight more than that.
 * @param size the maximum size of a sheet the folder can hold
 * @param number the amount of documents the folder can hold
 * @see query_sheet_size()
 * @see query_no_sheets()
 */
void setup_folder( int size, int number ) {
    if( number > 100 )
        number = 100;

    if( size > 3 )
        size = 3;

    sheets = number;
    sheet_size = size;

    set_max_weight( number * size );

} /* setup_folder() */

/**
 * This method returns the maximum sheet size this folder can hold.
 * @return the maximum sheet size we can hold
 * @see setup_folder()
 * @see query_no_sheets()
 */
int query_sheet_size() { return sheet_size; }

/**
 * This method returns the number of sheets this folder can hold.
 * @return the number of sheets we can hold
 * @see setup_folder()
 * @see query_sheet_size()
 */
int query_no_sheets() { return sheets; }

/** @ignore yes */
int test_add( object ob, int i ) {
    int ret;

    if( i )
        return 0;

    if( !ENV( ob ) )
        return ::test_add( ob, i );

    if( !ob->query_paper() && !ob->query_leaflet() ) {
        write( ob->the_short()+" "+verbalize("don't", "doesn't")+" appear to "
            "be of the right shape to fit in "+the_short()+".\n");
        return 0;
    }

    if( ob->query_weight() > sheet_size || sizeof(INV(TO)) > 2 * sheets ||
        !( ret = ::test_add( ob, i ) ) ) {
        write( ob->the_short()+" "+verbalize("don't", "doesn't")+" fit very "
            "well in "+the_short()+".\n");
        return 0;
    }

    return ret;

} /* test_add() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    return ([
        "::" : ::int_query_static_auto_load(),
        "sheets" : sheets,
        "sheet size" : sheet_size,
    ]);
} /* int_query_static_auto_load() */

/** @ignore yes */
void init_static_arg( mapping m ) {
    if( mapp( m ) ) {
        if( m["::"] )
            ::init_static_arg( m["::"] );
        if( m["sheets"] )
            sheets = m["sheets"];
        if( m["sheet size"] )
            sheet_size = m["sheet size"];
        set_max_weight( sheets * sheet_size );
    }
} /* init_static_arg() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( query_name() && query_name() != "object" &&
        base_name(TO)+".c" == __FILE__ )
        return int_query_static_auto_load();
    return 0;
} /* query_static_auto_load() */

/** @ignore yes */
string extra_look( object ob ) {
    string size;

    if( ob != TO || !sheets )
        return "";

    switch( sheet_size ) {
      case 1:
        size = "medium-sized";
      break;
      case 2:
        size = "large";
      break;
      case 3:
        size = "huge";
      break;
      default:
        size = "extremely buggy";
    }

    return "It can hold about "+query_num(sheets)+" "+size+" sheet"+
        ( sheets > 1 ? "s" : "")+" of paper.\n";

} /* extra_look() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({
        ({"sheets", sheets }),
        ({"sheet size", sheet_size }),
    });
} /* stats() */
