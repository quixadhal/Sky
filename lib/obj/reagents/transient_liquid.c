// Autogenned file

inherit "/obj/reagents/transient.c";

void setup(){

// #class
  add_adjective( "some" );
  set_amount_types( ([ "drop": ({ 1, "drops" }),                          "teaspoon": ({ 20, "teaspoons" }),                          "tablespoon": ({ 60, "tablespoons" }),                          "ounce": ({ 120, "ounces" }),                          "cup": ({ 960, "cups" }),                          "pint": ({ 2400, "pints" }),                          "quart": ({ 4800, "quarts" }),                          "gallon": ({ 19200, "gallons" }) ])  );
  add_adjective( ({ "drop", "drops", "teaspoon", "teaspoons", "tablespoon",                  "tablespoons", "ounce", "ounces", "cup", "cups", "pint",                  "pints", "quart", "quarts", "gallon", "gallons", "of" }) );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_continuous(  );
  set_liquid( );
} /* setup() */
