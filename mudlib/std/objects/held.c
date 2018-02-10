/**
 * This is the held item inherit that can be used for simple
 * items that should be able to be held.
 * @author Sandoz, 2002.
 */

inherit OBJECT_OBJ;
inherit "/std/basic/holdable";

/** @ignore yes */
void create() {
    object::create();
    holdable::create();
} /* create() */

/** @ignore yes */
varargs int move( mixed ob, string mess1, string mess2 ) {
    int ret;

    if( ret = holdable::move(ob) )
        return ret;

    return object::move( ob, mess1, mess2 );

} /* move() */

/** @ignore yes */
void dest_me() {
    holdable::dest_me();
    object::dest_me();
} /* dest_me() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    return ([ "::"   : object::query_dynamic_auto_load(),
              "hold" : holdable::query_dynamic_auto_load() ]);
} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg( mapping map, mixed dummy ) {
    if( map["hold"])
        holdable::init_dynamic_arg( map["hold"], dummy );
    ::init_dynamic_arg( map["::"] );
} /* init_dynamic_arg() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    return ([ "::" : object::int_query_static_auto_load(),
              "hold" : holdable::query_static_auto_load() ]);
} /* query_static_auto_load() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( base_name(TO) != __FILE__[0..<3] )
        return ([ ]);
    return int_query_static_auto_load();
} /* query_static_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( mapp( map ) ) {
        if( map["::"] )
            object::init_static_arg( map["::"] );
        if( map["hold"] )
            holdable::init_static_arg( map["hold"] );
    }
} /* init_static_arg() */

/** @ignore yes */
mixed stats() {
    return object::stats() + ({
        ({"held by", query_holder() }),
        ({"no limbs", query_no_limbs() }),
    });
} /* stats() */
