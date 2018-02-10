// Autogenned file

inherit "/obj/reagents/transient.c";

void setup(){

// #class
// #debug
  set_name( "blob" );
  set_short( "pulsating blob" );
  add_adjective( "pulsating" );
  set_long( "This is a pulsating blob of...something, $amount_size$.\n" );
  set_medium_alias( "MonsterMorph"  );
  set_amount_types( ([ "drop": ({ 1, "drops" }),                          "ounce": ({ 120, "ounces" }),                          "cup": ({ 960, "cups" }),                          "pint": ({ 2400, "pints" }),                          "quart": ({ 4800, "quarts" }),                          "gallon": ({ 19200, "gallons" }) ])  );
  add_adjective( ({ "drop", "drops", "teaspoon", "teaspoons", "tablespoon",                  "tablespoons", "ounce", "ounces", "cup", "cups", "pint",                  "pints", "quart", "quarts", "gallon", "gallons", "of"  }) );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" }) );
} /* setup() */
