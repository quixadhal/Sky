#include <config.h>
#include "defs.h"
inherit ROOM_OBJ;

object button, bookcase, board;

void setup() {
    /* the short description is what you get in a room in brief mode.
     * use capital letters for proper names only and don't put a period 
     * at the end, since the short can be used to construct sentences
     */
    set_short("workroom of "+ CREATOR);
    /* the determinate is put in front of the short. 
     * if you don't specify it, "a " or "an " will be used automatically. 
     * see help setup
     */
    add_property( "determinate", "the ");
    /* you set this so you can see in the room.
     * for a description of light levels and what they mean type
     * help light
     */
    set_light(100);
    /*
     * and when you work into a room in verbose mode.
     * remember to put two spaces after periods, and uses proper
     * capitalization, punctuation and spelling.
     */
    set_long("This is the workroom of "+CAP(CREATOR)+".  You can see a small wooden "
      "desk sulking in the corner of the room, it has a beautifully "
      "carved wooden rolly chair in front of it.  It looks like this "
      "was just newly created, there is a smell of paint and "
      "leather in the air.\n");
    /* The items are the things in the room that you can look at.
     * Item longs should be proper sentences and shouldn't have \n at the end.
     * Make sure you have a room item for every noun described in the long
     * of the room or in the long of another room item. */
    add_item("chair", "The teak rolly chair has amazing carvings of dragons and "
      "... other things on it.  They almost seem alive, "
      "you especialy don't note the way the dragon "
      "blinks at you." );
    add_item("desk", "The desk is made of mahogany with a touch of "
      "mountain ash.  The top has two little basket things "
      "on it labeled \"in\" and \"out\".  The in basket "
      "is rather small." );
    add_item( "dragon", "I told you, you don't notice it winking at you." );
    add_item("in basket", "The in basket is full of small yellow forms "
      "with large red letters on them saying "
      "\"Important read me first\" and then not "
      "appearing to actually have any thing else on "
      "them." );
    add_item( "out basket", "The out basket is completely empty." );
    add_item("basket", "There are two baskets sitting on top of the desk.  "
      "They have the words \"in\" and \"out\" written "
      "on them." );
    add_item("paint", "You look around to see the source of the paint "
      "smell and you can't find it.  The walls? of the room "
      "ummm seem to be well shall we just twisting in a "
      "confused mish mash of shapes, you are sure they are "
      "not threatening." );
    add_item("wall", "The walls are made up of a cloudy substance, white "
      "in nature that is roiling around the place where "
      "you are standing.  Great magic must be holding it "
      "back, every now and then you see a demonic head "
      "push its way out of the cloud roll its eyes and fade "
      "back into the cloud." );
    add_item("cloud", "The cloud is white coloured with a few flashes "
      "of light bursting in it." );
    add_item("demon", "The demonic heads that pop out are pretty nasty "
      "looking, they disapear with a snarl of strain." );

    /* adds all the exits to the room.
     * the first parameter is the verb used to get through the exit.
     * the second is where to go once that exit is opened.
     * and the third is the type of the exit.  For a better desription of
     * this read the docs on rooms.
     * 
     * remember not to put ".c" at the end of the second parameter
     */
    add_exit("common", "/w/common", "door");
    add_exit("pub", CONFIG_START_LOCATION, "path");
    add_exit("learning", "/d/learning/main", "path");

    /* these two add_alias alias the exits.  Now when you type north (or n)
     * you will go to the starting location and south to the common room
     */
    add_alias("north", "pub");
    add_alias("south", "common");
    /* this adds an exit in the common room back to your workroom 
     */
    "/w/common"->add_exit(CREATOR, "/w/"+CREATOR+"/workroom", "door");
} /* create() */

void reset() {
    /* if the button does not exist (it has been dested or it hasn't been
     * created yet the variable is set to 0)  This makes sure you don't get
     * more than one button in your room.
     */
    if (!button) {
        button = clone_object("/obj/misc/button");
        button->move(this_object());
    } 
    if (!board) {
        board = clone_object("/obj/misc/board_mas");
        board->move(this_object() );
    }
    if (!bookcase) {
        /* The first parts of making a shelf it covered elsewhere, it's 
         * basically a matter of cloning the object and make it look like 
         * it should
         */
        bookcase = clone_object("/obj/cont_save");
        bookcase->set_name( "bookcase" );
        bookcase->set_short( "oaken bookcase" );
        bookcase->add_adjective( "wooden" );
        bookcase->add_adjective( "oak" );
        bookcase->add_adjective( "oaken" );
        bookcase->add_property( "there", "standing against the wall" );
        bookcase->set_long( "This bookcase is made from oak and "
          "varnished to bring out the glow.  It has 2 shelves, "
          "upon which you can see some books, and other objects.\n" );
        bookcase->set_weight( 2000 );
        bookcase->set_max_weight( 4000 );
        bookcase->reset_get();
        /* This call will allow the shelf to write the  savefile in the same
         * domain as the room is in, if you don't do this call, the savefile
         * will have to be somewhere in /save/ 
         */
        bookcase->check_euid(); 
        /* This call tells the shelf what file should be used to save the stuff
         * placed on it.
         */
        bookcase->set_save_file( "/w/"+CREATOR+"/creator_kit" );
        /* end by putting the shelf in the room */
        bookcase->move( this_object() );
    }
} /* reset() */


/* This function is used by the button.  It gives a list of things in
 * the room that the button should NOT destroy.
 */
object *query_non_destable()  {
    return ({ bookcase, board });
} /* query_non_destable() */
