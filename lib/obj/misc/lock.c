/* A lock object so people can change the lock in
   a container. */

inherit "/std/object";

/** @ignore yes */
void create() {
  add_help_file("door_lock");
  do_setup++;
  ::create();
  do_setup--;
  if ( !do_setup ) {
    this_object()->setup();
    this_object()->reset();
  }
}

/** @ignore yes */
void init() {
  this_player()->add_command("change", this_object(),
      "[lock in] <indirect:object:here'door or furniture'> for "
                             "<indirect:object:me'lock'>");
}

int do_change(mixed *in_dir, string direct, string indirect, mixed *args,
              string) {
  object lock;
  object item;
  object other;
  string other_id;


  item = in_dir[0][0];
  lock = in_dir[1][0];

  if(function_exists("query_my_room", item) &&
     (query_property("locktype") != "door"))
    return notify_fail("This lock is not suitable for a door.\n");

  if(item->query_furniture()) {
    switch(item->query_pick_skill()) {
    case "covert.lockpick.door":
      if(query_property("locktype") != "furniture")
        return notify_fail("This lock is not suitable for furniture.\n");
    case "covert.lockpick.safes":
      if(query_property("locktype") != "safe")
        return notify_fail("This lock is not suitable for safes.\n");
    }
  }

  if(function_exists("query_my_room", item) &&
     !function_exists("test_occupier", environment(this_player())))
    return notify_fail("You cannot change the locks here.\n");
  
  if(function_exists("test_occupier", environment(this_player())) &&
     !environment(this_player())->test_occupier(this_player()->query_name()))
    return notify_fail("Sorry, only the owner of the house can change the "
                       "locks on items in the house.\n");

  if(!function_exists("pick_lock", item))
    return notify_fail("Sorry, that object can't take a lock.\n");

  if(!lock->query_property("key prop")) {
    this_player()->add_failed_mess(this_object(),
      "$I doesn't appear to have a key.\n", ({ lock }));
    return 0;
  }
  
  if(!lock->query_property("difficulty")) {
    this_player()->add_failed_mess(this_object(),
      "$I doesn't appear to have a difficulty.\n", ({ lock }));
    return 0;
  }

  item->set_key(lock->query_property("key prop"));
  item->set_difficulty(lock->query_property("difficulty"));
  
  // if it's a door we have to do the other side too.
  if(function_exists("query_my_room", item)) {
    other = item->query_dest();
    other_id = item->query_other_id();

    other->modify_exit(other_id, ({ "key", lock->query_property("key prop")}));
    other->modify_exit(other_id, ({ "difficulty",
                                      lock->query_property("difficulty")}));
    environment(this_player())->update_doors();
  }
  
  lock->set_short("busted lock");
  lock->add_adjective("busted");
  lock->set_main_plural("busted locks");
  lock->set_long("A broken door lock.\n");
  lock->remove_property("key prop");
  lock->remove_property("difficulty");
  lock->set_value(0);
  
  this_player()->add_succeeded_mess(this_object(),
                                    "$N remove$s the existing, and now "
                                    "broken, lock from $I and replace$s it "
                                    "with the new one.\n", ({ item }));
  return 1;
}

/** @ignore yes */
mixed query_static_auto_load() {
  if ( base_name(this_object()) + ".c" == __FILE__ )
    return int_query_static_auto_load();
  return ([ ]);
}

