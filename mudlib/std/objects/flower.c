/**
 * This is an inherit for flowers, it has some decay functionality
 * built in, plus adding custom smell messages/functions/effects
 * will be easy.
 * @author Sandoz, 2001.
 */

#include <bit.h>

#define BRAID_EFFECT EFFECTS_DIR "/other/braided_effect"

inherit OBJECT_OBJ;

mixed smell_mess;
mixed smell_func;
string smell_eff;
string withered_long;
int decay;

int do_braid( object *obs );

/** @ignore yes */
void create() {
    decay = 1000;
    ::create();
    add_alias("flower");
    add_plural("flowers");
    add_extra_look(TO);
    BIT_CONTROLLER->add_bit(TO);
} /* create() */

/**
 * This method sets the maximum decay time for the flower.
 * @param i the decay time to set
 */
void set_decay( int i ) { decay = i; }

/** @ignore yes */
int query_flower() { return 1; }

/**
 * This method returns the withered long of the flower.
 * @return the withered long description
 */
string query_withered_long() { return withered_long; }

/**
 * This method will set the long description to use when
 * the flower has withered.
 * @param str the long desc to use for the withered state
 */
void set_withered_long( string str ) { withered_long = str; }

/**
 * This method returns the smell message set for the flower.
 * @return the smell message(s)
 */
mixed query_smell_message() { return smell_mess; }

/**
 * This method sets the smell message(s) for the flower.
 * @param mess the message can be either a string or array, using an
 * add_succeeded_mess pattern.
 */
void set_smell_message( mixed mess ) {
    if( pointerp( mess ) ) {
        if( sizeof( mess ) != 2 )
            write( "Smell message must be either a string or an array of "
                   "two elements.\n" );
        else
            smell_mess = mess;
        return;
    }

    if( stringp( mess ) )
        smell_mess = mess;

} /* set_smell_message() */

/**
 * This method returns the smell effect set for the flower.
 * @return the smell effect
 */
string query_smell_effect() { return smell_eff; }

/**
 * This method sets the smell effect for the flower.
 * @param eff the effect to use when the flower is being smelled.
 */
void set_smell_effect( string eff ) { smell_eff = eff; }

/**
 * This method returns the smell function set for the flower.
 * @return the smell function
 */
mixed query_smell_func() { return smell_func; }

/**
 * This method sets the smell function for the flower.
 * This function will be called with the player doing
 * the smelling as an argument.
 * @param func the function to call when the flower is being
 * smelled - it can be either a string, in which case it will
 * be called on the flower object, or an array where the first
 * member would be the filename of the object to call the
 * function on, while the second member will be the function
 * to call.
 */
void set_smell_function( mixed func ) {
    if( pointerp( func ) ) {
        if( sizeof( func ) != 2 )
            write("Smell function must be either a string or an array of "
                  "two elements.\n");
        else
            smell_func = func;
        return;
    }

    if( stringp( func ) )
        smell_func = func;

} /* set_smell_function() */

/** @ignore yes */
string short( int dark ) {
    return ( !decay && !query_property("just withered") ?
             "withered " : "" ) + ::short( dark );
} /* short() */

/** @ignore yes */
string long( string word, int dark ) {
    if( !decay )
        return ( withered_long ? withered_long :
                 ::long() + "It is completely withered.\n" );
    return ::long( word, dark );
} /* long() */

/** @ignore yes */
void do_decay() {
    int rate;
    object cont, env;

    if( !cont = ENV(TO) ) {
        BIT_CONTROLLER->remove_bit(TO);
        return;
    }

    if( ( rate = 5 + (int)cont->query_property( "decay rate" ) ) > 0 )
        decay -= rate;

    if( decay <= 0 ) {
        decay = 0;
        BIT_CONTROLLER->remove_bit(TO);
        add_adjective("withered");
        add_property("just withered", 1, 2 );

        if( !ENV(cont) ) {
            tell_room( cont, one_short()+" $V$0=withers,wither$V$.\n");
            return;
        }

        if( living(cont) ) {
            tell_room( ENV(cont), poss_short()+" $V$0=withers,wither$V$.\n");
            return;
        }

        if( !cont->query_vase() )
            return;

        if( living( env = ENV(cont) ) )
            env = ENV(env);

        tell_room( env, one_short()+" in "+cont->poss_short()+" "
            "$V$0=withers,wither$V$.\n");

    }
} /* do_decay() */

