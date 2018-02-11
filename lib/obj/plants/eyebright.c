// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "eyebright" );
  set_short( "eyebright" );
  set_long( "This is $pile_size$ of eyebright, $amount_size$.\n" );
  add_adjective( ({ "some" }) );
  add_property( "determinate", "some " );
  add_plural( "eyebright" );
  set_medium_alias( "Eyebright"  );
  set_amount_types( ([ "stalk": ({ 1, "stalks" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "stalk", "stalks", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 }) );
} /* setup() */
