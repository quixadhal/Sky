// Autogenned file

inherit "/obj/cont_medium.c";

void setup(){

// #class
  set_name( "dust" );
  set_short( "sparkly dust" );
  set_long( "This is $pile_size$ of sparkly dust, $amount_size$.\n" );
  add_adjective( "some" );
  add_adjective( ({"diamond", "sparkly"}) );
  add_plural( "sparkly dust" );
  set_medium_alias( "DiamondDust"  );
  set_amount_types( ([ "pinch": ({ 1, "pinches" }), "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "pinch", "pinches", "handful", "handfuls", "of" }) );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_continuous(  );
  set_amount( 10 );
} /* setup() */
