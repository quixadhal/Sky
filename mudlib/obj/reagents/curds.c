// Autogenned file

inherit "/obj/food.c";

void setup(){

// #debug
// #class
  set_name("curds" );
  set_short("curds" );
  add_plural("curds" );
  set_main_plural("curds" );
  add_adjective(({"some", "drop", "drops", "globule", "globules",   "tablespoonful", "tablespoonfuls", "lump", "lumps", "of"}) );
  set_long("This is some curds, $amount_size$.\n" );
  set_medium_alias("CuRdS"  );
  set_amount_types(([   "drop": ({1, "drops"}),    "globule": ({20, "globules"}),   "tablespoon": ({60, "tablespoons"}),   "lump": ({120, "lumps"})])  );
  set_amount(2400  );
  set_continuous(  );
  set_pile_types( ({50, "small", 100, "medium", "large"}) );
} /* setup() */
