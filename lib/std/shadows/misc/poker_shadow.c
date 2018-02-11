/*  -*- LPC -*-  */
/*
 * $Locker: presto $
 * $Id: poker_shadow.c,v 1.1 1998/01/06 04:39:04 ceres Exp presto $
 * $Log: poker_shadow.c,v $
 * Revision 1.1  1998/01/06 04:39:04  ceres
 * Initial revision
 * 
*/
#define FEAR "/std/spells/priest/fear.c"
#define BLIND "/std/effects/priest/blind_effect"

object this_gambler;

void setup_shadow( object gambler )  {
   shadow( gambler, 1 );  
   this_gambler = gambler;  
   return;
}  /* setup_shadow() */

void net_dead()  {
   this_gambler->net_dead();
   if ( !environment( this_gambler )->query_status()  && 
        !environment( this_gambler )->query_anted( this_gambler ) )  {
      tell_room( environment( this_gambler ), "One of the imps taps " +
                 (string)this_gambler->short() + " on the chalky forehead, "
                 "pulls out a small hourglass, and begins watching it "
                 "intently.\n" );
      call_out( "do_leave", 30 );
   }
   else if ( this_gambler ==
             (object)environment( this_gambler )->query_player() )  {
      tell_room( environment( this_gambler ), "One of the imps taps " +
                 (string)this_gambler->short() + " on the chalky forehead, "
                 "pulls out a small hourglass, and begins watching it "
                 "intently.\n" );
      call_out( "do_fold", 30 );
   }
   return;
}  /* net_dead() */

void event_inform( object ob, mixed mess, string type, object thing )  {
   string mess2;

   if (type == "link-death")  {
      sscanf( lower_case( mess ), (string)this_gambler->query_name() + " %s",
              mess2 );
      if ( ( mess2 == "reconnects"  ||  mess2 == "enters Discworld" )  &&
           ( remove_call_out( "do_fold" ) != -1  ||
             remove_call_out( "do_leave" ) != -1 ) )  {
         tell_room( environment( this_gambler ), "The imp puts away its "
                    "hourglass, nods happily at " +
                    (string)this_gambler->short() + ", and returns to the "
                    "corner.\n", this_gambler );
         tell_object( this_gambler, "An imp puts away its hourglass, nods "
                      "happily at you, and returns to the corner.\n");
      }
   }
//   else if (type == "logon")  {
//      sscanf(lower_case(mess), this_gambler->query_name() + " %s", mess2);
//      if (mess2 == "leaves Discworld")  {
//         environment( this_gambler )->event_exit( this_gambler, 0, 0 );
//      }
//   }
   this_gambler->event_inform( ob, mess, type, thing );
   return;
}  /* event_inform()*/

void do_fold()  {
   tell_room( environment( this_gambler ), "The imp gently takes the cards "
              "from " + (string)this_gambler->short() + " and returns to the "
              "corner.\n" );
   environment( this_gambler )->fold( this_gambler );
   return;
}  /* do_fold() */

void do_leave()  {
   if ( environment( this_gambler )->query_status() )
      tell_room( environment( this_gambler ), "The imp gently takes the "
                 "cards from " + (string)this_gambler->short() + ", props " +
                 (string)this_gambler->query_objective() +
                 " against the wall, and returns to the corner.\n" );
   else
      tell_room( environment( this_gambler ), "The imp props " +
                 (string)this_gambler->short() + " against the wall and "
                 "returns to the corner.\n" );  
   environment( this_gambler )->leave( this_gambler );
   return;
}  /* do_leave() */

void do_check()  {
   tell_room( environment( this_gambler ), "The imp says, \"Time's up!\"\n" );
   environment( this_gambler )->check();
   return;
}  /* do_check() */

void do_idle()  {
   tell_room( environment( this_gambler ), "The imp announces, \"Time's up!  "
              "Continue without " + (string)this_gambler->query_short() +
              ".\"\n" );
   tell_room( environment( this_gambler ), "The imp whispers something to " +
              (string)this_gambler->query_short() + ".\n" );
   tell_object( this_gambler, "The imp whispers to you, \"If you can get "
                "some money before the cards are dealt, you can still "
                "play.\"\n" );
   environment( this_gambler )->sit_out( this_gambler );
   return;
}  /* do_idle() */

void do_death()  {
   tell_object( this_gambler, "As you breathe your last, one of the imps "
                "leaps on your head screaming, \"Dead people are not allowed "
                "to play!\"\n" );
   tell_room( environment( this_gambler ), "As " +
              (string)this_gambler->short() + " keels over, one of the imps "
              "leaps on " + (string)this_gambler->query_possessive() +
              " head and screeches, \"Dead people are not allowed to "
              "play!\"\n", this_gambler );
   environment( this_gambler )->leave( this_gambler );
   this_gambler->do_death();
   return;
}  /* do_death() */

void dest_poker_shadow()  {
   call_out( "zap", 0 );
   return;
}  /* dest_poker_shadow() */

void zap()  {
   destruct( this_object() );
   return;
}  /* zap() */

void prepare_to_leave()  {
   call_out( "do_leave", 30 );
   return;
}  /* prepare_to_leave() */

void prepare_to_fold( int delay )  {
   call_out( "do_fold", delay );
   return;
}  /* prepare_to_fold() */

void prepare_to_check( int delay )  {
   call_out( "do_check", delay );
   return;
}  /* prepare_to_check() */

void prepare_to_idle( int delay )  {
   call_out("do_idle", delay);
   return;
}  /* prepare_to_idle() */

void cancel_fold()  {
   int i;

   i = 0;
   while( remove_call_out( "do_fold" )  != -1  ||
          remove_call_out( "do_check" ) != -1  ||
          remove_call_out( "do_idle" )  != -1   )  i = 1;
   if ( i )
      tell_room( environment( this_gambler ), "The imp nods, puts away its "
                 "hourglass, and returns to the corner.\n" );
   return;
}  /* cancel_fold() */

void run_away()  {
   if ( previous_object() == find_object( FEAR ) )
      tell_object( this_player( 1 ), "One of the imps screeches, \"Quit "
                   "trying to scare " + (string)this_gambler->short() +
                   "away!\"\n" );
   else this_gambler->run_away();
   return;
}  /* run_away() */

void add_effect( string eff, int duration )  {
   if ( eff == BLIND )  {
      write( "One of the imps jumps up and pokes you in the eyes screaming, "
             "\"How do YOU like it?\"\n" );
      say( "One of the imps pokes " + this_player()->query_short() + "in the "
           "eyes screaming, \"How do YOU like it?\"\n" );
   }
   else this_gambler->add_effect( eff, duration );
   return;
} /* add_effect() */

void quit()  {
   environment( this_gambler )->event_exit( this_gambler, 0, 0 );
   this_gambler->quit();
   return;
}  /* quit() */
