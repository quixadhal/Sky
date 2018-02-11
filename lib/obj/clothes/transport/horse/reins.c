// Autogenned file

#include <npc/transport.h>
inherit "/obj/clothing_transport.c";

void setup(){

// #include
  set_name("reins" );
  set_main_plural("reins" );
  set_short("leather reins" );
  add_adjective("leather" );
  add_property(TRANSPORT_TEATHER_PROP, 1 );
  set_type("necklace" );
  set_weight(5 );
  set_material("leather" );
  set_worn_by_race("horse" );
  set_direction_difficulty_modifier(100 );
  set_long("A leather strap that connects up to a bit of a horse to allow "
       "you to control the horse with greater ease.\n");
} /* setup() */
