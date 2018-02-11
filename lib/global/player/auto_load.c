/**
 * Handles the automatic loading and saving of objects into the
 * players inventories on log in/out.
 * @author Ember
 */
#include <move_failures.h>

#define CLONER "/global/player/cloner"

#define RECEIPT "/obj/misc/al_receipt"

#define AUTO_STR_LENGTH 3

#define AUTO_LOAD_TYPE 0
#define AUTO_LOAD_NAME 1
#define AUTO_LOAD_DATA 2

/* Stuff for the data elements themselves */
#define AUTO_LOAD_STATIC_ARG  0
#define AUTO_LOAD_DYNAMIC_ARG 1

// This makes the replacement of virtual objects to work.
#define REPLACE_VIRTUAL

private mixed auto_load;
private nosave int _no_calls;
private nosave function _finished;

private void auto_clone_alt( object thing, mixed *load_info,
                             object dest, object tell_pl, function move_f );
private object *int_auto_load( int type, string name, mixed *load_info,
                               object dest, object tell_pl, int now,
                               function move_f );
private void move_to_destination( object thing, object dest, object tell_pl,
                                  function move_f );

/** @ignore yes */
mixed fragile_auto_str_ob( object ob ) {
   /* this function was added by Ember 30-Oct-93 */
   /* it is a copy of auto_str_ob below, with all the catches taken out */
   /* it is intended for the newer style of saving/loading with receipts */
   mixed static_arg;
   mixed dynamic_arg;
   string fname;

   static_arg = ob->query_static_auto_load();
   dynamic_arg = ob->query_dynamic_auto_load();

   if( !static_arg && !dynamic_arg )
       return ({ });

   if( sscanf( file_name( ob ), "%s#%*d", fname) == 2 )
       return  ({ 1, fname, ({ static_arg, dynamic_arg }) });
   else
       return ({ 0, file_name(ob), ({ static_arg, dynamic_arg }) });

} /* fragile_auto_str_ob() */

/**
 * This method figures out the auto load string for the given object.
 * The returned array contains 3 elements.  The first element is a
 * flag which tells if the object is a clone or the original object.
 * The second element is the name of the file and the third is the array
 * containing the saved information.  The third array contains three
 * elements.  The first is the static auto load information, the
 * second is the dynamic autoload information and the third is the shadow
 * auto load information.  The shadow autoload information is reduant these days
 * effects should be used instead.
 * @param ob the object to get the auto load information for
 * @return the array containing the auto load information
 * @see help::effects
 */
private string *auto_str_ob(object ob) {
   string static_arg;
   string dynamic_arg;
   string catch_static;
   string catch_dynamic;
   string fname;
   int value;
   string ob_name;
   mixed* tmp;

   catch_static = catch( static_arg = (string)ob->query_static_auto_load() );
   catch_dynamic = catch( dynamic_arg = (string)ob->query_dynamic_auto_load() );
   if( !static_arg && !dynamic_arg ) {
       // create receipts?
       if( catch_static || catch_dynamic ) {
           catch( value = ob->query_value() );
           ob_name = "unknown object";
           catch( ob_name = ob->short(1) );
           catch( RECEIPT->set_object(file_name(ob)) );
           RECEIPT->set_obname(ob_name);
           RECEIPT->set_value(value);
           tmp = fragile_auto_str_ob( find_object("/obj/misc/al_receipt") );
           tmp[0] = 1;
           // This is a bit of a hack.  It could easily break if the receipt
           // inheritance changes.
           if( !undefinedp(tmp[2][1]["::"]["cloned by"]) ) {
               tmp[2][1]["::"]["cloned by"] = "greco";
           }
       } else {
           tmp = ({ });
       }
       return tmp;
   }

   if( sscanf( file_name( ob ), "%s#%*d", fname ) == 2 ) {
       return  ({ 1, fname, ({ static_arg, dynamic_arg }) });
   } else {
       return ({ 0, file_name( ob ), ({ static_arg, dynamic_arg }) });
   }

} /* auto_str_ob() */

/**
 * Creates the complete auto load array from the array of objects
 * passed into this object.  It returns an array of elements
 * as specified in auto_str_ob.  The arrays from auto_str_ob() are
 * added together, so every 3 elemnts in the array is a new
 * autoload object.
 * @param obs the objects to create an autoload string for
 * @param into_array place the results directly iunto the auto_load array
 * @return the auto load array
 * @see auto_str_ob()
 */
