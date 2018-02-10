// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "garlic" );
  set_short( "wild garlic" );
  set_long( "This is a bulb of garlic.\n" );
  add_property( "determinate", "a " );
  add_plural( "garlic" );
  set_medium_alias( "WildGarlic"  );
  set_amount( 1  );
  add_eat_effect("/std/effects/ingested/garlic_breath", 200  );
  set_weight( 3  );
  set_weight_per_bite( 1  );
  set_piece_description( "This is a papery, white clove of garlic. "+  " It smells very pungent.\n"  );
  set_piece_short( "clove"  );
  set_piece_plural( "cloves"  );
  set_decay_speed( 1800  );
  add_alias(({"garlic", "wild garlic"}) );
  add_property( "herb lore", "Garlic, fresh or carefully dried, consists of "      "the main bulb with several attached cloves of A. sativum L., or "      "Auriental Chive.  Application of garlic is beneficial to almost "      "any ailment, especially social ones.  It should also be noted "      "that garlic causes the blood to heat and the senses to become "      "excited, and for that reason monks and priests are prohibited from "      "using it.  Excessive use of garlic is harmful to the eyes and "      "causes dizziness.");
} /* setup() */
