// Autogenned file

inherit "/obj/mineral.c";

void setup(){

// #class
  set_name( "coal" );
  set_short( "coal" );
  set_long( "This is $pile_size$ of coal, $amount_size$." );
  add_adjective( "some" );
  set_material( "coal" );
  set_weight_unit(({ 1, 1 })  );
  set_medium_alias( "Coal"  );
  set_amount_types( ([ "piece" : ({ 1, "pieces" }),                          "handful" : ({ 10, "handfuls" }),                          "pile" : ({ 50, "piles" }),                          "heap" : ({ 200, "heaps" }),                        ])  );
  add_adjective( ({"piece", "pieces", "handful", "handfuls", "pile",                  "piles", "heap", "heap", "of"}) );
  set_pile_types( ({50, "small", 100, "medium", "large"})  );
  set_amount( 1  );
  set_continuous( );
} /* setup() */