string *create_auto_load( object *obs, int into_array ) {
  int i;
  string *tmp;
  string *al_tmp;

  if( into_array ) {
      auto_load = ({ });
  } else {
      tmp = ({ });
  }
  /*
   * For some reason inventories are regenerated backwards,
   * if we go from the the bottom of this array, so go from the top.
   */
  for( i = sizeof(obs) - 1; i >= 0; i-- ) {
      reset_eval_cost();
      if( !objectp(obs[i]) ) {
          continue;
      }
      if( !catch( al_tmp = auto_str_ob(obs[i]) ) ) {
          if( into_array ) {
              auto_load += al_tmp;
          } else {
              tmp += al_tmp;
          }
      }
  }

  return tmp;

} /* create_auto_load() */

/**
 * The new method of handling auto loading of objects.  This determines
 * if the object failed to load and gives the player a recipt if it does
 * not.
 * <p>
 * The finished variable should <b>only</b> be set in outer
 * level calls to this.  If you set it inside containers when they call
 * this it will cause errors.  This will only be called from within
 * the player object itself.
 * @param auto_string the values specifing the objects to auto load
 * @param dest the destination to put the objects in
 * @param tell who to tell about errors
 * @param finished the function to call when the auto loading has completed
 * @see create_auto_load()
 * @see load_auto_load()
 */
void load_auto_load_alt( mixed *auto_string, object dest,
                         object tell_pl, function finished) {
   int i;

   reset_eval_cost();

   if( !auto_string || !sizeof( auto_string ) || !pointerp( auto_string ) )
       return;

   /*
    * This is to try to handle the slight over-weight problems that people
    * close to capacity may have due to mergers, e.g. money.
    */
   if( dest == TO && tell_pl == TO && !TO->query_no_check() ) {
       TO->set_no_check( 1 );
       TO->set_max_weight( 100 + TO->query_max_weight() );
   }

   _finished = finished;
   reset_eval_cost();

   for( i = 0; i < sizeof( auto_string ); i += AUTO_STR_LENGTH ) {
       _no_calls++;
       call_out( (: int_auto_load :), i / 3,
           auto_string[ i + AUTO_LOAD_TYPE ],
           auto_string[ i + AUTO_LOAD_NAME ],
           auto_string[ i + AUTO_LOAD_DATA ],
           dest, tell_pl, 0, (: $1->move($2) :) );
   }

} /* load_auto_load_alt() */

/**
 * This method creates all the objects and sets them up now.  This will
 * not move any of the objects anywhere...  Relying on the calling
 * code to handle this.
 * @param auto_string the values specifing the objects to auto load
 * @param dest the destination to put the objects in
 * @param tell who to tell about errors
 * @param finished the function to call when the auto loading has completed
 * @see create_auto_load()
 * @see load_auto_load()
 */
object *load_auto_load_to_array( mixed *auto_string, object tell_pl ) {
   object *obs;
   int i;

   obs = ({ });

   if( !tell_pl )
       tell_pl = TP || TO;

   for( i = 0; i < sizeof( auto_string ); i += AUTO_STR_LENGTH ) {
       _no_calls++;
       /* Set a null function so it always appears to succeed. */
       obs += int_auto_load( auto_string[ i + AUTO_LOAD_TYPE ],
                             auto_string[ i + AUTO_LOAD_NAME ],
                             auto_string[ i + AUTO_LOAD_DATA ],
                             0, tell_pl, 1, (: MOVE_OK :) );
   }

   return obs - ({ 0 });

} /* load_auto_load_to_array() */

/**
 * This method loads all the objects from the array and places them
 * into the inventory of the specified object.
 * @param auto_string the values specifing the objects to auto load
 * @param dest the destination to put the objects in
 * @param tell who to tell about errors
 * @param finished the function to call when the auto loading has completed
 * @see create_auto_load()
 * @see load_auto_load()
 */
void load_auto_load_to_inventory( mixed *auto_string, object dest,
                                  object tell_pl, function move_f ) {
   int i;

   if( !tell_pl )
       tell_pl = TP || TO;

   for( i = 0; i < sizeof( auto_string ); i += AUTO_STR_LENGTH) {
      _no_calls++;
      int_auto_load(auto_string[ i + AUTO_LOAD_TYPE ],
                    auto_string[ i + AUTO_LOAD_NAME ],
                    auto_string[ i + AUTO_LOAD_DATA ],
                    dest, tell_pl, 0, move_f);
   }
} /* load_auto_load_to_inventory() */

