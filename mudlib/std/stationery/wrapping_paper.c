/**
 * This is the inherit for wrapping papers which can be wrapped
 * around things.
 *
 * To use the inherit you must call make_wrapping() on the object,
 * to set up the basic parameters of the wrapping paper.
 *
 * @see make_wrapping()
 * @author Sandoz, 2002.
 */

inherit BAGGAGE_OBJ;

#define TINY    10
#define SMALL   20
#define MEDIUM  50
#define LARGE   100
#define HUGE    200
#define MAX     400

nosave int wrapped, moving;
nosave string desc, size;

/** @ignore yes */
string get_long() {
    if( !wrapped ) {
        int number;
        string str;

        number = query_max_weight() / 5;

        switch( number ) {
           case 0 :
             str = "less than half a pound";
           break;
           case 1 :
             str = "about half a pound";
           break;
           case 2 :
             str = "about a pound";
           break;
           case 3 :
             str = "about a pound and a half";
           break;
           case 4 .. 14 :
             str = "about "+query_num( ( number + 1 ) / 2 ) +" pounds";
           break;
           case 15 .. 24 :
             str = "about ten pounds";
           break;
           case 25 .. 34 :
             str = "about fifteen pounds";
           break;
           case 35 .. 149 :
             str = "about "+query_num( ( number + 10 ) / 20 * 10 )+" pounds";
           break;
           case 150 .. 249 :
             str = "about a hundred pounds";
           break;
           default :
             str = "over a hundred pounds";
        }

        return "This is "+add_a(size)+" piece of "+desc+" paper, perfect for "
            "wrapping around goods before posting a parcel, or just making a "
            "present.\nIt looks big and durable enough to fit "+str+" of "
            "goods.\n";
    }

    return "This "+size+" piece of "+desc+" paper appears to have been "
        "wrapped around some goods.  A loose flap invites you to unwrap the "
        "packet and unravel whatever lies within.\n";

} /* get_long() */

/** @ignore */
void create() {
    ::create();
    set_stuck( 1 );
} /* create() */

/**
 * This method queries whether this object is a parcel
 * acceptable by the post office or not.
 * It returns true if it has been wrapped.
 * @return 1 if wrapped, 0 if not
 */
int query_is_postage_parcel() { return wrapped; }

/** @ignore yes */
void set_wrapped() {
    if( wrapped ) {
        set_name("packet");
        remove_adjective( ({"piece", "of", "wrapping"}) );
        remove_plural("papers");
        add_adjective("paper");
        add_plural("packets");
        set_short( size+" "+desc+" paper packet");
        set_main_plural( size+" "+desc+" paper packets");
    }
} /* get_short() */

/**
 * This method queries whether or not we have been wrapped or not.
 * @return 1 for wrapped, 0 for unused
 */
int query_wrapped() { return wrapped; }

/**
 * This method returns the size string of the paper.
 * @return the size of the paper as a string
 */
string query_size() { return size; }

/**
 * This method returns the extra description of the paper.
 * @return the extra description of the paper
 */
string query_desc() { return desc; }

/** @ignore yes */
int do_not_sell() { return wrapped; }

/**
 * This method sets up the basic parameters of the wrapping paper.
 * The description argument should be a short, adjective-like
 * description for the object, which should not hint at the size
 * of the paper, because that will be added automatically.
 * Capacity should be the maximum weight of the things that could
 * be wrapped with the paper.
 * <br>
 * The size description of the paper will be calculated from the
 * capacity according to the following table:<br>
 * 0..10    - "tiny"<br>
 * 11..20   - "small"<br>
 * 21..50   - "medium-sized"<br>
 * 51..100  - "large"<br>
 * 101..200 - "huge"<br>
 * 201..400 - "enormous"<br>
 * <br>
 * Anything above 200 will give an error message and a 'broken' paper
 * will be created.
 * @param description the short adjective-like description
 * @param capacity the capacity of the wrapping paper
 * @example
 * // Make a "huge brown wrapping paper"
 * make_wrapping("brown", 120 );
 * @example
 * // Make a "small beige wrapping paper"
 * make_wrapping("beige", 8 );
 */
