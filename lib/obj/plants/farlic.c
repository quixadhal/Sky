// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "farlic" );
  set_short( "farlic" );
  set_long( "This is a bulb of farlic.\n" );
  add_property( "determinate", "a " );
  add_plural( "garlic" );
  set_medium_alias( "Farlic"  );
  set_amount( 1  );
  add_eat_effect("/std/effects/ingested/garlic_breath", 200  );
  set_weight( 3  );
  set_weight_per_bite( 1  );
  set_piece_description( "This is a papery, white clove of farlic. "+  " It smells very pungent.\n"  );
  set_piece_short( "clove"  );
  set_piece_plural( "cloves"  );
  set_decay_speed( 1800  );
  add_alias(({"farlic"}));
} /* setup() */
