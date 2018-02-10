// Autogenned file

inherit "/obj/reagents/monster_morph.c";

void setup(){

// #class
  set_name( "blob" );
  set_short( "pulsating blob" );
  set_long( "A pulsationg blob of...something.\n" );
  set_progress_msg( ({     ({"The blob of stuff in @@the_short:#env#@@ starts to take on a " +         "humanoid shape.\n", 0}),     ({"The blob forms into @@a_short:#result#@@ who tumbles out onto " +         "the floor.\n", 0}) })  );
  set_amount_types( ([ "drop": ({1, "drops" }) ])  );
  set_final( "monster_morph"  );
  set_duration( 15  );
  set_result( "/obj/monster"  );
  set_result_name( "troll"  );
  set_result_long( "This is a big, ugly, dumb-looking troll.\n"  );
  set_result_race( "troll"  );
  set_result_al( 700  );
  set_result_class( "fighter"  );
  set_result_max_hp( 500  );
  set_result_weight( 1000  );
  set_continuous( );
} /* setup() */
