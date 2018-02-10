// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "seed" );
  set_short( "melon seeds" );
  set_long( "This is $pile_size$ of melon seeds, $amount_size$.\n" );
  add_adjective( ({ "some", "melon" }) );
  add_adjective( ({ "seed", "seeds", "handful", "handfuls", "of" }) );
  add_property( "determinate", "some " );
  add_plural("seeds" );
  set_main_plural("melon seeds" );
  set_medium_alias( "MelonSeed"  );
  set_amount_types( ([ "seed": ({ 1, "seeds" }),
                            "handful": ({ 50, "handfuls" }) ])  );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 25, "small", 50, "medium", "large" }) );
} /* setup() */
