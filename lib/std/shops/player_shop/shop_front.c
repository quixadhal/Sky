/**
 * The standard inheritable object for player-run shop fronts.
 * This object defines the section of the shop visible to the shop's
 * customers, and is also responsible for taking applications.  The
 * majority of player-shop functionality is handled from within the
 * main office.  See the header file for a complete description of
 * the shop's workings.
 * @see /include/player_shop.h
 * @see /std/shops/player_shop/office.c
 * @see /std/shops/player_shop/mgr_office.c
 * @see /std/shops/player_shop/counter.c
 * @see /std/shops/player_shop/storeroom.c
 * @see /std/shops/player_shop/shopkeeper.c
 * @author Ringo
 * @started 1st August 1999
 */
#include <player_shop.h>
#include <mail.h>
#define SAY_CMD "/cmds/living/sa_y.c"

inherit ROOM_OBJ;

int do_list(string stuff);

private nosave class applying_player {
   int step;
   string *answers;
}

private nosave string _inside = "",
               _outside = "",
               _office = "",
               _counter = "",
               _storeroom = "",
               _mgr_office = "",
               _channel = "";

/** @ignore yes */
void create()
{
   do_setup++;
   ::create();
   do_setup--;
   add_property( "no burial", 1 );
   add_property( "los", "closed" );
   add_property( "determinate", "" );
   add_item( "counter", "The counter divides this room of the shop in two.  "
     "You get the feeling that only employees can pass it.  There's a "
     "%^BOLD%^card%^RESET%^ on the counter that catches your attention, "
     "next to which is a small %^BOLD%^bell%^RESET%^." );
   add_item( "card", "The card seems to be a simple form.  One section "
     "seems to allow you to \"apply\" for a job here, whilst the "
     "bottom section could be filled out to make a \"suggestion\".\n"
     "If you wish to complain about an employee, you may do so here.  "
     "Please type \"complain\" for further information.\n"
     "If you have been accepted for employment, you can also \"confirm\" "
     "you accept, or you can \"cancel\" your application at any time." );
   add_item( "doorbell", "The bell above the door is positioned so that as the "
     "door passes underneath it, it will ring." );
   add_item( ({ "bell", "push-bell" }),
     ({ "long", "This is a small push-bell for summoning the employees.",
     "push", ({ this_object(), "do_push", "<direct:object>" }) }) );
   if ( !do_setup )
   {
      this_object()->setup();
      this_object()->reset();
   }
   add_extra_look( this_object() );
}
/* create() */


/** @ignore yes */
void init()
{
   string tp;

   ::init();
   if ( !_office || _office == "" )
   {
      return;
   }
   tp = this_player()->query_name();
   _office->summon_shopkeeper( 0 );
   add_command( "complain", "" );
   add_command( "suggestion", "" );
   if ( !_office->query_employee( tp ) && !_office->query_retired( tp ) )
   {
      add_command( "apply", "" );
   }
   if ( _office->query_applicant( tp ) == HIRED )
   {
      add_command( "confirm", "employment" );
   }
   if ( _office->query_applicant( tp ) )
   {
      add_command( "cancel", "application" );
   }
   add_command( "list", ({ "", "<string'items'>" }), (:do_list($4):));
}
/* init() */


/**
 * Set the path of the main office.
 * @param path The full path & filename.
 */
void set_office( string path )
{
   _office = path;
   _counter = _office->query_counter();
   _mgr_office = _office->query_mgr_office();
   _storeroom = _office->query_storeroom();
   _channel = _office->query_channel();
   set_short( _office->query_shop_name() );
}
/* set_office() */


/**
 * Add the exit to the counter.
 * Creates a hidden exit to the shop which will only allow employees to
 * pass.  This function must be called after the set_office function.
 * @param direction The direction to the counter.
 */
void set_exit_counter( string direction )
{
   _inside = direction;
   add_exit( direction, _counter, "hidden" );
   modify_exit( direction, ({ "function", "check_employee" }) );
}
/* set_exit_out() */


/**
 * Add the exit to outside the shop.
 * This exit will notify employees anywhere in the shop when
 * someone walks through it.  This function must be called after
 * the set_office function.
 * @param direction The direction to outside the shop.
 * @param path The path to outside the shop.
 */
void set_exit_out( string direction, string path )
{
   _outside = path;
   add_exit( direction, path, "door" );
   modify_exit( direction, ({ "open/close func", ({ this_object(),
     "tinkle_bell" }) }) );
}
/* set_exit_out() */


/**
 * Query the path to the room outside the shop.
 * @return The full path set by set_exit_out()
 */
