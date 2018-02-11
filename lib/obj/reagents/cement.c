// Autogenned file

inherit "/obj/cont_medium.c";

void setup(){

// #class
  set_name( "cement" );
  set_short( "cement dust" );
  set_long( "This is $pile_size$ of cement dust, $amount_size$.\n" );
  add_adjective( "some" );
  add_adjective( "dust" );
  add_plural( "cement dust" );
  set_medium_alias( "CementDust"  );
  set_amount_types( ([ "pinch": ({ 1, "pinches" }),          "handful": ({ 50, "handfuls" }),          "sackful": ({ 170400, "sackfuls" }) ])  );
  add_adjective( ({ "pinch", "pinches", "handful", "handfuls",         "sackful", "sackfuls", "of" }) );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_continuous(  );
  set_amount( 170400 );
} /* setup() */
