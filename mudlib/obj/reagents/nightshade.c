// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "nightshade" );
  set_short( "deadly nightshade" );
  set_long( "This is $pile_size$ of deadly nightshade, $amount_size$.\n" );
  add_adjective( ({ "some", "deadly" }) );
  add_plural( "nightshade" );
  set_medium_alias( "GroundDeadlyNightshade"  );
  add_eat_effect( "/std/effects/ingested/poison.c", 2000  );
  set_amount_types( ([ "pinch": ({ 1, "pinches" }), "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "pinch", "pinches", "handful", "handfuls", "of" }) );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_continuous(  );
  set_amount( 3 );
} /* setup() */