private object create_auto_load_object( string name, mixed *load_info,
                                        object dest, object tell_pl,
                                        function move_f ) {
   object thing;

   catch( name = (string)CLONER->other_file( name ) );

   if( !catch( thing = (object)CLONER->clone( name ) ) ) {
       if( thing ) {
           if( base_name( thing ) == name ) {
               auto_clone_alt( thing, load_info, dest, tell_pl, move_f );
           } else {
               move_to_destination( thing, dest, tell_pl, move_f );
           }
       } else {
           tell_object( tell_pl, "Could not clone "+ name +".\n" );
           thing = clone_object( RECEIPT );
           thing->set_object( name );
           thing->set_static_save( load_info );
           move_to_destination(thing, dest, tell_pl, move_f);
       }
   } else {
       tell_object( tell_pl, "Error in loading "+ name +".\n" );
       thing = clone_object( RECEIPT );
       thing->set_object( name );
       thing->set_static_save( load_info );
       move_to_destination(thing, dest, tell_pl, move_f);
   }

   return thing;

} /* create_auto_load_object() */

private object *int_auto_load(int type, string name, mixed *load_info,
                              object dest, object tell_pl, int now,
                              function move_f) {
   object thing;
   object new_thing;
   mixed *stuff;

   _no_calls--;
   if( type ) {
       // Do this first so we don't leak receipts.
       thing = create_auto_load_object( name, load_info, dest, tell_pl, move_f );
       if( name == RECEIPT ) {
           /* Try and reload the receipt */
           stuff = thing->query_static_save();
           if( stuff ) {
               new_thing = create_auto_load_object( name, load_info, dest,
                                                    tell_pl, move_f );
              if( new_thing ) {
                  thing->dest_me();
                  thing = new_thing;
              }
           }
       }
   } else {
       if( !find_object( name ) ) {
           if( !catch( load_object( name ) ) ) {
               thing = find_object( name );
               if( thing ) {
                   auto_clone_alt( thing, load_info, dest, tell_pl, move_f );
               } else {
                   tell_object( tell_pl, "Could not load "+ name +".\n" );
               }
           } else {
              tell_object( tell_pl, "Error in loading "+ name +".\n" );
           }
       } else {
           tell_object( tell_pl, "Object "+ name +" exists.\n" );
       }
   }

   // Check to see if we have finished.
   if( _no_calls == 0 && _finished ) {
       // NB: This has to be run at the end of the setup...
       // Because a container may attempt to reload some stuff.
       evaluate( _finished, TO );
       _finished = 0;
   }

   if( thing )
       return ({ thing });

   return ({ });

} /* int_auto_load() */

/** @ignore yes */
private string ident( object thing ) {
   mixed word;

   catch( word = (string)thing->query_short() );

   if( stringp( word ) )
       return "\""+ word +"\"";

   return file_name( thing );

} /* ident() */

/**
 * This moves the cloned object into the correct location and
 * loads up the arguments.  The arguments array contains three elements
 * the first is the object being cloned, the second is the auto load
 * string and the third is the destination to move the object too.  This is
 * used by the alternate system of loading objects.
 * @param arg the arg array
 * @see load_auto_load_alt()
 * @see create_auto_load()
 * @see auto_clone()
 */
private void auto_clone_alt( object thing, mixed *load_info,
                             object dest, object tell_pl,
                             function move_f ) {
   /*
    * The setting up the object was previously done after this...
    * Not sure why?  Moved it to here...  Oh!  I have an idea.  The
    * object which fail might not end up being moved into the right place
    * or somewhere the player can see at all.  Changed the movement stuff
    * to also try and move into the telling players environment if we
    * run out of options.
    *
    * This is a moot problem now however, since with the inventory
    * generation one the contents is generated slowly after a while
    * anyway...
    */
   reset_eval_cost();
   if( load_info[ AUTO_LOAD_STATIC_ARG ] ) {
       catch( thing->init_static_arg( load_info[ AUTO_LOAD_STATIC_ARG ],
                                      tell_pl ) );
   }

   catch( thing->set_player( tell_pl ) );
   catch( thing->init_dynamic_arg( load_info[ AUTO_LOAD_DYNAMIC_ARG ] ),
                                   tell_pl );

   move_to_destination( thing, dest, tell_pl, move_f );

} /* auto_clone_alt() */