string query_outside() { return copy(_outside); }


/** @ignore yes */
int do_cancel() { return _office->do_cancel(); }


/** @ignore yes */
int do_confirm() { return _office->do_confirm(); }


/**
 * Extra text for long description.
 * This function appends a list of the employees who are currently
 * clocked in.  Mask this function to amend the extra_look.
 */
string extra_look( object thing )
{
   if ( thing == this_object() )
   {
      return sprintf( "%s %s currently clocked in.\n",
        _office->employees_clocked_in(),
        ( _office->num_employees_in() == 1 )? "is" : "are" );
   }
}
/* extra_look() */


/** @ignore yes */
int check_employee( string verb, object thing, string special )
{
   if ( creatorp(thing) || _office->query_retired( thing->query_name() ) ) {
      return 1;
   }
   return copy(_office->query_employee( thing->query_name() ));
}
/* check_employee() */


/** @ignore yes */
int do_push( string command, object *indir, string dir_match,
  string indir_match, mixed *args, string pattern )
{
   object badge, *badges;

   if ( check_employee( "void", this_player(), "void" ) )
   {
       return notify_fail( "There's already an employee here: You!\n" );
   }
   if ( !_channel || _channel == "" )
   {
      return notify_fail( "You push the bell, but no-one can hear you.\n" );
   }
   if ( this_player()->query_property( "shop bell pressed" ) )
   {
       return notify_fail( "Calm down!  You've only just pushed it.  Try "
         "in a minute or so if there's still no reply.\n" );
   }
   this_player()->add_property( "shop bell pressed", 1, BELL_TIMEOUT );
   HISTORY->add_chat_history( _channel, "Shop: ", this_player()->
     query_short() + " called for assistance." );
   badges = children( BADGE );
   foreach ( badge in badges )
   {
      if ( badge->query_channel() != _channel )
      {
         badges -= ({ badge });
      }
   }
   if ( !badges )
   {
       return notify_fail( "You press the bell, but there are no "
         "employees on "+mud_name()+" at the moment.\n" );
   }
   badges->receive( "Shop", this_player()->query_short()+
     " is calling for assistance."  );
   this_player()->add_succeeded_mess( previous_object(),
     "$N $V the "+ dir_match +".\n", ({ }) );
   return 1;
}
/* do_push() */


/**
 * Application for employment.
 * This function is the start of the application process.  The
 * applicant is asked the questions defined in APP_QUESTIONS in
 * player_shop.h, and the answers are forwarded to each manager, or
 * CREATOR if there are currently no managers of the shop.
 */
int do_apply() {
   int awaiting_vacancies = 0;
   string applicant, tp;
   mapping applicants;

   tp = this_player()->query_name();

#ifdef TESTING
   if ( !playtesterp(tp) ) {
      tell_object( this_player(), "I'm sorry.  This shop is only "
        "open to applications from playtesters at this time.  "
        "Please try again at a later date.\n" );
      return 1;
   }
#endif

   if ( _office->query_applicant( tp ) )
   {
      tell_object( this_player(), "You've already applied here!\n" );
      return 1;
   }
   if ( _office->query_declined( tp ) )
   {
      tell_object( this_player(), "I'm sorry, but you cannot make another "
        "application just yet.\n" );
      return 1;
   }
   applicants = _office->get_applicants();
   foreach ( applicant in m_indices( applicants ) )
   {
      if ( applicants[applicant][APP_TYPE] == AWAITING )
      {
         awaiting_vacancies++;
      }
   }
   if ( ( awaiting_vacancies > ( _office->query_maxemp() / 10 ) ) )
   {
      tell_object( this_player(), "I'm sorry, but we do not have any "
        "vacancies at the moment.  Please try again at a later date.\n" );
      return 1;
   }
   call_out( "apply", 0 );
   add_succeeded_mess("");
   return 1;
}
/* do_apply() */


/**
 * Complaint about the shop's employees.
 * The complaint is forwarded to each manager, or CREATOR if there
 * are currently no managers of the shop.
 */
int do_complain() {
    tell_object( this_player(), COMPLAINT_TEXT +
      "  This complaint will then be sent to the managers of the shop.  "
      "If you would rather remain anonymous to the managers, please "
      "contact a liaison, or send a mail with all the above details to "+
      capitalize( _office->query_creator() )+ ".\n" );
    this_player()->do_edit( "%^CYAN%^"+ COMPLAINT_TEXT + "%^RESET%^\n\n",
      "end_complaint" );
    add_succeeded_mess("");
    return 1;
}
/* do_complain() */


