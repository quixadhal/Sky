/**
 * water_outside.c - area implementation of /std/room/water_outside.
 */

inherit "/std/room/water_outside";

// My apologies for including .c files.  If anyone else has a better way,
// I'm open to suggestions (the main problem is I need to mask
// query_dest_other() in the base room).

#include "/std/room/inherit/topography/aroom.c"
