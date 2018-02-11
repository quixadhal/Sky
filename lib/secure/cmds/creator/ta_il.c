
#include <creator.h>

inherit COMMAND_BASE;

mixed cmd( string str, int num ) {
   string *filenames;
   int loop;
   object *obs;
  
   if (num < 0)  {
      num = -num;
   }
   if (num == 0)  {
      num = 20;
   }

   if ( !str ) {
      notify_fail( "Tail what file(s)/object(s)?\n" );
      return 0;
   }
   if ( !sizeof( filenames = this_player()->get_files( str ) ) ) {
      if ( sizeof( obs = WIZ_PRESENT->wiz_present( str, this_player() ) ) ) {
         filenames = map( obs, (: base_name($1) :) );
         filenames = map( filenames,
           (: ( file_size($1) <= 0 ? $1 + ".c" : $1 ) :) );
      } else
         return notify_fail( str + ": No such file(s)/object(s).\n" );
   }
   
   foreach( str in filenames )
      if( ( file_size( str ) == -2 ) || ( str[<2..<1] == ".." ) )
         filenames -= ({ str });

   for( loop = 0; loop < sizeof( filenames ); loop++ ) {
      str = filenames[loop];
      if( sizeof( filenames ) > 1 ) {
         printf( "%*'-'s\nFILE : %s\n", 80, "", str );
      }
      if ( file_size( str ) < 0)
         write( "That file/object is not readable, or does not exist.\n" );
      else {
         write( read_file( str, ( ( file_length( str ) + 1 ) - num ) ) );
      }   
   }
   return 1;
}

mixed *query_patterns()  {
   return ({ "<number> <string>", (: cmd($4[1], to_int( $4[0] ) ) :) ,
             "<string>", (: cmd($4[0], 20) :) });
}
