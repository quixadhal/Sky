// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "bane" );
  set_short( "Leopard's bane" );
  set_long( "This is $pile_size$ of Leopard's bane, $amount_size$.\n" );
  add_adjective( ({ "some", "leopard" }) );
  add_property( "determinate", "some " );
  add_property( "no recycling", 1 );
  add_plural( "clusters of Leopard's bane" );
  set_medium_alias( "Leopard'sBane"  );
  set_amount_types( ([ "cluster": ({ 1, "clusters" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "cluster", "clusters", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 })   );
  add_eat_effect("/std/effects/ingested/cum_poison", 700  );
  add_property( "herb lore", "Leopard's Bane, also known as Wolf's Bane or "   "Mountain Tobacco is a small herbaceious perennial found in woods and "   "mountain pastures.  The leaves form a flat rosette, from the centre of "   "which rises a flower stalk bearing clusters of small orange-yellow flowers ."    " The rhizome is dark brown, cylindrical, usually curved, and bears brittle "    "wiry rootlets on the under surface.  The flowers are collected entire and "   "dried.  Tincture made from the flowers are applied externally on injured "   "areas.  This remedy is widely used heal sprains and bruises and "   "occasionally, hair loss.\n");
} /* setup() */
