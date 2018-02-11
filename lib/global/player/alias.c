/**
 * The alias control module for players.
 * @author Pinkfish
 */
#include <alias.h>
#include <player.h>
inherit "/global/player/history";

mapping aliases;
nosave mapping doing_alias;

protected void exec_alias(string verb, string args);
protected string *expand_alias(string verb, string args);
protected void set_doing_alias(string verb);
protected int is_doing_alias(string verb);
protected string *run_alias(string verb, string args);
int remove_alias_thing(string);
protected int alias(string);
protected int edit_alias(string);

/**
 * This method sets the alias to the new value.
 * @param name the name of the alias
 * @param value the value to set the alias too
 * @see query_player_alias()
 */
int add_player_alias( string name, mixed value ) {
   if( file_name(PO) != ALIAS_CMD &&
       file_name(PO) != EDIT_ALIAS_CMD ) {
       return 0;
   }

   aliases[name] = value;
   return 1;

} /* add_player_alias() */

/**
 * This method returns the value of the specified alias.
 * @param name the name of the alias to query
 * @return the value of the alias
 * @see add_player_alias()
 */
mixed *query_player_alias(string name) {
   return copy(aliases[name]);
} /* query_player_alias() */

/**
 * This method will remove the alias from the player.
 * @param name the name of the alias
 * @see query_player_alias()
 * @see add_player_alias()
 */
int remove_player_alias(string name) {
   if( file_name(PO) != UNALIAS_CMD && name != "" ) {
       printf("%O\n", file_name(PO) );
       return 0;
   }
   map_delete(aliases, name);
   return 1;
} /* remove_player_alias() */

/**
 * This method adds all the alias commands onto the player.  The commands
 * are 'alias', 'unalias', 'ealias', 'END_ALIAS' and the "*" pattern
 * expand the alias.
 */
void alias_commands() {
   add_command("END_ALIAS", "<string>", (:remove_alias_thing($4[0]):));
} /* alias_commands() */

/**
 * This method zaps all the current aliases defined.  This can only be
 * called by a high lord.
 * @return 0 on failure and 1 on success
 */
int remove_all_aliases() {
   if (!lordp(this_player(1))) {
      write("You can't do that :)\n");
      return 0;
   }

   aliases = ([ ]);
   return 1;
} /* remove_all_aliases() */


/**
 * This method returns a complete list of all the aliases
 * defined on the player.  This is the internal mapping so it will
 * be quite unreadable.
 * @see print_aliases()
 * @see /include/alias.h
 * @return the mapping of aliases
 */
mapping query_aliases() {
   return copy(aliases);
} /* query_aliases() */

/**
 * This method tells us if the given name is an alias.
 * @param verb the verb to check
 * @return 1 if it is an alias, 0 if not
 */
int is_alias(string verb) {
   return aliases[verb] != 0;
} /* is_alias() */

/**
 * This method runs the alias and executes all the commands in the
 * alias.  You should probably use run_alias() instead.  This calls
 * set_doing_alias() to setup blocking.
 * @param verb the name of the alias
 * @param args the arguments associated with the alias
 * @see run_alias()
 * @see set_doing_alias()
 */
protected void exec_alias(string verb, string args) {
   string *bing;
   string comm;

   bing = run_alias(verb, args);

   if( bing ) {
       set_doing_alias(verb);
       foreach( comm in bing )
          command(comm);
   }
} /* exec_alias() */

/**
 * Attempt to expand the alias.    This will look up the alias and
 * see if it is defined.  If it is, it will attempt to expand the
 * alias. This does not call set_doing_alias().  This will
 * return 0 if the alias does not exist, or the alias is already
 * being run.
 * @return the array if the alias was expanded, 0 if failed
 * @param verb the name of the alias to expand
 * @param args the arguments to the alias
 * @see exec_alias()
 * @see is_doing_alias()
 * @see set_doing_alias()
 * @see expand_alias()
 */
protected string *run_alias(string verb, string args) {
   if( !mappingp(aliases) )
       aliases = ([ ]);
   if( !aliases[verb] || is_doing_alias(verb) )
       return 0;
   return expand_alias( verb, args );
} /* run_alias() */

/**
 * This method checks to see if the player is doing the specified alias
 * already.
 * @param verb the verb to check
 * @see exec_alias()
 * @see set_doing_alias()
 */
protected int is_doing_alias(string verb) {
   if( !doing_alias )
       doing_alias = ([ ]);
   if( doing_alias[verb] )
       return 1;
   return 0;
} /* is_doing_alias() */

/**
 * This method sets us as currently doing the given alias.
 * @param verb the alias to do
 * @see is_doing_alias()
 */
protected void set_doing_alias(string verb) {
  doing_alias[verb] = 1;
  in_alias_command++;
} /* set_doing_alias() */

/**
 * This method expands the alias from the input string thingy.
 * @param verb the verb to expand
 * @param args the arguments to the verb
 * @see run_alias()
 */
protected string *expand_alias( string verb, string args ) {
   int i;
   int num;
   string *bits;
   string line;
   mixed stuff;
   string *ret;

   /* Default expansion :) */
   if( !aliases[verb] )
       return ({ verb + " " + args });

   ret = ({ });
   stuff = aliases[verb];
   line = "";
   if( !args )
       args = "";

   bits = explode( verb+" "+args, " ");
   for( i = 0; i < sizeof(stuff); i++ ) {
      if( stringp(stuff[i]) ) {
          line += stuff[i];
      } else {
          num = stuff[i] & ALIAS_MASK;
          switch (stuff[i] - num) {
          case NEW_LINE :
             ret += ({ line });
             line = "";
             break;
          case ALL_ARGS :
             line += args;
             break;
          case ONE_ARG  :
             if (num < sizeof(bits)) {
                line += bits[num];
             }
             break;
          case TO_ARG   :
             line += implode(bits[1..num], " ");
             break;
          case FROM_ARG :
             line += implode(bits[num..100], " ");
             break;
          case ALL_ARG  :
             i++;
             if (args == "") {
                line += stuff[i];
             } else {
                line += args;
             }
             break;
          case ARG_THING :
             i++;
             if (num < sizeof(bits)) {
                line += bits[num];
             } else {
                line += stuff[i];
             }
             break;
          case ALL_IFARG :
             i++;
             if (args == "") {
                i += stuff[i];
             }
             break;
          case IFARG_THING :
             i++;
             if (num >= sizeof(bits)) {
                i += stuff[i];
             }
             break;
          case ELSE_THING :
             i++;
             i += stuff[i];
             break;
          case CURR_LOC :
             i++;
             line += file_name(environment())+".c";
             break;
          case END_IF :
             break;
          }
      }
   }

   if( line != "" && line != " " )
       ret += ({ line });

   ret += ({ "END_ALIAS " + verb });
   return ret;

} /* expand_alias() */

/**
 * This is called to signify the end of an alias.  This is needed
 * keep track of recursive aliases and such like.
 * @param verb the verb name to remove
 * @return always returns 1
 * @see set_doing_alias()
 * @see is_doing_alias()
 */
int remove_alias_thing(string verb) {
   in_alias_command--;
   map_delete(doing_alias, verb);
   this_player()->adjust_time_left(-DEFAULT_TIME);
   return 1;
} /* remove_alias_thing() */
