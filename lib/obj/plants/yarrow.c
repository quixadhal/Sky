// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "yarrow" );
  set_short( "yarrow" );
  set_long( "This is $pile_size$ of yarrow, $amount_size$.\n" );
  add_adjective( ({ "some" }) );
  add_property( "determinate", "some " );
  add_plural( "yarrow" );
  set_medium_alias( "Yarrow"  );
  set_amount_types( ([ "stalk": ({ 1, "stalks" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "stalk", "stalks", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 })  );
  add_eat_effect("/std/effects/object/oak_wand_healing", 100  );
  add_eat_effect("/std/effects/ingested/cum_poison", 600  );
  add_property( "herb lore", "Yarrow, also known as Fever Plant or "     "millefolium to some, is a tall plant with clusters of small white "     "flowers and bright green, ragged leaves.  The roots are not used, "     "but the leaves or flowers may be used fresh or dried, especially "     "to make various types of teas.  Yarrow is good to cure colds, "     "fevers, stop bleeding, and ease the stomach, but too much yarrow "     "is poisonous.");
} /* setup() */
