// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "convolvus" );
  set_short( "convolvus" );
  set_long( "This is $pile_size$ of convolvus (also known as bindweed), "          "$amount_size$.  It has a long, springy stem and pale "          "funnel-shaped flowers.\n" );
  add_adjective( ({ "some" }) );
  add_property( "determinate", "some " );
  add_plural( "convolvus" );
  add_alias( "bindweed" );
  set_medium_alias( "Convolvus"  );
  set_amount_types( ([ "length": ({ 1, "lengths" }),                          "handful": ({ 10, "handfuls" }) ])  );
  add_adjective( ({ "length", "lengths", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 10, "small", 20, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 }) );
} /* setup() */
