/**
 * This is an inherit writing pads.
 * @author Sandoz, 31-Aug-2002.
 */

inherit OBJECT_OBJ;

private int sheets, left, s_val;
private string paper_ob;

/**
 * This method returns 1 to notify that we can be written on,
 * or 0 if we have no paper left, and cannot be written on.
 * @return 1 if we can be written on, 0 if not
 */
int query_writeable() { return left > 0; }


/**
 * This method returns the number of sheets in this pad.
 * @return the number of sheets in the pad
 */
int query_no_sheets() { return sheets; }

/**
 * This method returns the number of sheets left in this pad.
 * @return the number of sheets left in the pad
 */
int query_sheets_left() { return left; }

/**
 * This method returns the paper object of the pad.
 * @return the paper object we are using
 */
string query_paper_ob() { return paper_ob; }

private void update_pad( object ob ) {
    if( ob ) {
        set_weight( ob->query_weight() * ( left + 4 ) );
        set_value( ob->query_value() * left * s_val / 100 );
        set_max_size( ob->query_max_size() );
    } else if( paper_ob && ( ob = ARMOURY_H->request_item( paper_ob, 100 ) ||
        clone_object( paper_ob ) ) ) {
        set_weight( ob->query_weight() * ( left + 4 ) );
        set_value( ob->query_value() * left * s_val / 100 );
        set_max_size( ob->query_max_size() );
        ob->dest_me();
    } else {
        write( poss_short()+" seems to have lost its paper object!\n");
    }
} /* update_pad() */

/**
 * This method sets the paper object, the number of sheets, the
 * percentual value of a single sheet in the pad.  The percentual value
 * is to be a percentage of the full value of a single sheet of paper
 * (e.g. you could get 1 sheet for 50 copper, 10 sheets for 500 copper,
 * while in a pad you would get 10 sheets for 400 copper with a discount
 * figure of 80).
 * The total value of the pad will be calculated as follows:<br>
 * sheet_value * num_of_sheets * percentual_figure / 100
 * @param str the filename of paper object to set
 * @param num the number of sheets in the pad (max 100)
 * @param val the optional percentual value the sheets in this pad
 * should have (default 80)
 */
varargs void setup_pad( string str, int num, int val ) {
    paper_ob = str;

    if( num > 100 )
        num = 100;

    if( num < 1 )
        num = 1;

    sheets = left = num;

    if( !val || val < 1 )
        val = 100;

    s_val = val;

} /* setup_pad() */

void create() {
    ::create();

    add_extra_look(TO);

    if( function_exists("setup", TO ) )
        update_pad( 0 );

} /* create() */

/** @ignore yes */
int do_tear() {
    object ob;
    string mess;

    if( !left ) {
        add_failed_mess("$D $V$0=seems,seem$V$ to have no paper left.\n");
        return 0;
    }

    if( !paper_ob || !( ob = ARMOURY_H->request_item( paper_ob, 100 ) ||
        clone_object( paper_ob ) ) ) {
        add_failed_mess("Bugger, $D $V$0=seems,seem$V$ to have lost "
            "$V$0=its,their$V$ paper object.  Please file a bug report.\n");
        return 0;
    }

    ob->set_read_mess( query_read_mess() );
    set_read_mess( ({ }) );

    if( --left )
        update_pad( ob );
    else
        move("/room/rubbish");

    add_succeeded_mess("");
    ob->move(TP);

    mess = "You tear "+( !left ? "the last "+ob->short() :
           ob->a_short() )+" off "+the_short();

    if( !left )
        mess += " and toss aside the remains";

    tell_object( TP, mess+".\n");
    tell_room( ENV(TP), TP->the_short()+" $V$0=tears,tear$V$ "+
        ob->a_short()+" from "+a_short()+".\n", TP );

    return 1;

} /* do_tear() */

/** @ignore yes */
void init() {
    add_command("tear", "[a] {sheet} {off|from} <direct:object>");
} /* init() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    mapping map;

    map = ([ "::" : ::int_query_static_auto_load() ]);

    if( s_val )
        map["s_val"] = s_val;
    if( sheets )
        map["sheets"] = sheets;
    if( paper_ob )
        map["paper_ob"] = paper_ob;

    return map;

} /* query_static_auto_load() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return ([ ]);
} /* query_static_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( mapp( map ) ) {
        if( map["::"] )
            ::init_static_arg( map["::"] );
        if( map["s_val"] )
            s_val = map["s_val"];
        if( map["sheets"] )
            sheets = map["sheets"];
        if( map["paper_ob"] ) {
            paper_ob = map["paper_ob"];
            update_pad( 0 );
        }
    }
} /* init_static_arg() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;

    map = ([ "::" : ::query_dynamic_auto_load() ]);

    if( left )
        map["left"] = left;

    return map;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg( mapping map ) {
    if( mapp( map ) ) {
        if( map["::"] )
            ::init_dynamic_arg( map["::"] );
        left = map["left"];
    }

    if( paper_ob )
        update_pad( 0 );

} /* init_dynamic_arg() */

/** @ignore yes */
string extra_look( object ob ) {
    if( ob == TO ) {
        string str;
        int i;

        switch( left ) {
          case 0:
            return "There seem to be no more sheets left in "+
                the_short()+".\n";
          case 1..3:
            return "Only "+query_num(left)+" sheet"+( left > 1 ? "s" :
                   "")+" of paper "+( left > 1 ? "are" : "is")+" still "
                   "left in "+the_short()+".\n";
          default:
            switch( 100 * left / sheets ) {
              case 0..10:
                str = "Nearly all sheets have";
              break;
              case 11..25:
                str = "More than two thirds of the sheets have";
              break;
              case 26..40:
                str = "More than half of the sheets have";
              break;
              case 41..60:
                str = "About half of the sheets have";
              break;
              case 61..75:
                str = "Less than half of the sheets have";
              break;
              default:
                switch( i = sheets - left ) {
                  case 0:
                    str = "No sheets have";
                  break;
                  case 1..3:
                    str = "Only "+query_num(i)+" sheet"+( i > 1 ?
                          "s have" : " has");
                  break;
                  default:
                    str = "Merely a few sheets have";
                }
            }
            return str + " been torn out of "+the_short()+".\n";
        }
    } else {
        return "";
    }
} /* extra_look() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({
        ({"paper ob", paper_ob }),
        ({"sheets", sheets }),
        ({"sheets left", left }),
        ({"sheet value", s_val }),
    });
} /* stats() */
