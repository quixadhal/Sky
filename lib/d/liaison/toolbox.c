/*  Liaison's Tool box - Aragorn 04/29/99 */
/*        Original code by Presto         */

#include <playerinfo.h>

/*#define LOGFILE      "WAND"*/
#define OK           0

inherit "/std/object";

string legality_string(int legal);

/*
 * This mapping lists the valid items.  The main entry for each item is in
 * the form "name" : ({ "path", 'difficulty' }).  Note the single quotes
 * around the difficulty.  An 'h' difficulty means it's a hard to get item
 * that should be replaced for anyone.  A 't' means it's a tricky item which
 * should be replaced for players under level 120.  A difficulty of 0 means
 * it's a common item that probably shouldn't be replaced.  See 'help
 * replacement' for more info.
 *
 * Quest items should have the name of the relevant quest as a third member
 * of the array after 'difficulty'.  Any special quest info for the item
 * should be specified in a fourth element.  If this info is provided, the
 * wand will automatically check if the player has done the quest.
 *
 * Aliases for the items have the form "alias" : "item name".
 */
mapping stuff = ([
"presto's wand"       :({ "/obj/misc/wand.c", 0 }),
"wand"                :"presto's wand",
"Prestos wand"        :"presto's wand",
"gagger"              :({ "/obj/misc/gagger.c", 0 }),
"transporter"         :({ "/obj/misc/transporter.c", 0 }),
"octagon"             :({ "/obj/misc/octagon.c", 0 }),
"file finder"         :({ "/obj/misc/file_finder.c", 0 }),
"scarab"              :({"/w/olorin/objects/scarab.arm", 0 }),
"arcane instrument"   :({"/w/jeremy/idle_chart.c", 0 }),
"instrument"          :"arcane instrument",
"master key"          :({"/w/jeslek/master_key.c", 0 }),
"key"                 :"master key",
"apex corrector"      :({"/w/taffyd/items/apex_corrector.c", 0 }),
"corrector"           :"apex corrector",
"resurrector"         :({"/w/obilix/temp/resurrector.c", 0 }),
"wossname finder"     :({"/w/dragonkin/items/misc/finder.c", 0 }),
"label remover"       :({"/w/taffyd/items/lremove2.c", 0 }),
"remover"             :"label remover",
])/*, money*/;

/* "money" is the mapping that will contain the valid money types */

object last_player = 0;

void setup() {

   set_read_mess( "Written in tiny little letters you can see:\n%^YELLOW%^"
                  "Aragorn's Liaison Toolbox\n%^RESET%^To use, "
                  "type \"conjure <item> for <creator>\".  The current list "
                  "of valid tools is:\n\n" +
                  sprintf( "%-#*s", (int)this_player()->query_cols(),
                           implode( keys(stuff), "\n" ) ) +
                  "\n");
   set_name( "toolbox" );
   add_adjective( ({ "shiny", "red", "liaison", "Liaison"}) );
   add_alias("box");
   set_long( "This shiny red toolbox is where liaisons store their tools.\n" );
   set_short( "Liaison toolbox" );
   add_property( "determinate", "" );
   add_property( "no recycling", 1 );
   set_enchant( 50 );
   return;
} /* setup() */


void init()  {
   add_command("conjure", "<string'item'> for <string'creator'>");
   return;
} /* init() */


void yesno( string answer, string what, object whobject, int legal )  {
   answer = lower_case( answer );
   if ( answer == "y"  ||  answer == "yes" )  {
      write( "The toolbox whispers:  Right.  I'll give it another try.\n" );
      call_out( "replace_stuff", 0, what, whobject, legal );
   }
   else write( "The toolbox whispers:  Oh, ok.  Never mind then. :)\n" );
   return;
} /* yesno() */

void self_destruct( int time_limit )  {
   object carrier, room;

   carrier = room = environment( this_object() );
   while ( environment( room ) )   room = environment( room );
   if ( !time_limit )  {
      if ( carrier == this_player() )  {
         write( "The toolbox whispers:  Have a nice day.\n" );
         this_player()->adjust_hp( -50 - random( 51 ) );
      }
      if ( carrier == room  ||  living( carrier ) )  {
         if ( room == environment( this_player() ) )
            write( "The toolbox disintegrates with a loud *POP !*\n" );
         say( "The toolbox disintegrates with a loud *POP !*\n" );
      }
      move( "/room/rubbish" );
      return;
   }
   else if ( carrier == room  ||  living( carrier ) )  {
      if ( room == environment( this_player() ) )
         write( "The toolbox says: " + query_num( time_limit ) + "...\n" );
      say( "The toolbox says: " + query_num( time_limit ) + "...\n" );
   }
   --time_limit;
   call_out( "self_destruct", 1, time_limit );
   return;
}/* self_destruct() */


