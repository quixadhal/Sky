// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "grass" );
  set_short( "grass" );
  set_long( "This is $pile_size$ of green grass, $amount_size$.\n" );
  add_adjective( ({ "some", "green" }) );
  add_property( "determinate", "some " );
  add_plural( "grass" );
  set_medium_alias( "GreenGrass"  );
  set_amount_types( ([ "blade": ({ 1, "blades" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "blade", "blades", "handful", "handfuls", "of" }) );
  set_amount( 5  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" }) );
} /* setup() */
