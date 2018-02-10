// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "clover" );
  set_short( "pink clover" );
  set_long( "This is $pile_size$ of pink clover, $amount_size$.\n" );
  add_adjective( ({ "some", "pink" }) );
  add_property( "determinate", "some " );
  add_plural( "clover" );
  set_medium_alias( "PinkClover"  );
  set_amount_types( ([ "blossom": ({ 1, "blossoms" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "blossom", "blossoms", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" }) );
} /* setup() */
