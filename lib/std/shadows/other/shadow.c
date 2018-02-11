/*  -*- LPC -*-  */
/*
 * $Locker:  $
 * $Id: shadow.c,v 1.1 1998/01/06 04:42:56 ceres Exp $
 * $Log: shadow.c,v $
 * Revision 1.1  1998/01/06 04:42:56  ceres
 * Initial revision
 * 
*/
/*
 * This is based on the 'sneak' system. I know that the following system is
 * somewhat convoluted, but its the best system I could come up with for
 * various reasons not least among which is that this way I needed to add
 * no code to the player or room objects which keeps memory usage down
 * for the majority who are not shadowing anyone.
 * -- Ceres July 1997
 */
#include "path.h"
#include <tasks.h>

inherit "/std/effect_shadow";

#define COST 7
#define SKILL "covert.stealth"
#define HSKILL "covert.hiding.person"

#define TC(X) tell_object(find_player("ceres"), X)

int level, out_of_hiding, moving, sneak_removed, dont_unsneak;
string skill, oldskill;
object shadowee;

void set_sneak();
void set_hide();

// this is used to calculate the current sneaking level for when someone
// wants to view it.
int query_shadow_level() {
  return level;
}
object query_shadow_victim() {
  return shadowee;
}

// called when the person starts shadowing
void start_shadowing(object who, int lev) {
  level = lev;
  dont_unsneak = 1;
  shadowee = who;
  set_hide();
  dont_unsneak = 0;
}

void stop_shadowing() {
  remove_this_effect();
}

void hide_callback() {
  out_of_hiding = 1;

  if(query_verb() == "hide" || query_verb() == "sneak") {
    tell_object(player, "Attempting to remain hidden you stop sneaking.\n");
    return;
  }

  if(!moving) {
    if(player == previous_object() )
      tell_object(player, "You come out of hiding.\n");
    else
      tell_object(player, "You have been noticed.\n" );
    tell_room( environment(player), "You notice $a_short:"+
         file_name( player ) +"$ nearby.\n", player );
  }
}

void shadow_callback() {
  if(!dont_unsneak)
    sneak_removed++;
}

string calc_skill(object who) {
  string s;
  
  if(environment(who)->query_property("location"))
    s = SKILL + "."+ environment(who)->query_property("location");
  else
    s = SKILL + ".outside";
  return s;
}

void set_sneak() {
  int bonus;

  bonus = (player->query_skill_bonus(HSKILL) * level) / 3;
  player->add_hide_invis( "sneaking", 0, bonus, ({this_object(),
                                                  "shadow_callback"}));
}

void set_hide() {
  int bonus;

  bonus = (player->query_skill_bonus(HSKILL) * level) / 3;
  player->add_hide_invis( "hiding", 0, bonus, ({this_object(),
                                                "hide_callback"}));
}

// used to charge people for sneaking
// the out_of_hiding and moving variables are needed coz exit_command
// a. gets called a lot even when not moving
// b. automatically takes you out of hiding even though you haven't been
//    spotted in this case.
varargs int exit_command(string word, mixed verb, object thing) {
  int retval;

  set_sneak();

  moving = 1;
  retval = player->exit_command(word, verb, thing);
  moving = 0;
  
  if(retval) {
    // reduce their gps
    if(player->query_specific_gp("covert") < COST * level) {
      tell_object(player, "You can't concentrate sufficiently to shadow "
                  "anymore.\n");
      remove_this_effect();
      return 1;
    } else if(sneak_removed) {
      remove_this_effect();
      return 1;
    }
    tell_object(player, "You shadow "+shadowee->the_short()+" "+word+".\n");
    player->adjust_gp(-(COST * level));
    out_of_hiding = 0;
  }

  dont_unsneak = 1;
  player->remove_hide_invis("sneaking");
  dont_unsneak = 0;

  if(!out_of_hiding) {
    set_hide();
  }
  
  return retval;
}

// function called by /global/events:event_enter to perform a taskmaster
// test for the shadower.
void sneak_task(object who) {
  mapping hide_invis;
  string location;
  int perception;

  hide_invis = player->query_hide_invis();
  
  if(hide_invis["sneaking"]) {
    
    // set their perception.
    perception = who->query_skill_bonus("other.perception");

    switch(who->check_dark((int)environment(who)->query_light())) {
    case -2:
    case 2:
      perception /= 4;
      break;
    case -1:
    case 1:
      perception /= 2;
      break;
    default:
      break;
    }

    player->query_skill_bonus(calc_skill(player));
    switch(TASKER->attempt_task(perception, hide_invis["sneaking"][1], 40)) {
    case AWARD:
      if(player->add_skill_level(calc_skill(player), 1)) {

        if(environment(player)->query_property("location"))
          location = environment(who)->query_property("location");
        else
          location = "outside";
        
        tell_object(player, "%^YELLOW%^You "+
                    ({ "understand", "comprehend", "realise",
                       "appreciate" })[ random( 4 ) ] +
                    " the methods of shadowing "+ location +" more fully."
                    "%^RESET%^\n" );
      }
    }
  }
}

// function called by /global/events:event_enter to perform a taskmaster
// test for the person entering.
void perception_task(object who) {
  mapping hide_invis;
  int perception;

  hide_invis = player->query_hide_invis();

  if(player->query_visible(who)) {
    return;
  }
    
  if(hide_invis["hiding"]) {
    
    // set their perception.
    perception = who->query_skill_bonus("other.perception");

    switch(who->check_dark((int)environment(who)->query_light())) {
    case -2:
    case 2:
      perception /= 4;
      break;
    case -1:
    case 1:
      perception /= 2;
      break;
    default:
      break;
    }

    switch(TASKER->attempt_task(hide_invis["hiding"][1], perception, 50)) {
    case AWARD:
      who->add_skill_level("other.perception", 1);
      tell_object(who, "%^YELLOW%^You feel very alert.%^RESET%^\n" );
    }
  }
}

int attack_by(object ob) {
  call_out("stop_shadowing", 1);
  return player->attack_by(ob);
}

int attack_ob(object ob) {
  call_out("stop_shadowing", 1);
  return player->attack_ob(ob);
}

void do_shadow(object to) {
  string *direc;
  int i, found;
  
  direc = (environment(player))->query_dest_dir(to);

  if(sizeof(direc)) {
    for(i=0; i<sizeof(direc)-1; i+=2) 
      if(direc[i+1] == file_name(to) ||
         direc[i+1] == file_name(to)+".c" ||
         direc[i+1] == file_name(to)+".r") {
        found = 1;
        break;
      }

    if(found) {
      if(environment(player)->query_door_open(direc[i]) == 0) {
        tell_object(player, "You decide not to follow through a closed "
                    "door.\n");
        stop_shadowing();
        return;
      } else
        this_object()->exit_command(direc[i]);
      return;
    }
  }
}

void event_exit(object ob, string mess, object to) {
  player->event_exit(ob, mess, to);

  if(!shadowee) {
    tell_object(player, "Your victim seems to have disappeared.\n");
    stop_shadowing();
  }

  if(environment(shadowee) != environment(ob)) {
    tell_object(player, "You seem to have lost your target.\n");
    stop_shadowing();
  }
  
  if(ob == shadowee && shadowee->query_visible(player))
    call_out("do_shadow", 0, to);
}
