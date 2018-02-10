// Autogenned file

inherit "/obj/reagents/transient.c";

void setup(){

// #class
// #debug
  set_name( "liquid" );
  set_short( "glowing liquid" );
  add_adjective( "glowing" );
  set_long( "This is a quantity of glowing liquid, $amount_size$.\n" );
  set_progress_msg( ({     ({"The liquid in the @@short:#env#@@ begins to glow faintly.\n", ({         "set_short:#obj#|faintly glowing liquid"})}),     ({"The liquid begins to glow brightly.\n", ({         "set_short:#obj#|brightly glowing liquid"})}),     ({"The liquid flashes brilliantly, and then goes out.\n", 0}) })  );
  set_medium_alias( "GenericShimmer"  );
  set_amount_types( ([ "drop": ({ 1, "drops" }),                          "teaspoon": ({ 20, "teaspoons" }),                          "tablespoon": ({ 60, "tablespoons" }),                          "ounce": ({ 120, "ounces" }),                          "cup": ({ 960, "cups" }),                          "pint": ({ 2400, "pints" }),                          "quart": ({ 4800, "quarts" }),                          "gallon": ({ 19200, "gallons" }) ])  );
  add_adjective( ({ "drop", "drops", "teaspoon", "teaspoons", "tablespoon",                  "tablespoons", "ounce", "ounces", "cup", "cups", "pint",                  "pints", "quart", "quarts", "gallon", "gallons", "of"  }) );
  set_amount( 10  );
  set_hp_base( 0  );
  set_final( "morph"  );
  set_duration( 15  );
  set_result( "/std/object"  );
  set_continuous(  );
  set_liquid(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" }) );
} /* setup() */
