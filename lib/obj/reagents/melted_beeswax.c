// Autogenned file

inherit "/obj/reagents/heatable.c";

void setup(){

// #class
// #debug
  set_name( "beeswax" );
  set_short( "beeswax" );
  set_long( "This is a pool of melted beeswax, $amount_size$.\n" );
  add_adjective( ({ "some", "bees", "bee" }) );
  add_alias( ({ "wax", "beewax" }) );
  set_progress_msg( ({     ({"The beeswax in @@the_short:#env#@@ hardens.\n", 0     }) })  );
  set_medium_alias( "MeltedBeeswax"  );
  set_amount_types( ([ "drop": ({ 1, "drops" }),                          "teaspoon": ({ 20, "teaspoons" }),                          "tablespoon": ({ 60, "tablespoons" }),                          "ounce": ({ 120, "ounces" }),                          "cup": ({ 960, "cups" }),                          "pint": ({ 2400, "pints" }),                          "quart": ({ 4800, "quarts" }),                          "gallon": ({ 19200, "gallons" }) ])  );
  add_adjective( ({ "drop", "drops", "teaspoon", "teaspoons", "tablespoon",                  "tablespoons", "ounce", "ounces", "cup", "cups", "pint",                  "pints", "quart", "quarts", "gallon", "gallons", "of" }) );
  set_amount( 10  );
  set_phase( ({     ({ 25, "morph", "/obj/reagents/solid_beeswax.ob", 0, 1.0,         ({             ({"The beeswax in @@the_short:#env#@@ hardens.\n", 0             })         })     }),     ({ 75, "boom", 0, 0, 1.0,         ({             ({"@@the_short:#env#@@ explodes!\n", 0             })         })      }) })  );
  set_hp_base( 20  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_liquid( );
} /* setup() */