void make_wrapping( string description, int capacity ) {
    if( capacity < 2 )
        capacity = 2;

    desc = description;

    switch( capacity ) {
      case 0..TINY:
        size = "tiny";
      break;
      case TINY+1..SMALL:
        size = "small";
      break;
      case SMALL+1..MEDIUM:
        size = "medium-sized";
      break;
      case MEDIUM+1..LARGE:
        size = "large";
      break;
      case LARGE+1..HUGE:
        size = "huge";
      break;
      case HUGE+1..MAX:
        size = "enormous";
      break;
      default:
        size = "broken";
        capacity = MAX;
        printf("The maximum capacity for a wrapping paper is "+MAX+".\n");
    }

    set_name("paper");
    set_short( size+" piece of "+desc+" wrapping paper");
    set_main_plural( size+" pieces of "+desc+" wrapping paper");
    add_adjective( size+" piece of "+desc+" wrapping");

    set_long( (: get_long() :) );

    set_weight( 1 + capacity / ( LARGE * 25 ) );
    set_value( 100 + to_int( 20 * sqrt( to_float(capacity) ) ) );
    set_max_weight( capacity );

    set_material("paper");
    add_property("writeable", 1 );

} /* make_wrapping() */

/** @ignore yes */
int test_add( object ob, int flag ) {
    if( !moving )
        return 0;
    return ::test_add( ob, flag );
} /* test_add() */

/** @ignore */
int do_wrap( object *obs ) {
    object ob, *failed;
    int i;

    if( wrapped ) {
        add_failed_mess("$D $V$0=has,have$V$ already been wrapped around "
            "something.\n");
        return 0;
    }

    if( !sizeof(obs) ) {
        add_failed_mess("You need to $V $D around something.\n");
        return 0;
    }

    if( ( i = member_array( TO, obs ) ) != -1 ) {
        add_failed_mess("You cannot wrap $I around itself.\n", obs[i..i] );
        return 0;
    }

    if( sizeof( failed = filter( obs, (: living($1) :) ) ) ) {
        add_failed_mess("You cannot $V $D around $I because they are living "
            "things.\n", failed );
        return 0;
    }

    if( sizeof( failed = filter( obs, (: ENV($1) != TP :) ) ) ) {
        add_failed_mess("You don't appear to have $I anymore.\n", failed );
        return 0;
    }

    if( sizeof( failed = filter( obs, (: ENV($1) != TP :) ) ) ) {
        add_failed_mess("You don't appear to have $I anymore.\n", failed );
        return 0;
    }

    if( sizeof( failed = filter( obs, (: $1->query_worn_by() :) ) ) ) {
        add_failed_mess("You cannot $V $D around $I because you are "
            "wearing "+({"it", "them"})[query_group(failed)]+".\n", failed );
        return 0;
    }

    if( sizeof( failed = filter( obs, (: $1->query_wielded() :) ) ) ) {
        add_failed_mess("You cannot $V $D around $I because you are "
            "holding "+({"it", "them"})[query_group(failed)]+".\n", failed );
        return 0;
    }

    if( sizeof( failed = filter( obs, (: $1->query_keep() :) ) ) ) {
        add_failed_mess("You cannot $V $D around $I because you are "
            "keeping "+({"it", "them"})[query_group(failed)]+".\n", failed );
        return 0;
    }

    i = 0;

    foreach( ob in obs )
        i += ob->query_complete_weight();

    if( i > query_max_weight() ) {
        add_failed_mess("$I $V$0=is,are$V$ too large to fit in $D.\n", obs );
        return 0;
    }

    set_stuck( 0 );
    moving = 1;
    obs->move(TO);
    moving = 0;
    do_close();
    set_stuck( 1 );
    wrapped = 1;

    call_out( (: set_wrapped :), 2 );

    add_succeeded_mess("$N carefully $V $D around $I.\n", obs );
    return 1;

} /* do_wrap() */

