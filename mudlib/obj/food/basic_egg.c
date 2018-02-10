/**
 * This is an egg inherit, which will make it possible for
 * eggs to have additional funcionality compared to regular
 * food objects.  You can crack eggs into containers or throw
 * them at people.
 * @author Sandoz, 20, September 2001.
 */

#define AMOUNT_MOD 300
#define EGG_EFFECT EFFECTS_DIR "/other/covered_in_egg"

inherit FOOD_OBJ;

int cracked;
string creature;

void do_liquidate();
int do_throw( object *obs );
int do_crack( object *obs );
string query_size_string();

/** @ignore yes */
int query_egg_yolk() { return cracked; }

/**
 * This method sets the race/type of creature the egg belongs to.
 * @param who the type of creature to set the egg to
 * @see query_creature
 */
void set_creature( string who ) {
    creature = who;
    set_short( creature+" egg" );
    add_adjective( creature );
} /* set_creature() */

/**
 * This method returns the race/type of creature the egg belongs to.
 * @return the type of creature the egg belongs to
 * @see set_creature
 */
string query_creature() { return creature; }

/** @ignore yes */
void create() {
    ::create();
    set_name("egg");
    set_long("This $size$, oval $creature$ egg would probably taste good "
        "if baked, or might come in handy otherwise.  You could probably "
        "'crack' it into a bowl or similar, or just eat it as it is.\n");
    if( cracked )
        do_liquidate();
} /* create() */

/** @ignore yes */
string long( string words, int number ) {
   return replace( ::long( words, number ), ({
                   "$creature$", creature,
                   "$size$", query_size_string() }) );
}/* long() */

/** @ignore yes */
string query_size_string() {
    switch( query_weight() ) {
      case 0..1:
        return "tiny";
      case 2..4:
        return "small";
      case 5..10:
        return "medium sized";
      case 11..20:
        return "large";
      case 21..40:
        return "huge";
      default:
        return "enormous";
    }
} /* query_size_string() */

/** @ignore yes */
void set_weight( int i ) {
    set_weight_per_bite( i );
    ::set_weight( i );
} /* set_weight() */

/** @ignore yes */
void do_liquidate() {
    set_determinate("some");
    set_name("yolk");
    add_alias("egg");
    set_short( creature+" egg yolk");
    set_long("This is some $creature$ egg yolk, $amount_size$.\n");
    add_adjective( ({ creature+" egg", "some", "drop", "drops", "teaspoon",
        "teaspoons", "tablespoon", "tablespoons", "ounce", "ounces", "cup",
        "cups", "pint", "pints", "quart", "quarts", "gallon", "gallons", "of"
        }) );
    set_liquid();
    set_value_scale( 0.0001 );
    set_medium_alias( CAP(creature)+"EggYolk");
    set_amount( AMOUNT_MOD * query_weight() );
    set_weight_unit( ({ 1, AMOUNT_MOD }) );
    set_pile_types( ({ 50, "small", 100, "medium", "large" }) );
    set_amount_types( ([
        "drop": ({ 1, "drops" }),
        "teaspoon": ({ 20, "teaspoons" }),
        "tablespoon": ({ 60, "tablespoons" }),
        "ounce": ({ 120, "ounces" }),
        "cup": ({ 960, "cups" }),
        "pint": ({ 2400, "pints" }),
        "quart": ({ 4800, "quarts" }),
        "gallon": ({ 19200, "gallons" }) ]) );
} /* do_liquidate() */

/** @ignore yes */
void init() {
    ::init();
    if( !cracked ) {
        add_command("throw",
            "<direct:object:me> [at] <indirect:living:here>",
            (: do_throw( $1 ) :) );
        add_command("crack",
            "<direct:object:me> [into] <indirect:object:me'vessel'>",
            (: do_crack( $1 ) :) );
    }
} /* init() */

/** @ignore yes */
int do_throw( object *obs ) {
    if( sizeof(obs) > 1 ) {
        add_failed_mess("You can only $V $D at one target.\n");
        return 0;
    }
    move("/room/rubbish");
    call_out( (: $1[0]->add_effect( EGG_EFFECT,
        ({ 240 + roll_MdN( sqrt(query_weight()), 120 ), TO }) ) :), 0, obs );
    add_succeeded_mess( "$N $V $D at $I.\n", obs );
    return 1;
} /* do_throw() */

/** @ignore yes */
int do_crack( object *obs ) {
    object ob;
    int space;

    if( cracked ) {
        add_failed_mess("How do you expect to $V a mass of egg "
            "yolk and albumen?\n");
        return 0;
    }

    if( sizeof(obs) > 1 ) {
        add_failed_mess("You can only $V $D into one vessel.\n");
        return 0;
    }

    ob = obs[0];

    if( !ob->query_max_volume() ) {
        add_failed_mess("You can only $V $D into a vessel.\n");
        return 0;
    }

    if( ( space = ob->query_volume_left() ) < 1 ) {
        add_failed_mess( "You can't $V $D into $I, because it is "
            "completely full.\n", obs );
        return 0;
    }

    if( ( space - AMOUNT_MOD * query_weight() ) < 1 ) {
        add_failed_mess("You don't think all of $D would fit into $I.\n",
            obs );
        return 0;
    }

    if( !ob->test_add(TO) ) {
        add_failed_mess("You can't $V $D into $I.\n", obs );
        return 0;
    }

    cracked = 1;

    /* use call_outs so the short desc wouldn't change before the move */
    call_out( "do_liquidate", 0 );
    call_out( (: move($1) :), 1, ob );
    add_succeeded_mess("$N $V open $D and poor$s the contents into $I.\n",
        obs );
    return 1;

} /* do_crack() */

/** @ignore yes */
int do_eat() {
    if( cracked ) {
        add_failed_mess("You cannot $V $D.\n");
        return 0;
    }
    add_succeeded_mess(({ "You crack open $D, suck out and swallow all of "
        "the slightly salty yolk and albumen and lick clean your fingers.\n",
        "$N crack$s open $D, suck$s out all of the yolk and albumen and "
        "lick$s clean $p fingers.\n" }) );
    move("/room/rubbish");
    consume( TP, 0 );
    return 1;
} /* do_eat() */

/** @ignore yes */
int do_cut( int i ) {
    add_failed_mess("You cannot $V $D.\n");
    return 0;
} /* do_cut() */

/** @ignore yes */
int do_mince() {
    add_failed_mess("You cannot $V $D.\n");
    return 0;
} /* do_mince() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;
    if( !query_name() || query_name() == "object" )
        return 0;
    map = ([ "::" : ::query_dynamic_auto_load() ]);
    if( cracked )
        map["cracked"] = cracked;
    return map;
} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg( mapping map ) {
    if( map["::"] )
        ::init_dynamic_arg( map["::"] );
    if( map["cracked"] )
        cracked = map["cracked"];
} /* init_dynamic_arg() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    mapping map;
    map = ([ "::" : ::int_query_static_auto_load() ]);
    if( creature )
        map["creature"] = creature;
    return map;
} /* int_query_static_auto_load() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( !query_name() || query_name() == "object" )
        return 0;
    if( base_name(TO) + ".c" == __FILE__ )
        return int_query_static_auto_load();
    return 0;
} /* query_static_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( !mappingp( map ) )
        return;
    if( map["::"] )
        ::init_static_arg( map["::"] );
    if( map["creature"] )
        creature = map["creature"];
} /* init_static_arg() */

/** @ignore yes */
mixed *stats() {
    return ::stats() + ({
        ({ "creature", creature }),
        ({ "cracked", cracked }) });
} /* stats() */
