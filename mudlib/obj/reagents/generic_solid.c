// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  add_adjective( "some" );
  set_amount_types( ([ "pinch": ({ 1, "pinches" }),                          "handful": ({ 50,"handfuls" })])  );
  add_adjective( ({ "pinch", "pinches", "handful", "handfuls", "of" }) );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_continuous( );
} /* setup() */
