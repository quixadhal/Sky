// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "leaf" );
  set_short( "ground comfrey leaf" );
  set_long( "This is $pile_size$ of ground comfrey leaf, $amount_size$." );
  add_adjective( ({ "some", "comfrey", "ground" }) );
  add_plural( "leaves" );
  add_alias( "comfrey" );
  set_medium_alias( "GroundComfreyLeaf"  );
  set_amount_types( ([ "pinch": ({ 1, "pinches" }),         "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "pinch", "pinches", "handful", "handfuls", "of" }) );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_amount( 1  );
  set_continuous( );
} /* setup() */
