
#include <dirs.h>
#define THRESHOLDS ({ 10, 30, 200, 300 })
inherit COMMAND_BASE;

/*
 * No easy way to get this information from the race object as yet.
 * Everyone is human, though, so it doesn't yet matter.
 */
string illumination( int number ) {
   if( number < THRESHOLDS[ 0 ] )
       return "in pitch darkness";
   if( number < THRESHOLDS[ 1 ] )
       return "in near darkness";
   if( number > THRESHOLDS[ 3 ] )
       return "too brightly lit to see";
   if( number > THRESHOLDS[ 2 ] )
       return "too brightly lit to see clearly";

   number = ( 100 * ( number - THRESHOLDS[ 1 ] ) ) /
            ( THRESHOLDS[ 2 ] - THRESHOLDS[ 1 ] );

   switch( number ) {
     case 0 .. 5 :
       return "very poorly lit";
     case 6 .. 10 :
       return "poorly lit";
     case 11 .. 20 :
       return "dimly lit";
     case 21 .. 30 :
       return "quite well lit";
     case 31 .. 40 :
       return "well lit";
     case 41 .. 50 :
       return "brightly lit";
     default :
       return "very brightly lit";
   }
} /* illumination() */

string intensity( int number, object thing ) {
   int flag = ( thing == TP );

   if( number < THRESHOLDS[0] )
       return "produce" + (flag?"":"s") + " a faint light";
   if( number < THRESHOLDS[ 1 ] )
       return "produce" + (flag?"":"s") + " a bit of light";
   if( number > THRESHOLDS[ 3 ] )
       return (flag?"are":"is") + " so bright you can't bear to look";
   if( number > THRESHOLDS[ 2 ] )
       return (flag?"are":"is") + " so bright it hurts to look";

   number = ( 100 * ( number - THRESHOLDS[ 1 ] ) ) /
            ( THRESHOLDS[ 2 ] - THRESHOLDS[ 1 ] );

   switch( number ) {
   case 0 .. 24 :
     return "produce" + (flag?"":"s") + " quite a bit of light";
   case 25 .. 49 :
     return (flag?"are":"is") + " very bright";
   default :
     return (flag?"are":"is") + " extremely bright";
   }
} /* intensity() */

int check_exists( string file ) {
  if( objectp( find_object( file ) ) )
      return 1;
  return file_size(file+".c") > 0;
} /* check_exists() */

mixed cmd(object *things) {
   int its_light;
   string results;
   object thing;

   results = "";

   results += ( ENV(TP)->query_light() < THRESHOLDS[0] ?
                "Wherever you are, it's " :
                "$C$"+ENV(TP)->the_short(1)+" is " );

   results += illumination( ENV(TP)->query_light() )+".\n";

   if( !things ) {
       write(results);
       return 1;
   }

   foreach( thing in things ) {
      results += "$C$"+thing->the_short(1)+" ";
      its_light = (int)thing->query_light();
      if( its_light )
          results += intensity( its_light, thing ) + ".\n";
      else if( thing == TP )
          results += "produce no light at all.\n";
      else
          results += "produces no light at all.\n";
   }
   write(results);
   return 1;
} /* cmd() */

int cmd_exit(string arg) {
   object room;
   string other;

   room = ENV(TP);

   if( room->query_mirror_room() )
       room = room->query_mirror_room();

   if( LENGTHEN[arg] )
       arg = LENGTHEN[arg];

   if( !room->query_exit(arg) )
       arg = (string)TP->find_abs(arg);

   if( room->query_exit( arg ) ) {
       other = room->query_destination(arg);
       if( check_exists( other ) ) {
           object door;
           door = room->query_door_control(arg);
           if( room->query_door_open(arg) || door->query_transparent() ) {
               write( other->the_short(1)+" is "+
                      illumination(other->query_light())+".\n");
               return 1;
           } else {
               add_failed_mess("The exit " + arg + " is not open.\n");
               return 0;
           }
       } else {
           add_failed_mess("The exit " + arg + " does not exist.\n");
           return 0;
       }
   }
   add_failed_mess("Unable to find the exit " + arg + ".\n");
   return 0;
} /* cmd_exit() */


mixed *query_patterns() {
   return ({ "<indirect:object>", (: cmd($1) :),
             "exit <string'exit name'>", (: cmd_exit($4[0]) :),
             "", (: cmd(0) :) });
}
