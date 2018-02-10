// Autogenned file

inherit "/obj/food.c";

void setup(){

// #class
  set_name( "comfrey" );
  set_short( "comfrey" );
  set_long( "This is $pile_size$ of comfrey, $amount_size$.\n" );
  add_adjective( ({ "some" }) );
  add_property( "determinate", "some " );
  add_plural( "comfrey" );
  set_medium_alias( "Comfrey"  );
  set_amount_types( ([ "stalk": ({ 1, "stalks" }),                          "handful": ({ 50, "handfuls" }) ])  );
  add_adjective( ({ "stalk", "stalks", "handful", "handfuls", "of" }) );
  set_amount( 1  );
  set_continuous(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_weight_unit( ({ 1, 10 })  );
  add_eat_effect("/std/effects/ingested/basic_healing", 200  );
  add_eat_effect("/std/effects/ingested/cum_poison", 800  );
  add_property( "herb lore", "Comfrey, or Knitbone, is erect in habit and "     "rough and hairy all over.  The roots are fleshy and juicy, the flowers "     "pale and creamy.  Both the leaves and roots may be used in various "     "teas to reduce swelling, gangrene, and unpredictable intestinal events.  "     "Comfrey also speeds the healing of broken bones and sprained backs.  "     "The scientific name for this herb is Symphytum officinale.");
} /* setup() */
