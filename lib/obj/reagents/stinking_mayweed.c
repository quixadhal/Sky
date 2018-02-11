// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "mayweed" );
  set_short( "stinking mayweed" );
  add_adjective( ({ "some", "stinking" }) );
  add_alias( "flower" );
  set_long( "This is a weedy plant with evil-smelling leaves and " +          "daisy-like flower heads.\n" );
  set_medium_alias( "GroundStinkingMayweed"  );
  set_amount_types( ([ "pinch": ({ 1, "pinches" }),                          "handful": ({ 50,"handfuls" })])  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_medium_aliases( ({ "", "", "PowderedStinkingMayweed" })  );
  set_longs( ({ "",     "This is $pile_size$ of a smelly coarsely-ground flower.\n",     "This is $pile_size$ of brownish-green powder.  The odor is very strong.\n" })  );
  set_shorts( ({ "", "ground stinking mayweed",                    "powdered stinking mayweed" })  );
  set_amount( 10  );
  reset_continuous( );
} /* setup() */
