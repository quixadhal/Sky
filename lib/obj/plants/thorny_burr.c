// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "burr" );
  set_short( "thorny burr" );
  set_long( "This is $pile_size$ of thorny burr, $amount_size$.\n" );
  add_adjective( ({ "some", "thorny" }) );
  add_property( "determinate", "some " );
  add_plural( "thorny burr" );
  set_medium_alias( "ThornyBurr"  );
  set_amount_types( ([ "stalk": ({ 1, "stalks" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "stalk", "stalks", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 })  );
  add_eat_effect("/std/effects/object/oak_wand_healing", 100  );
  add_eat_effect("/std/effects/ingested/cum_poison", 600  );
  add_property( "herb lore", "Thorny Burr, more commonly known as Beggar's "     "Buttons or as arctium lappa to the scientific community is a large "     "plant that grows up to five feet "     "tall.  It has crimson, funnel shaped flowers, and the stem is "     "woody with broad green leaves.  Its roots are used fresh or dried "     "to cure gastrointestinal ailments, aid digestion, clean the kidney "     "and clean the blood.");
} /* setup() */
