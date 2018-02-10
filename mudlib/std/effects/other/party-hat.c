#include <effect.h>

#define CLASS "object.party.hat"

string query_classification() { return CLASS; }
int query_indefinite() { return 1; }
string query_shadow_ob() { return "/std/shadows/other/party-hat.c"; }

void beginning( object player, int amount, int id ) {
    player->add_extra_look( this_object() );
    tell_object(player, "You suddenly feel the urge to do something silly "
      "and put a pair of underpants on your head!\n");
    add_succeeded_mess ("");

}

void restart( object player, int amount, int id ) {
  beginning(player, amount, id);
}

void merge_effect(object player, mixed oldarg, mixed newarg, int id) {
  return;
}

void end( object player, int amount, int id ) {
   player->remove_extra_look( this_object() );
   tell_object(player, "You suddenly realise how stupid you must look and "
      "tug the pair of underpants from your head, smiling sheepishly.\n");
   add_succeeded_mess ("");

} /* end() */

string extra_look( object player ) {
int *enums;

    enums = (int *)player->effects_matching( CLASS );
        if ( !sizeof( enums ) )
            return "";

    return(capitalize((string )player->query_pronoun()) +
      " looks rather ridiculous wearing a pair of large underpants on " +
      player->query_possessive() + " head.\n");
}
