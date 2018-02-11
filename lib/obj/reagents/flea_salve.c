// Autogenned file

inherit "/obj/salve.c";

void setup(){

// #class
  set_name( "salve" );
  set_short( "grey salve" );
  set_long( "This is $pile_size$ of grey salve, $amount_size$.  It is very..." +     " um... aromatic.\n" );
  add_plural( "salve" );
  add_adjective( ({ "some", "grey" }) );
  set_amount_types( ([ "drop": ({ 1, "drops" }),                          "dab": ({ 20, "dabs" })                       ])  );
  add_adjective( ({ "drop", "drops", "dab", "dabs", "of" }) );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  add_apply_effect( "/std/effects/external/flea_cure", 3  );
  set_continuous( );
} /* setup() */