/**
 * Suggestion for the shop.
 * The complaint is forwarded to each manager, or s if there
 * are currently no managers of the shop.
 */
int do_suggestion()
{
   tell_object( this_player(), SUGGEST_TEXT+ "\n" );
   this_player()->do_edit( "%^CYAN%^"+ SUGGEST_TEXT+ "%^RESET%^\n",
     "end_suggestion" );
   add_succeeded_mess("");
   return 1;
}
/* do_suggestion() */


/** @ignore yes */
void end_complaint( string text )
{
   string *managers;

   if ( !text )
   {
      tell_object( this_player(), "Aborted.\n" );
      return;
   }
   managers = _office->get_managers();
   if ( !sizeof( managers ) )
   {
      managers = ({ _office->query_creator() });
   }
   AUTO_MAILER->auto_mail( implode( managers, "," ),
     this_player()->query_name(), _office->shop_very_short()+ " complaint",
     "", text, 0, 0 );
   tell_object( this_player(), "Your complaint has now been sent to the "
     "managers, and will be dealt with as soon as possible.\n" );
}
/* end_complaint() */


/** @ignore yes */
void end_suggestion( string text )
{
   string *managers;

   if ( !text )
   {
      tell_object( this_player(), "Aborted.\n" );
      return;
   }
   managers = _office->get_managers();
   if ( !sizeof( managers ) )
   {
       managers = ({ _office->query_creator() });
   }
   AUTO_MAILER->auto_mail( implode( managers, "," ),
     this_player()->query_name(), _office->shop_very_short()+ " suggestion",
     "", text, 0, 0 );
    tell_object( this_player(), "Your suggestion has been sent to the "
      "managers, and will be dealt with as soon as possible.\n" );
}
/* end_suggestion() */


/** @ignore yes */
int tinkle_bell( string action )
{
    tell_room( this_object(), "The bell tinkles as the door "+
      action+ "s.\n" );
    tell_room( find_object( _counter ), "The bell over the shop door "
      "tinkles.\n" );
    tell_room( find_object( _office ), "You hear the bell tinkle in "
      "the main room of the shop.\n" );
    tell_room( find_object( _storeroom ), "You hear the bell tinkle in "
      "the main room of the shop.\n" );
    tell_room( find_object( _mgr_office ), "You hear the bell tinkle in "
      "the main room of the shop.\n" );
    return 1;
}
/* tinkle_bell() */


/**
 * Someone has entered the room.
 * This function will automatically move a banned person outside the shop.
 * To avoid this happening, or to modify, mask this function.
 */
void event_enter( object ob, string message, object from )
{
   if ( _office->query_baddie( ob->query_name() ) )
   {
      tell_room( this_object(), ob->query_short()+
        " drifts out of the door, seemingly against "+
        ob->query_possessive()+ " will.\n", ({ ob }) );
      tell_object( ob, "You feel yourself pushed out of the shop by "
        "a mysterious force.\n" );
      ob->move( _outside );
   }
}
/* event_enter() */


/**
 * Someone has died.
 * This function will automatically fire an employee if they have
 * killed someone whilst on duty.  It will also make a note of anyone
 * who has killed an on-duty employee (including the npc shopkeeper).  To
 * avoid this happening, or to modify, mask this function.
 */
void event_death( object killed, object *others, object killer,
  string rmess, string kmess )
{
   _office->event_death( killed, others, killer );
}
/* event_death() */


/**
 * Query the direction to another part of the shop.
 * This function is used by the npc shopkeeper to navigate
 * around the shop.
 * @param place The full path to the destination.
 * @return The direction, or "here" if already there.
 */
string directions_to( string place )
{
   if ( place == _counter || place == _office || place == _storeroom )
   {
      return copy(_inside);
   }
   return "here";
}
/* directions_to() */


/**
 * Query the path to the office.
 * @return The path to the office.
 */
string query_office() { return copy( _office ); }


/**
 * @ignore yes
 * The application form for the shop.  Viciously ripped out of the
 * Patricican's Application room for Creators.
 */
void continue_loop( class applying_player player_info )
{
   string q_str;
   q_str = "Question #"+( player_info->step+1 )+": ";
   tell_object( this_player(), "\n%^CYAN%^" + sprintf("%s%-=*s\n",
     q_str, this_player()->query_cols() - strlen( q_str ),
     APP_QUESTIONS[player_info->step] ) + "%^RESET%^");
   this_player()->do_edit( "", "finish_edit",
     this_object(), 0, player_info );
}
/* continue_loop() */


