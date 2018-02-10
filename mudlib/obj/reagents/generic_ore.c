// Autogenned file

inherit "/obj/mineral.c";

void setup(){

// #class
  add_adjective( "some" );
  set_weight_unit(({ 1, 1 })  );
  set_amount_types( ([ "piece" : ({ 1, "pieces" }),                          "handful" : ({ 10, "handfuls" }),                          "pile" : ({ 50, "piles" }),                          "heap" : ({ 200, "heaps" }),                        ])  );
  add_adjective( ({"piece", "pieces", "handful", "handfuls", "pile",                  "piles", "heap", "heap", "of"}) );
  set_pile_types( ({ 50, "small", 100, "medium", "large" })  );
  set_continuous( );
} /* setup() */
