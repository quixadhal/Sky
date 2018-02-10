/**
 * plain.c - iroom implementation of /std/outsides/plain.
 */

inherit "/std/outsides/plain";

// My apologies for including .c files.  If anyone else has a better way,
// I'm open to suggestions (the main problem is I need to mask
// query_dest_other() in the base room).

#include "/std/room/inherit/topography/iroom.c"

void create() {

  do_setup++;
  ::create();
  do_setup--;

  if ( !do_setup ) {
    TO->setup();
    TO->reset();
  }

  add_exit(motion_verb, "interroom", "hidden");
} /* create() */
