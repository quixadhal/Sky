// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name("champagne" );
  set_short("champagne" );
  set_long("This is some fine champagne, $amount_size$.\n" );
  add_adjective(({"some", "drop", "drops", "teaspoon", "teaspoons",    "tablespoon", "tablespoons", "ounce", "ounces", "cup",    "cups", "pint", "pints", "quart", "quarts", "gallon", "gallons", "of"}) );
  add_plural("champagne" );
  set_medium_alias("Champagne"  );
  set_amount_types(([   "drop" : ({1, "drops"}),    "teaspoon" : ({20, "teaspoons"}),   "tablespoon" : ({60, "tablespoons"}),   "ounce" : ({120, "ounces"}),   "cup" : ({960, "cups"}),   "pint" : ({2400, "pints"}),   "quart" : ({4800, "quarts"}),   "gallon" : ({19200, "gallons"})])  );
  set_amount(2400  );
  set_continuous(  );
  set_liquid(  );
  set_pile_types(({50, "small", 100, "medium", "large"})  );
  add_eat_effect( "/std/effects/ingested/drunk.c", 50 );
} /* setup() */