/** @ignore */
int do_unwrap() {
    object *obs, ob;

    if( !wrapped ) {
        add_failed_mess("$D $V$0=has,have$V$ not been wrapped around "
            "anything.\n");
        return 0;
    }

    set_stuck(0);
    do_open();

    if( !sizeof( obs = INV(TO) ) ) {
        add_succeeded_mess("$N tear$s $D to pieces and, much to $p "
            "surprise, find$s nothing within.\n");
    } else {
        add_succeeded_mess("");
        tell_object( TP, "You tear "+the_short()+" to pieces and find "+
            query_multiple_short( obs, "a")+" enclosed within.\n");
        tell_room( ENV(TP), TP->the_short()+" $V$0=tears,tear$V$ "+
            the_short()+" to pieces and $V$0=finds,find$V$ "+
            query_multiple_short( obs, "a")+" enclosed within.\n", TP );
        if( sizeof( obs = filter( obs, (: $1->move(TP) :) ) ) ) {
            obs->move(ENV(TP));
            tell_object( TP, "Unfortunately you are too burdened and fumble "+
                query_multiple_short(obs)+".\n");
        }
    }

    ob = clone_object(PAPER_OBJ);

    ob->set_short( size+" torn piece of "+desc+" paper");
    ob->set_adjectives( ({ }) );
    ob->add_adjective( size+" torn piece of "+desc );
    ob->set_long("This is "+add_a(size)+", somewhat torn piece of "+
        desc+" paper that seems to have come off a packet or something "
        "similar.\n");

    ob->set_weight( 80 * query_weight() / 100 + 1 );
    ob->set_value( 1 + to_int( 20 * sqrt( to_float(query_max_weight()) ) ) );
    ob->set_read_mess( query_read_mess() );

    if( ob->move(TP) )
        ob->move(ENV(TP));

    move("/room/rubbish");
    return 1;

} /* do_unwrap() */

/** @ignore */
void init() {
    ::init();
    add_command("wrap",
        "<direct:object:me> around <indirect:object:me'things'>",
        (: do_wrap($1) :) );
    add_command("wrap", "<indirect:object:me'things'> in <direct:object:me>",
        (: do_wrap($1) :) );
    add_command("unwrap");
} /* init() */

/** @ignore */
mixed stats() {
    return ::stats() + ({
        ({ "wrapped", wrapped }),
        ({ "desc", desc }),
        ({ "size", size }),
    });
} /* stats() */

/** @ignore yes */
void init_dynamic_arg( mapping map, mixed dummy ) {
    if( map["::"] ) {
        moving = 1;
        ::init_dynamic_arg( map["::"], dummy );
        moving = 0;
    }

    if( map["wrapped"] )
        wrapped = map["wrapped"];

    set_wrapped();
    set_long( (: get_long() :) );

} /* init_dynamic_arg() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;

    map = ([ "::" : ::query_dynamic_auto_load() ]);

    if( wrapped )
        map["wrapped"] = wrapped;

    return map;

} /* query_dynamic_auto_load() */

/** @ignore */
mapping int_query_static_auto_load() {
    mapping map;

    map = ([ "::" : ::int_query_static_auto_load() ]);

    if( desc )
        map["desc"] = desc;
    if( size )
        map["size"] = size;

    return map;

} /* int_query_static_auto_load() */

/** @ignore */
void init_static_arg( mapping map ) {
    if( map["::"] )
        ::init_static_arg( map["::"] );

    if( map["desc"] )
        desc = map["desc"];

    if( map["size"] )
        size = map["size"];

} /* init_static_arg() */

/** @ignore */
mixed query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return ([ ]);
} /* query_static_auto_load() */
