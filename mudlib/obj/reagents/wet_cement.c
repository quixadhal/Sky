// Autogenned file

inherit "/obj/reagents/transient.c";

void setup(){

// #class
  set_name( "cement" );
  set_short( "wet cement" );
  set_long( "This is a lump of wet cement, about $amount_size$.\n" );
  add_adjective( "some" );
  add_plural( "cement" );
  set_progress_msg( ({     ({"The @@short:#obj#@@ in @@the_short:#env#@@ begins to harden.\n", 0}),     ({"The @@short:#obj#@@ in @@the_short:#env#@@ is getting really hard now.\n", 0}),     ({"The @@short:#obj#@@ in @@the_short:#env#@@ turns into a rock.\n",         ({         "set_name:#result#|rock",         "set_long:#result#|A big rocky thing.\n",         })     }) })  );
  set_medium_alias( "WetCement"  );
  set_amount_types( ([ "drop": ({ 1, "drops" }),                          "pound": ({ 1800, "pounds" }) ])  );
  add_adjective( ({ "drop", "drops",                  "ounce", "ounces",                  "quarts", "gallon", "gallons", "of" }) );
  set_final( "morph"  );
  set_duration( 25  );
  set_amount( 10000  );
  set_result( "/std/object"  );
  set_continuous(  );
  set_liquid(  );
  set_pile_types( ({ 50, "small", 100, "medium", "large" }) );
} /* setup() */
