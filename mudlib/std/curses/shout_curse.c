#include "path.h"
#define MY_NAME "Shout curse"

/*
 * The shout curse.  Makes people unable to shout...
 */
object my_player;

int init_curse(object pl) {
  string str, i;

  if( sscanf ( ( str = file_name(TO) ), "%s#%d", str, i) != 2 ) {
    /* A class, not a clone. */
    seteuid((string)master()->creator_file(str));
    return (int)clone_object(str)->init_curse(pl);
  }
  my_player = pl;
  printf("%O, %O\n", pl, TO);
  pl->add_curse(MY_NAME, str);
  if (!shadow(pl, 1)) {
    pl->remove_curse(MY_NAME);
    return 0;
  }
  return 1;
} /* init_curse() */

/*
 * No special requirements for removeing this curse...
 */
int query_remove(string name) {
  return 1;
} /* query_remove() */

/*
 * Called when the player logs on
 */
int player_start(object pl) {
  object ob;

  if (!pl)
    pl = TP;
  ob = clone_object(HERE+"shout_curse");
  ob->init_curse(pl);
} /* player_start() */

/*
 * This gets called with the name of the curse we are getting rid of.
 */
int destruct_curse(string str) {
  if (str == MY_NAME)
    destruct(TO);
  else
    my_player->destruct_curse(str);
} /* destruct_curse() */

int do_emote_all(string str) {
  write("Your voice is too hoarse to shout.\n");
  return 1;
} /* do_emote_all() */

int do_shout(string str) {
  write("Your voice is too hoarse to shout.\n");
  return 1;
} /* do_shout() */
