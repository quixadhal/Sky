/**
 * This is the basic sealable packet code split from the envelopes
 * and rewritten a little - Sandoz, 2002.
 * @author Lemming
 */

#define UNSEALED  1
#define SEALED    2
#define OPENED    3

inherit BAGGAGE_OBJ;

nosave int status;

/** @ignore */
void create() {
    ::create();
    add_extra_look(TO);
    status = UNSEALED;
} /* create() */

/** @ignore */
void init() {
    ::init();
    add_command("seal");
    add_command("unseal");
} /* init() */

/**
 * This method queries whether or not this object is sealed.
 * @return 1 if we are sealed, 0 if not
 */
int query_sealed() { return status == SEALED; }

/** @ignore yes */
int do_not_sell() { return status != UNSEALED; }

/** @ignore yes */
string extra_look( object ob ) {
    if( ob != TO )
        return "";

    switch( status ) {
      case UNSEALED :
        return "It has a sticky flap at one end which can be used to seal it"+
            ( !sizeof(INV(TO)) ? ", though it might be wise to put something "
              "into it first" : "")+".\n";
      case SEALED :
        return "The sticky flap seems to have been sealed down, just waiting "
               "to be unsealed.\n";
      case OPENED :
        return "It looks like it has been sealed and then opened up.\n";
      default :
        return "It is completely broken - you had better tell a creator.\n";
    }

} /* extra_look() */

/** @ignore */
int do_seal() {
    if( status != UNSEALED ) {
        add_failed_mess("$D $V$0=has,have$V$ already been sealed.\n");
        return 0;
    }

    do_close();
    set_stuck( 1 );
    status = SEALED;

    add_succeeded_mess("$N lick$s the flap on $D and seal$s it down.\n");
    return 1;

} /* do_seal() */

/** @ignore */
int do_unseal() {
    if( status != SEALED ) {
        add_failed_mess("$D $V$0=has,have$V$ not been sealed.\n");
        return 0;
    }

    set_stuck(0);
    do_open();
    status = OPENED;

    add_succeeded_mess("$N tear$s back the flap on $D and open$s it.\n", ({ }) );
    return 1;

} /* do_unseal() */

/**
 * This method returns the status of the object.
 * @return 1 for unsealed, 2 for sealed, 3 for sealed and torn open
 */
int query_envelope_status() {
    return status;
} /* query_envelope_status() */

/** @ignore */
mixed stats() {
    string tmp;

    switch( status ) {
      case UNSEALED :
        tmp = "unsealed";
      break;
      case SEALED :
        tmp = "sealed";
      break;
      case OPENED :
        tmp = "torn open";
      break;
      default:
    }

    return ::stats() + ({ ({ "status", tmp }), });

} /* stats() */

/** @ignore */
mapping query_dynamic_auto_load() {
    mapping map;

    map = ([ "::" : ::query_dynamic_auto_load() ]);

    if( status )
        map["status"] = status;

    return map;

} /* query_dynamic_auto_load() */

/** @ignore */
void init_dynamic_arg( mapping map, mixed dummy ) {
    if( map["::"] )
        ::init_dynamic_arg( map["::"], dummy );
    if( map["status"] )
        status = map["status"];
} /* init_dynamic_arg() */

/** @ignore */
mixed query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return ([ ]);
} /* query_static_auto_load() */
