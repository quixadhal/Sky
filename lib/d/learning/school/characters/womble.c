// Example npc for creator school
// October '01 by Avelan

#include "defs.h"
#include <armoury.h>
#define TESTARMOUR "/d/learning/school/items/womble_cape"
inherit NPC_OBJ;
object cloak;

void setup(){
    /**
    * Choose a random choice of the words given in the following
    * array and assign it to the variable.
* We shall use these variables in the description and
* in the short desc and adjective also.
    */
    string type = choice(({ "funny","happy","smiling","red","bouncing" }));
    string type2 = choice(({ "stoopid","clueless","crazy","insane" }));

    set_name("womble");
    set_short( type+ " womble");
    add_adjective( type );
    set_long("A "+type+" but obviously "+type2+" womble with"
      " long whiskers and a grey fur.  Her eyes are"
      " enough to make even the most battle-hardened"
      " warriors think twice before attacking the poor"
      " thing.\n");

    basic_setup("dwarf", 10 + random(10), "female");

    adjust_money(100+random(100),choice(({"copper","silver","gold"})));

    /*
    * So, here are the lines which makes the npc respond to things.
    * Do 'help add_respond_to_with' for more details.
    */
    add_respond_to_with(({"@say",({ "womble" }) }),"#do_womble");
    add_respond_to_with(({"@say",({ "yes","y","sure","yep" }) }),"#got_yes");
    add_respond_to_with(({ "@womble" }),"womble $hcname$");
    add_respond_to_with(({ "@say", ({ "wibble","wobble" }) }),
      "'No, i don't do that.  I can only womble.");

    load_chat( 35, ({
        1,"@womble",
        1,":wombles around aimlessly.",
        1,"The womble stares at you with her big, funny eyes.",
        1,"'I can womble pretty good.",
        1,"�I'm the cutest womble there is.",
        1,":stares at $lname$ and does a couple of wombles."
      }) );

    ARMOURY_H->request_item(choice(({"giant club","large club",
          "mace","staff","spiked club","knobbly mace"})),40+random(30))->move(TO);

    cloak = clone_object(TESTARMOUR);
    cloak->move(TO);

    init_equip();

} /* setup() */


/*
* Ok, if a player says 'womble' in the same room, we will
* execute this piece of code.
* init_commands are executed after the given time,
* ie. The womble will giggle after two seconds when someone
* says womble.
*/
void do_womble(){
    init_command("giggle",2);
    init_command("'Heh, are you sure you wan't to see me womble?",2);

    /**
    * add a property called 'asked' to the player
    * so we won't give the effect to a player that
    * hasn't asked us to womble.
    * The property will time out in 30 seconds,
    * it's best to use timed properties almost
    * in every case to avoid having unneccessary
    * properties on players.
    */
    TP->add_property("asked",1,30);
} /* do_womble() */

/*
* So, we asked the player if they want us to womble and
* they said yes, so let's womble and stun the player.
*/
void got_yes(){

    /* Checks if the player has asked us to womble or not */
    if(TP->query_property("asked")){

        do_command("'Well, you asked for it!");
        do_command(":wombles around so furiously that you get"
          " dizzy and pass out.");
        /*
        * call_out will make us go the the defined function inside
        * this object after the given time.  The TP at the end is
        * a variable we want to pass to that function.
        * With it we know who we want to stun, ie. who said yes
        * and caused us to execute this function.
        */
        call_out("do_effect",2,TP);
    }
} /* got_yes() */

void do_effect(object player){

    /**
    * add the effect called 'out_cold' to the player
    * for 30 seconds.
    */
    player->add_effect("/std/effects/other/out_cold.c",30);
    player->remove_property("asked");
} /* do_effect() */
