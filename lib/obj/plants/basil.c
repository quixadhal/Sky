// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "basil" );
  set_short( "basil" );
  add_property( "determinate", "some " );
  set_long( "This is $pile_size$ of basil, $amount_size$.\n" );
  add_adjective( ({ "some" }) );
  add_property( "determinate", "some "    );
  add_plural( "basil" );
  set_medium_alias( "Basil"  );
  set_amount_types( ([ "sprig": ({ 1, "sprigs" }),                          "bunch": ({ 50, "bunches" }) ])  );
  add_adjective( ({ "sprig", "sprigs", "bunch", "bunches", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 }) );
} /* setup() */
