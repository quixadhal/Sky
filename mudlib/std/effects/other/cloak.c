#include <effect.h>

#define CLASS "object.cloak"

string query_classification() { return CLASS; }
int query_indefinite() { return 1; }
string query_shadow_ob() { return "/std/shadows/other/cloak.c"; }

void beginning( object player, int amount, int id ) {
    player->add_extra_look( this_object() );
    tell_object(player, "As you swirl the cloak around you, it settles on your "
      "shoulders as light as a cobweb.\n");
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
   tell_object(player, "With a strange sense of relief, you remove the cloak "
     "from your shoulders.\n");
   add_succeeded_mess ("");

} /* end() */

string extra_look( object player ) {
int *enums;

    enums = (int *)player->effects_matching( CLASS );
        if ( !sizeof( enums ) )
            return "";

    return(capitalize((string )player->query_pronoun()) +
      " appears to be shrouded in darkness.\n"); 
}