/** @ignore yes */
void apply( class applying_player player_info )
{
   player_info = new( class applying_player );
   player_info->step = 0;
   player_info->answers = ({ });
   tell_object( this_player(), "\n\n" );
   tell_object( this_player(), "%^BOLD%^Application for employment with "+
     _office->query_shop_name()+ "%^RESET%^\n\n"
     "Please note that a blank entry will abort your application.\n" );
   tell_object( this_player(), "Please answer all the questions given.  "
     "You will get an opportunity at the end to let us know anything "
     "else about you that seems relevant.\n\n" );
   continue_loop( player_info );
}
/* apply() */


/** @ignore yes */
void confirm_abort( string confirm, class applying_player player_info )
{
    confirm = lower_case( confirm );
    if ( strlen( confirm ) < 1 ||
      ( confirm[0] != 'y' && confirm[0] != 'n' ) )
    {
        tell_object( this_player(), "Please enter 'yes' or 'no'.\n"
          "Are you sure you want to ABORT the application? ");
        input_to( "confirm_abort", 0, player_info );
        return ;
    }
    if ( confirm[0..0] != "y" )
    {
        continue_loop( player_info );
        return ;
    }
    tell_object( this_player(), "Application aborted.\n" );
    return;
}
/* confirm_abort() */


/** @ignore yes */
void abort_app( class applying_player player_info )
{
    tell_object( this_player(),
      "Are you sure you want to ABORT the application? " );
    input_to( "confirm_abort", 0, player_info );
}
/* abort_app() */


/** @ignore yes */
void confirm_apply( string str, string message,
  class applying_player player_info )
{
   string from, *managers;

   str = lower_case( str );
   if ( strlen(str) < 1 || ( str[0] != 'y' && str[0] != 'n' ) )
   {
      tell_object( this_player(), "Are you sure you want to send "
        "the application (Yes or No)? " );
      input_to( "confirm_apply", 0, message, player_info );
      return;
   }
   if( str[0] == 'n' )
   {
      confirm_abort( "y", player_info );
      return;
   }
   from = (string)this_player()->query_name();
   managers = _office->get_managers();
   if ( !sizeof( managers ) )
   {
      managers = ({ _office->query_creator() });
   }
   AUTO_MAILER->auto_mail( implode( managers, "," ),
     capitalize( from ), "Application for employment with "+
     _office->shop_very_short(), "", message, 0, 0 );
   _office->add_applicant( from );
   _office->employee_log( this_player()->query_name(),
     "Applied for employment" );
   tell_object( this_player(), "\nYour application has now been sent "
     "to the managers.  You will hear from us as soon as they have "
     "made a decision.  Thank you for your application, and good luck.\n" );
}
/* confirm_apply() */


/** @ignore yes */
void end_app( class applying_player player_info )
{
   int i;
   string message;

   message = "";
   for ( i = 0; i < sizeof( APP_QUESTIONS ); i++ )
   {
      message += "%^CYAN%^" + sprintf("%2d) %-=*s\n", ( i+1 ), 70,
        APP_QUESTIONS[i] ) + "%^RESET%^" +
        sprintf("      %-=*s\n\n", 72, player_info->answers[i]);
   }
   tell_object( this_player(), "We have your application as:\n" + message+
     "\nDo you wish to send it? " );
   input_to( "confirm_apply", 0, message, player_info );
}
/* end_app() */


/** @ignore yes */
void finish_edit( string message, class applying_player player_info )
{
   if ( !message || message == "" )
   {
      abort_app( player_info );
      return;
   }
   player_info->answers += ({ message });
   player_info->step++;
   if ( player_info->step == sizeof( APP_QUESTIONS ) )
   {
      end_app( player_info );
   }
   else
   {
      continue_loop( player_info );
   }
}
/* finish_edit() */

/** @ignore yes */
object get_server() {
  object *obs;
  obs = filter(INV(TO), (: _office->employee_clocked_in($1) :));
  if(!sizeof(filter(obs, (: userp($1) :)))) {
    if(sizeof(obs)) {
      return obs[0]; //NPC shopkeeper, last choice.
    }
    else {
      return 0;  //No-one's available.
    }
  }
  obs = filter(obs, (: userp($1) :));
  return obs[random(sizeof(obs))]; //Player shopkeeper, first choice.
}

/**
 * Lists what the shop has available. Should not be called directly.
 * @param stuff The stuff to list.
 */
int do_list(string stuff) {
  object server;
  object say_cmd;
  server = get_server();
  if(!server) {
    return notify_fail("No-one's here to serve you!\n");
  }
  set_this_player(server);
  return 1;
}
