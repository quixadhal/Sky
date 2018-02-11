// Example npc for creator school
// October '01 by Avelan

#include "defs.h"

/*
* Note that you will have to include the <armoury.h>
* if you want to use the armoury.
*/
#include <armoury.h>

#define TESTARMOUR "/d/learning/school/items/womble_cape"
inherit NPC_OBJ;
object cloak;

void setup(){

    set_name("womble");
    set_short( "funny womble");
    add_adjective( "funny" );
    set_long("A funny but obviously crazy womble with"
      " long whiskers and a grey fur.  Her eyes are"
      " enough to make even the most battle-hardened"
      " warriors think twice before attacking the poor"
      " thing.\n");

    /**
    * There is no such race as womble so i chose dwarf for
    * it since it can hold weapons also.
    */

    basic_setup("dwarf", 10 + random(10), "female");

    adjust_money(100+random(100),choice(({"copper","silver","gold"})));

    /*
    * Ok, the syntax is:
    * load_chat( <chance of chat>, ({ <weight of next chat>,<"chat"> }) );
    * The chance of chat is the chance in 1000 that a chat will happen
    * every two seconds.
    * The 'weight of next chat' determines which of the chats is most likely
    * to occur, you can control them with these numbers.
    * The chat is obviously an emote or a say for example.
    * The first character in the chat determines what the npc
    * does when this particular chat happens.
    * You can do the following:
    *    @        Use a soul command.
    *    :        Emote.
    *    '        Say.
    *    �        Loud Say.
    *    default  Echo.
    *    $lname$  Translates to a random player in the room.
    */

    load_chat( 35, ({
        1,"@womble",
        1,":wombles around aimlessly.",
        1,"The womble stares at you with his big, funny eyes.",
        1,"'I can womble pretty good.",
        1,"'I'm the cutest womble there is.",
        1,":stares at $lcname$ and does a couple of wombles."
      }) );

    /**
    * request an item from the armoury handler using choice
    * to make it a bit random.
    * Choice makes a random pick from the array of things
    * given to it.
    * Set the condition of 40+random(30)
    * for it and move it to the wombles inventory.
    */
    ARMOURY_H->request_item(choice(({"giant club","large club",
          "mace","staff","spiked club","knobbly mace"})),40+random(30))->move(TO);

    /*
    * Next we clone the cape we have coded and move it to the
    * wombles inventory.
    * NOTE. You should use the armoury if at all possible.
    * If you can't find suitable items from the /obj directory,
    * then you can use this method and make your own armours/weapons.
    */

    cloak = clone_object(TESTARMOUR);
    cloak->move(TO);

    /**
    * init_equip() == Make the npc wear and hold everything
    * in its inventory.
    */
    init_equip();

} /* setup() */
