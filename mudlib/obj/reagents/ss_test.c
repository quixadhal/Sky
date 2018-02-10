// Autogenned file

inherit "/obj/reagents/second_sight.c";

void setup(){

// #class
// #debug
  set_name( "liquid" );
  set_short( "cloudy liquid" );
  add_adjective( "cloudy" );
  set_long( "This is a quantity of cloudy liquid, $amount_size$.\n" );
  set_progress_msg( ({     ({"The liquid in @@the_short:#env#@@ suddenly turns clear.\n", 0}) })  );
  set_medium_alias( "SecondSight"  );
  set_amount_types( ([ "drop": ({ 1, "drops" }),                          "teaspoon": ({ 20, "teaspoons" }),                          "tablespoon": ({ 60, "tablespoons" }),                          "ounce": ({ 120, "ounces" }),                          "cup": ({ 960, "cups" }),                          "pint": ({ 2400, "pints" }),                          "quart": ({ 4800, "quarts" }),                          "gallon": ({ 19200, "gallons" }) ])  );
  add_adjective( ({ "drop", "drops", "teaspoon", "teaspoons", "tablespoon",                  "tablespoons", "ounce", "ounces", "cup", "cups", "pint",                  "pints", "quart", "quarts", "gallon", "gallons", "of"  }) );
  set_amount( 10  );
  set_hp_base( 0  );
  set_final( "morph"  );
  set_duration( 15  );
  set_result( "/obj/reagents/water.ob"  );
  set_continuous(  );
  set_liquid(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" }) );
} /* setup() */
