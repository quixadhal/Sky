inherit ARMOUR_OBJ;

/**
 * This implements a breakable jewelley.  The jewelley breaks into
 * two parts with an unreadable message on each, when you place the
 * two back together the message becomes clear.  This allows you
 * to make up those cute breakuable bits of jewelery that only fit
 * together at one point :)
 * @started Thu May 14 23:37:14 CST 1998
 * @author Pinkfish
 */

#include <language.h>
#include <move_failures.h>

string id_num;
int broken;
mixed *save_read_mess;

void set_broken(int flag);
int do_break_necklace();
int do_fit_together(object *obs);
string query_broken_id();
int query_broken();

void create() {
   object *obs;

   save_read_mess = ({ });
   do_setup++;
   ::create();
   do_setup--;
   if ( !do_setup ) {
      this_object()->setup();
   }

   /* Create a unique id for this thing. */
   if (this_player()) {
     id_num = (string)this_player()->query_name() + time() + "-" + random(12);
   } else {
     obs = users();
     id_num = (string)obs[random(sizeof(obs))]->query_name() + time() + "-" +
                   random(12);
   }
} /* create() */

/** @ignore yes */
void init() {
   add_command("break", "<direct:object> in two", (: do_break_necklace :) );
   add_command("fit", "<direct:object> together with <indirect:object>",
                 (: do_fit_together :) );
} /* init() */

/** @ignore yes */
string long(string word, int dark) {
   if (query_broken()) {
      return ::long(word, dark) + "It looks like half a piece of jewellery, "
                                  "you could \"fit\" it back together with "
                                  "the other half.\n";
   } else {
      return ::long(word, dark) + "It looks like you could \"break\" the "
                                  "jewellery into two matching halves.\n";
   }
} /* long() */ 

/**
 * This method breaks the object into two.  It creates an identical
 * second copy with the message set on, but set as unreadable.
 * @see query_broken()
 * @see set_broken()
 */
int do_break_necklace() {
   object new_copy;
   object ob;

   if (!broken) {
      /* First duplicate ourselves... */
      new_copy = clone_object(base_name(this_object()));
      new_copy->set_player(player);
      new_copy->init_static_arg(query_static_auto_load(), player);
      new_copy->init_dynamic_arg(query_dynamic_auto_load(), player);
      /* Set the weight as half. */
      new_copy->set_weight(query_weight() / 2);
      set_weight(query_weight() / 2);
      /* Set the value as half. */
      new_copy->set_value(query_value() / 2);
      set_value(query_value() / 2);
      /* Set them both as broken. */
      new_copy->set_broken(1);
      set_broken(1);
      ob = this_object();
      while (environment(ob)) {
         if (new_copy->move(environment(ob)) == MOVE_OK) {
            break;
         }
         ob = environment(ob);
      }
      return 1;
   }
   return 0;
} /* do_break_necklace() */

/**
 * This fits the two bits of the necklace back together.
 * @param obs the objects to get the fittable bit from
 */
int do_fit_together(object *obs) {
   object ob;

   if (query_broken()) {
      foreach (ob in obs) {
         if (ob != this_object() &&
              ob->query_broken_id() == query_broken_id() &&
              ob->query_broken()) {
            /* They are the same! */
            /* Join them back together. */
            set_weight(query_weight() + ob->query_weight());
            set_value(query_value() + ob->query_value());
            set_cond((query_cond() + ob->query_cond()) / 2);
            set_broken(0);
            ob->move("/room/rubbish");
            ob->set_broken(0);
            add_succeeded_mess("$N fit$s $D and $I together.\n", ({ ob }));
            return 1;
         }
      }
   }
   add_failed_mess("You cannot fit $D and $I together.\n", obs);
   return 0;
} /* do_fit_together() */

/**
 * This method sets the broken flag on the object.  If the object is
 * not set as broken then the current set of read messages will
 * be stored away, so they can be properly mangled when seen
 * back.
 * @param flag the new broken flag
 * @see query_broken()
 * @see do_break()
 */
void set_broken(int flag) {
   if (!broken && flag) {
      save_read_mess = ::query_read_mess();
      set_read_mess( ({ }) );
   } else if (broken && !flag) {
      set_read_mess( save_read_mess + ::query_read_mess() );
      save_read_mess = ({ });
   }
   broken = flag;
} /* set_broken() */

/**
 * This method returns the read message, it is overridden here to
 * do special things to the broken messages.
 * @return the standard read message array
 */
mixed *query_read_mess() {
   mixed *ret;
   mixed *stuff;
   int j;
 
   ret = ({ });
   foreach (stuff in save_read_mess) {
      stuff = copy(stuff);
      for (j = 0; j < strlen(stuff[READ_MESS]); j++) {
         if (stuff[READ_MESS][j] != ' ') {
            stuff[READ_MESS][j] = '*';
         }
      }
      ret += ({ stuff });
   }
   /* Put the new messages last... */
   ret += ::query_read_mess();
   return ret;
} /* query_read_mess() */

/**
 * This method returns the broken flag of the object.
 * @return the broken flag of the object
 * @see set_broken()
 * @see do_break()
 */
int query_broken() {
   return broken;
} /* query_broken() */

/**
 * This method returns the id used for the broken object.
 * @return the unique id of the object
 */
string query_broken_id() {
   return id_num;
} /* query_broken_id() */
 
/** @ignore yes */
mapping query_static_auto_load() {
   if ( base_name( this_object() ) != __FILE__[0..<3]) {
      return ([ ]);
   }
   return ([
      "::" : ::int_query_static_auto_load(),
   ]);
} /* query_static_auto_load() */

/** @ignore yes */
void init_static_arg(mapping map) {
   ::init_static_arg(map["::"]);
} /* init_static_arg() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
   return ([ "::" : ::query_dynamic_auto_load(),
             "broken" : broken,
             "saved" : save_read_mess,
             "id_num" : id_num ]);
} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_dynamic_arg(mapping arg) {
   ::init_dynamic_arg(arg["::"]);
   broken = arg["broken"];
   save_read_mess = arg["saved"];
   id_num = arg["id_num"];
} /* init_dynamic_arg() */

/** @ignore yes() */
mixed *stats() {
   return ::stats() + ({
           ({ "broken", broken }),
           ({ "id", id_num })
          });
} /* stats() */
