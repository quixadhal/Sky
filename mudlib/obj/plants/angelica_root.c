// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "root" );
  set_short( "holy ghost root" );
  set_long( "This is $pile_size$ of holy ghost root, $amount_size$.\n" );
  add_adjective( ({ "some", "holy", "ghost" }) );
  add_property( "determinate", "some " );
  add_property( "no recycling", 1 );
  add_plural( "holy ghost roots" );
  set_medium_alias( "HolyGhostRoot"  );
  set_amount_types( ([ "clump": ({ 1, "clumps" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "clump", "clumps", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 })  );
  add_eat_effect("/std/effects/object/oak_wand_healing", 100  );
  add_eat_effect("/std/effects/ingested/cum_poison", 600  );
  add_property( "herb lore", "Holy Ghost Root, more commonly known as angelica "     "root comes from a plant that is a perennial herb.  The plant grows to "     "about 3 metres in height with a green groved hollow stem and large "     "pinnate leaves and umbels of greenish-white or green flowers.  Its roots "     "are usually dug out in autumn.  Fiberous roots are first removed before "     "storing away to be dried.  Its medical functions includes replenishing "     "stagnant blood, invigorating blood, alleviating pain and moistening the "     "intestines.");
} /* setup() */
