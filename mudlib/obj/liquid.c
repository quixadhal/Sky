/**
 * This is a simple inherit for liquids, just to avoid copying
 * the amount types and adjectives in each and every file.
 * @author Sandoz
 */

inherit FOOD_OBJ;

void create() {
    do_setup++;
    ::create();
    do_setup--;

    add_alias("liquid");

    set_amount_types( ([ "drop": ({ 1, "drops" }),
                         "teaspoon": ({ 20, "teaspoons" }),
                         "tablespoon": ({ 60, "tablespoons" }),
                         "ounce": ({ 120, "ounces" }),
                         "cup": ({ 960, "cups" }),
                         "pint": ({ 2400, "pints" }),
                         "quart": ({ 4800, "quarts" }),
                         "gallon": ({ 19200, "gallons" }),
                         ]) );

    add_adjective( ({"some", "drop", "drops", "teaspoon", "teaspoons",
        "tablespoon", "tablespoons", "ounce", "ounces", "cup", "cups",
        "pint", "pints", "quart", "quarts", "gallon", "gallons", "of"}) );

    set_pile_types( ({ 50, "small", 100, "medium", "large" }) );

    set_liquid();

    if( !do_setup )
        TO->setup();

} /* create() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    return ([ "::" : ::int_query_static_auto_load() ]);
} /* int_query_static_auto_load() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( !query_name() || query_name() == "object" )
        return 0;

    if( base_name(TO) + ".c" == __FILE__ || query_continuous() )
        return int_query_static_auto_load();

    return 0;

} /* query_static_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( !mapp(map) )
        return;
    if( map["::"] )
        ::init_static_arg(map["::"]);
} /* init_static_arg() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    return ([ "::" : ::query_dynamic_auto_load() ]);
} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg( mapping map ) {
    if( !mapp(map) )
        return;
    if( map["::"] )
        ::init_dynamic_arg(map["::"]);
} /* init_dynamic_arg() */
