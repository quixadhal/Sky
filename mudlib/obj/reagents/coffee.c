// Autogenned file

inherit "/obj/food.c";

void setup(){

// #debug
// #class
  set_name( "coffee" );
  set_short( "coffee grounds" );
  add_alias( "grounds" );
  set_long( "This is $pile_size$ of coffee grounds, $amount_size$.  They've been used, but they still might contain some potency.\n" );
  add_adjective( "some" );
  add_plural( "coffee" );
  set_medium_alias( "GroundCoffee"  );
  set_amount_types( ([ "pinch": ({ 1, "pinches" }), "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "pinch", "pinches", "handful", "handfuls", "of" }) );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_continuous(  );
  set_amount( 50 );
} /* setup() */
