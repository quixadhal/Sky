#include <top_ten_tables.h>

inherit "/std/object";

string long( string words, int number );

nosave string table_name;

void setup() {
   table_name = 0;
   set_name( "table" );
   set_short( "top ten table" );
   add_adjective( ({ "top", "ten", "score" }) );
   reset_get();
   add_property( "there", "in one corner" );
   add_property( "survive fire", 1 );
   add_read_mess((: long("",0) :), 0, "common", 0);
} /* setup() */

string query_table_name() { return table_name; }

void set_table_name( string word ) { table_name = word; }

string convert_age( int number ) {
   if ( number > 86400 )
      return ( number / 86400 ) +"D";
   if ( number > 3600 )
      return ( number / 3600 ) +"h";
   if ( number > 60 )
      return ( number / 60 ) +"m";
   return number +"s";
} /* convert_age() */

string generate_list( mixed *args ) {
   int i, number, sum;
   string format, list;

   if ( table_name )
      format = "  %2d. %-13s: level %4d  %5s";
   else
      format = "  %2d. %-23s: level %4d  %5s";
   format += "\n";
   list = "";
   number = sizeof( args );
   sum = 0;
   for ( i = 0; i < number; i++ )  {
      list += sprintf( format, i + 1, capitalize( args[ i ][ TOP_TEN_NAME ] ),
            args[ i ][ TOP_TEN_LEVEL ],
            convert_age( args[ i ][ TOP_TEN_AGE ] ) );
      if ( i > 0  &&  i < number - 1 )  {
         sum += args[i][TOP_TEN_RATING];
      }
   }
   //list += "\nA rating of " + sum / (number - 2) +
           //" would put you firmly in the middle.\n";
   return list;
} /* generate_list() */

string long( string words, int number ) {
   string long;
   mixed *args;

   long = "High Score Table: ";
   args = (mixed *)TOP_TEN_HANDLER->query_ordered_table( table_name );
   if ( !table_name ) {
      long += "All Players";
      if ( !sizeof( args ) )
         return long +": There is no overall top ten at present.\n";
   } else {
      if ( file_size( "/std/guilds/"+ table_name +".c" ) < 0 )
         return "*Barf!* Can't find /std/guilds/"+ table_name +".\n";
      long += capitalize( (string)( "/std/guilds/"+ table_name )->
                     query_name() ) + "' Guild";
      if ( !sizeof( args ) )
         return long +": There is no top ten for this guild at present.\n";
   }
   return sprintf( "%|*s\n", table_name ? 40 : 50, long ) +
          sprintf( "%|*s\n", table_name ? 40 : 50, "Sorted by rating" ) +
          sprintf( "%|*s\n\n", table_name ? 40 : 50, 
                   "-=- " + ctime(time()) + " -=-" ) +
          generate_list(args) + "\n";
} /* long() */