private object replace_virtual( object ob, object tell_pl ) {
    string file, bit;
    string static_arg;
    string dynamic_arg;
    object thing;

    if( ( file = ob->query_property("virtual name") ) &&
        sscanf( file, "%s.%s", file, bit ) == 2 ) {
        file = (string)CLONER->other_file( file );
        if( file_exists(file+".c") ) {
            reset_eval_cost();
            static_arg = (string)ob->query_static_auto_load();
            dynamic_arg = (string)ob->query_dynamic_auto_load();
            if( !static_arg && !dynamic_arg )
                return ob;
            if( !thing = clone_object(file+".c") )
                return ob;
            if( static_arg )
                thing->init_static_arg( static_arg );
            thing->set_player( tell_pl );
            thing->init_dynamic_arg( dynamic_arg );
            thing->remove_property("virtual name");
            thing->remove_property("virtual time");
            user_event( TO, "inform", sprintf("Replacing %s (%O -> %O) for %s",
                ob->query_short() || ob->query_name(), file_name(ob),
                file_name(thing), tell_pl->query_cap_name() ), "admin" );
            return thing;
        }
        user_event( TO, "inform", sprintf("Failed to replace %s (%O -> %s) for %s",
            ob->query_short() || ob->query_name(), file_name(ob), file+".c",
            tell_pl->query_cap_name() ), "admin" );
    }

    return ob;

} /* replace_virtual() */

private void move_to_destination( object thing, object dest, object tell_pl,
                                  function move_f ) {
   object place;
   int ret_val;
   string catch_str;

#ifdef REPLACE_VIRTUAL
   if( !catch( place = replace_virtual( thing, tell_pl ) ) && thing != place ) {
       thing->dest_me();
       thing = place;
   }
#endif

   /* Move it into its correct location. */
   place = dest;
   ret_val = MOVE_OK - 1;
   while( place && ret_val != MOVE_OK ) {
      catch_str = catch( ret_val =  evaluate( move_f, thing, place ) );
      if( catch_str ) {
          tell_object( tell_pl, "Error moving object: " + catch_str + "\n");
          ret_val = MOVE_OK - 1;
      }

      if( ret_val != MOVE_OK ) {
          if( ENV( place ) ) {
              tell_object( tell_pl, "Cannot move "+ ident( thing ) +" into "+
                  ident( place ) +" (move returned " + ret_val + "); "
                  "attempting to move it into "+ident( ENV( place ) ) +".\n");
              place = ENV( place );
         /*
          * I don't think this case is needed.  I think it just
          * adds confusion to the issue.  Who thinks it just adds confusiong
          * to the issue, was this me? - pf
          */
          } else if( place != ENV(tell_pl) && ENV( tell_pl ) ) {
              tell_object( tell_pl, "Cannot move "+ ident( thing ) +" into "+
                  ident( place ) +" (move returned " + ret_val + "); "
                  "attempting to move it into environment of " +
                  tell_pl->query_cap_name()+".\n");
              place = ENV( tell_pl );
          } else  if( place != find_object("/room/broken") ) {
              tell_object( tell_pl, "Cannot move "+ ident( thing ) +" into "+
                  ident( place ) +" (move returned " + ret_val + "); moving "
                  "it to the room for broken objects --- please ask a "
                  "creator for help.\n");
              place = load_object("/room/broken");
          } else {
              // Error!  Error!
              tell_object( tell_pl, "Cannot move "+ ident( thing ) +" into "+
                  " the room for broken objects --- This is a serious "
                  "error!  Please tell a creator immediately.\n");
              place = 0;
          }
          /* Only use the strange move function for the first move. */
          move_f = (: $1->move($2) :);
      }
   }
} /* auto_clone_alt() */

/**
 * This method tells us if the player is in the inventory regeneration
 * phase still.
 * @return 1 if the inventory is being regenerated, 0 otherwise.
 */
int query_auto_loading() { return _no_calls > 0; }

/**
 * This method retrusn the auto load string which is used to
 * regenerate the players inventory.
 * @return the array of the auto load string
 * @see set_auto_load_string()
 */
protected mixed query_auto_load_string() { return auto_load; }

/**
 * This method sets the current auto load string to the specified value.
 * @see query_auto_load_string()
 */
protected void set_auto_load_string(mixed str) { auto_load = str; }
