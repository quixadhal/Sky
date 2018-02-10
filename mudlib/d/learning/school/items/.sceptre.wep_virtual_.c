#include <virtual.h>

void dest_me() { destruct( this_object() ); }

void create()
{
  seteuid( (string)"/secure/master"->creator_file( file_name( this_object() ) ) );
} /* create() */

object create_virtual_ob() {
   object clone;

  clone = (object)SERVER->create_virtual_object( "/obj/weapon.c", 1 );
  clone->add_property( "virtual name", "/d/learning/school/items/sceptre.wep#1" );
  call_other( clone, "set_name", "sceptre"  );
  call_other( clone, "set_short", "sceptre of ultimate power"  );
  call_other( clone, "set_main_plural", "sceptres of ultimate power"  );
  call_other( clone, "add_alias", ({"sceptre of power"})  );
  call_other( clone, "add_adjective", ({""})  );
  call_other( clone, "add_plural", ({"sceptres of power"})  );
  call_other( clone, "set_long", "This is the weapon of king Richard.  It is the symbol of his infinite power over the land of the npc-school.\n"  );
  call_other( clone, "new_weapon", 10000  );
  call_other( clone, "set_weight", 6  );
  call_other( clone, "set_no_limbs", 1  );
  call_other( clone, "set_damage_chance", 5  );
  call_other( clone, "set_value", 2500  );
  call_other( clone, "add_attack",  "hammer",50,({4,10,50}),"blunt","blunt",0 );
   return clone;
} /* create_virtual_ob() */
