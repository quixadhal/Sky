// Autogenned file

inherit "/obj/reagents/heatable.c";

void setup(){

// #class
// #debug
  set_name( "beeswax" );
  set_short( "beeswax" );
  set_long( "This is a small lump of beeswax.\n" );
  add_adjective( ({ "some", "bees", "bee" }) );
  add_alias( ({ "wax", "beewax" }) );
  set_progress_msg( ({     ({"The beeswax in @@the_short:#env#@@ melts.\n", 0     }) })  );
  set_medium_alias( "PowderedBeeswax"  );
  set_medium_aliases( ({ "", "PowderedBeeswax" })  );
  set_amount_types( ([ "pinch": ({ 1, "pinches" }),                          "handful": ({ 50, "handfuls" }) ])  );
  set_longs( ({ "This is a small lump of beeswax.\n",     "This is $pile_size$ of powdered beeswax, $amount_size$.\n" })  );
  set_shorts( ({ "beeswax", "powdered beeswax" })  );
  add_adjective( ({ "pinch", "pinches", "handful", "handfuls" }) );
  set_amount( 10  );
  set_phase( ({ 0,     ({ 50, "morph", "/obj/reagents/melted_beeswax.ob", 0, 1.0,         ({             ({"The beeswax in @@the_short:#env#@@ melts.\n", 0             })         })     }) })  );
  reset_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" }) );
} /* setup() */