int replace_stuff( string what, object whobject, int legal )  {
   object thingy;

   if ( stuff[ what ] )  {
      if ( file_size( stuff[ what ][ 0 ] ) < 1 )  {
         write( "The toolbox whispers:  Hmm.  " + stuff[ what ][ 0 ] +
                " doesn't seem to exist.\n");
         return notify_fail( "" );
      }
      last_player = whobject;
      if ( catch( thingy = clone_object( stuff[ what ][ 0 ] ) ) )  {
         write( "The toolbox whispers:  Uh oh.  There's something wrong with " +
                stuff[ what ][ 0 ] + ".\n" );
         return notify_fail( "" );
      }
      if ( thingy->move( whobject ) )  {
         thingy->dest_me();
         write( "The toolbox whispers:  Forget it.  " +
                capitalize( (string)whobject->query_pronoun() ) + "'s "
                "carrying too much junk already.\n" );
         return notify_fail( "" );
      }
      if ( whobject != this_player() )  {
         tell_object( whobject, (string)this_player()->query_cap_name() +
                      " thumps " + (string)this_player()->query_possessive() +
                      " toolbox and " + add_a( what ) + " appears in your "
                      "hands!\n" );
         write( "You conjure up " + add_a( what ) + " for " +
                (string)whobject->query_cap_name() + ".\n" );
      }
      else write( "You conjure up " + add_a( what ) + ".\n" );
   /*   log_file( LOGFILE, (string)this_player()->query_cap_name() +
                ": " + what + " for " + (string)whobject->query_cap_name() +
                " on "+ ctime( time() ) );*/
      PLAYERINFO_HANDLER->add_entry(
            this_player(),
            whobject->query_name(),
            "replace",
            "Replaced "+what);
      return 1;
   }

}  /* replace_stuff() */


int do_conjure(object *, string, string, mixed *args)  {
   string what, who, *spods_names;
   int    i, *name_match;
   object whobject, *spods;

   if ( find_call_out( "self_destruct" ) != -1 )
      return notify_fail( "The toolbox whispers:  A self-destruct is in "
                          "progress.  This function has been disabled.  "
                          "Thank you.\n" );
   if ( !( this_player()->query_creator() ) )  {
     /* log_file( LOGFILE, "Self-destruct caused by " +
                (string)this_player()->query_cap_name() + " on " +
                ctime( time() ) + "\n" );*/
      PLAYERINFO_HANDLER->add_entry(
            this_player(),
            this_player()->query_name(),
            "replace",
            "Unautorized usage of Aragorn's toolbox; self-destructing.");
      call_out( "self_destruct", 0, 10 );
      say( "The toolbox says:  Alert!  Alert!  Unauthorized user!  This toolbox "
           "will self-destruct in ten seconds.\n" );
      return notify_fail( "The toolbox says:  Alert!  Alert!  Unauthorized "
                          "user!  This toolbox will self-destruct in ten "
                          "seconds.\n" );
   }

   what = (string)this_player()->expand_nickname( args[0] );
   if ( stringp( stuff[ what ] ) )  what = stuff[ what ];
   who = lower_case( (string)this_player()->expand_nickname( args[1] ) );
   if ( !( whobject = find_player( who ) ) )  {
      spods_names = allocate( sizeof( spods = users() ) );
      for( i = sizeof( spods ) - 1; i >= 0; i-- )
         spods_names[ i ] = (string)spods[ i ]->query_name();
      if ( name_match[ 1 ] > 60 )  {
         input_to( "yesno", 0, what,
                   find_player( spods_names[ name_match[ 0 ] ] ), OK );
         return notify_fail( "The toolbox whispers:  Um, there's nobody logged "
                             "named \"" + who + "\".  Do you mean " +
                             (string)spods[ name_match[ 0 ] ]->
                                     query_cap_name() + "?  " );
      }
      else return notify_fail( "The toolbox whispers:  Who the heck is " + who +
                               "?\n" );
   }
   return replace_stuff( what, whobject, OK );
}  /* replace() */
/* EOF */