/** @ignore yes */
void init() {
    add_command("smell");
    add_command("braid", "<indirect:living> hair with <direct:object:me>",
        (: do_braid( $1 ) :) );
} /* init() */

/** @ignore yes */
int do_braid( object *obs ) {
    object ob;

    if( sizeof(obs) > 1 ) {
        add_failed_mess("You can only $V one person's hair with $D.\n");
        return 0;
    }

    ob = obs[0];

    if( !living(ob) ) {
        add_failed_mess("$I is not living.\n");
        return 0;
    }

    if( ob->query_cannot_braid() ) {
        add_failed_mess("It appears "+ob->poss_short()+" hair is already "
            "braided full of flowers.\n");
        return 0;
    }

    ob->add_effect( BRAID_EFFECT, TO );
    add_succeeded_mess("$N $V $D into "+( ob == TP ? "$p" :
        ob->poss_short() )+" hair.\n");
    return 1;

} /* do_braid() */

/** @ignore yes */
int do_smell() {
    if( !decay ) {
        add_failed_mess("$D $V$0=appears,appear$V$ to be completely withered "
            "and $V$0=doesn't,don't$V$ smell that much anymore.\n");
        return 0;
    }

    if( smell_eff )
        TP->add_effect( smell_eff );

    if( smell_func ) {
        if( pointerp( smell_func ) )
            call_other( smell_func[0], smell_func[1], TP );
        else
            call_other( TO, smell_func, TP );
    }

    add_succeeded_mess( ( !smell_mess ? "$N $V $D.\n" : smell_mess ) );
    return 1;

} /* do_smell() */

/** @ignore yes */
void dest_me() {
    BIT_CONTROLLER->remove_bit(TO);
    ::dest_me();
} /* dest_me() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping m;

    if( !query_name() || query_name() == "object" )
        return 0;

    m = ([ "::" : ::query_dynamic_auto_load() ]);

    if( decay )
        m["decay"] = decay;

    return m;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg( mapping m ) {
    if( m["::"] )
        ::init_dynamic_arg( m["::"] );
    decay = m["decay"];
    if( !decay ) {
        add_adjective("withered");
        BIT_CONTROLLER->remove_bit(TO);
    }
} /* init_dynamic_arg() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    mapping m;
    m = ([ "::" : ::int_query_static_auto_load() ]);
    if( withered_long )
        m["withered long"] = withered_long;
    if( smell_mess )
        m["smell mess"] = smell_mess;
    if( smell_eff )
        m["smell eff"] = smell_eff;
    if( smell_func )
        m["smell func"] = smell_func;
    return m;
} /* int_query_static_auto_load() */

/** @ignore yes */
void init_static_arg( mapping m ) {
    if( !mappingp( m ) )
        return;
    if( m["::"] )
        ::init_static_arg( m["::"] );
    if( m["withered long"] )
        withered_long = m["withered long"];
    if( m["smell mess"] )
        smell_mess = m["smell mess"];
    if( m["smell eff"] )
        smell_eff = m["smell eff"];
    if( m["smell func"] )
        smell_func = m["smell func"];
} /* init_static_arg() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( !query_name() || query_name() == "object" )
        return 0;
    if( base_name(TO) + ".c" == __FILE__ )
        return int_query_static_auto_load();
    return 0;
} /* query_static_auto_load() */

/** @ignore yes */
string extra_look() {

    if( !decay )
        return "";

    switch( decay / 10 ) {
      case 0..3:
        return "It is completely withered.\n";
      break;
      case 4..20:
        return "It is fairly withered.\n";
      break;
      case 21..50:
        return "It is somewhat withered.\n";
      break;
      case 51..80:
        return "It has started to wither.\n";
      break;
      default:
        return "It looks quite fresh.\n";
    }
} /* extra_look() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({ ({ "decay", decay }), });
} /* stats() */
