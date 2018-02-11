#include <effect.h>

#define CURE 200

int beginning( object player, int time ) {
   tell_object( player, "You have a strange desire to eat bananas "
   "and curl up in a tree.\n" );
player->submit_ee( "orangutan_symptoms", ({ 30, 30 }), EE_CONTINUOUS);
player->submit_ee( 0, time, EE_REMOVE );
return time;
}

int merge_effect( object player, int time1, int time2 ) {
   int duration;

   duration = time2 + player->expected_tt();
   player->submit_ee( 0, duration, EE_REMOVE );
   return duration;
} /* merge_effect() */
 
void end( object player, int time ) {
    tell_object( player, "Your craving for bananas goes away.\n");
} /* end() */

void orangutan_symptoms( object player, int arg, int sid ) {
   switch ( random( 7 ) ) {
      case 0 :
         player->new_parser( "scratch" );
         break;
      case 1 :
         player->new_parser( "eek" );
         break;
      case 2 :
         player->new_parser( "slap" );
         break;
      case 3 :
         player->new_parser( "bite" );
         break;
      case 4 :
         player->new_parser( "gibber" );
         break;
      case 5 :
         player->new_parser( "gesture" );
         break;
      default :
         player->new_parser( "ook" );
   }
   arg -= CURE / 10;
   if ( arg < 0 )
      arg = 0;
   player->set_arg_of( (int)player->sid_to_enum( sid ), arg );
} /* nausea_symptoms() */

string query_classification() { return "misc.curse.banana"; }

string query_shadow_ob() { return "/std/shadows/ingested/banana_effect.c"; }
