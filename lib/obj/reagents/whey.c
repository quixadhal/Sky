// Autogenned file

inherit "/obj/food.c";

void setup(){

// #debug
// #class
  set_name("whey" );
  set_short("whey" );
  add_plural("whey" );
  set_main_plural("whey" );
  add_adjective(({"some", "drop", "drops", "teaspoon",   "teaspoons", "tablespoon", "tablespoons", "ounce", "ounces",   "cup", "cups", "pint", "pints", "quart", "quarts", "gallon",   "gallons", "of"}) );
  set_long("This is some whey, $amount_size$.\n" );
  set_medium_alias("WhEy"  );
  set_amount_types(([   "drop": ({1, "drops"}),    "teaspoon": ({20, "teaspoons"}),   "tablespoon": ({60, "tablespoons"}),   "ounce": ({120, "ounces"}),   "cup": ({960, "cups"}),   "pint": ({2400, "pints"}),   "quart": ({4800, "quarts"}),   "gallon": ({19200, "gallons"})])  );
  set_amount(2400  );
  set_continuous(  );
  set_liquid(  );
  set_pile_types( ({50, "small", 100, "medium", "large"}) );
} /* setup() */
