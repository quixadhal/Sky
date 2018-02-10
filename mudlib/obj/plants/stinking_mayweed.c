// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "mayweed" );
  set_short( "stinking mayweed" );
  set_long( "This is $pile_size$ of stinking mayweed, $amount_size$." );
  add_adjective( ({ "some" }) );
  add_property( "determinate", "some " );
  add_plural( "mayweed" );
  set_medium_alias( "StinkingMayweed"  );
  set_amount_types( ([ "stalk": ({ 1, "stalks" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "stalk", "stalks", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 }) );
} /* setup() */
