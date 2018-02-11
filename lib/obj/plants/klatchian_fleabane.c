// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "fleabane" );
  set_short( "Klatchian fleabane" );
  set_long( "This is $pile_size$ of Klatchian fleabane, $amount_size$." );
  add_adjective( ({ "some" }) );
  add_property( "determinate", "some " );
  add_plural( "fleabane" );
  set_medium_alias( "KlatchianFleabane"  );
  set_amount_types( ([ "stalk": ({ 1, "stalks" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "stalk", "stalks", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 }) );
} /* setup() */
