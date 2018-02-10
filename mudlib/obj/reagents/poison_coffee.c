// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "coffee" );
  set_short( "coffee" );
  set_long( "This is some freshly made coffee, $amount_size$.\n" );
  add_adjective( "some" );
  add_plural( "coffee" );
  set_medium_alias( "PoisonCoffee"  );
  add_eat_effect( "/std/effects/ingested/poison.c", 10  );
  set_amount_types( ([ "drop": ({ 1, "drops" }),                          "teaspoon": ({ 20, "teaspoons" }),                          "tablespoon": ({ 60, "tablespoons" }),                          "ounce": ({ 120, "ounces" }),                          "cup": ({ 960, "cups" }),                          "pint": ({ 2400, "pints" }),                          "quart": ({ 4800, "quarts" }),                          "gallon": ({ 19200, "gallons" }) ])  );
  add_adjective( ({ "drop", "drops", "teaspoon", "teaspoons", "tablespoon",                  "tablespoons", "ounce", "ounces", "cup", "cups", "pint",                  "pints", "quart", "quarts", "gallon", "gallons", "of" }) );
  set_continuous(  );
  set_liquid(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" }) );
} /* setup() */
