// Autogenned file

inherit "/obj/mineral.c";

void setup(){

// #class
  set_name( "ore" );
  set_short( "raw iron ore" );
  set_long( "This is $pile_size$ of raw iron ore, $amount_size$.\n" );
  add_adjective( ({"some", "raw", "iron"}) );
  set_material( "raw iron ore" );
  add_plural( "ores" );
  set_weight_unit(({ 1, 1 })  );
  set_medium_alias( "RawIronOre"  );
  set_amount_types( ([ "piece" : ({ 1, "pieces" }),                          "handful" : ({ 10, "handfuls" }),                          "pile" : ({ 50, "piles" }),                          "heap" : ({ 200, "heaps" }),                        ])  );
  add_adjective( ({"piece", "pieces", "handful", "handfuls", "pile",                  "piles", "heap", "heap", "of"}) );
  set_pile_types( ({50, "small", 100, "medium", "large"})  );
  set_amount( 1  );
  set_continuous( );
} /* setup() */
