/**
 * This class controls the entire command queue for all player
 * objects.  Whenever a player tries to execute a command,
 * it gets placed in a queue here and eventually (during the
 * player's heart_beat()), the command will (hopefully) be executed.
 *
 * <p>Since this class is a nexus of control for player commands, it
 * is also the natural place for a number of other utilities and
 * functions which also affect the execution of all commands.
 * Among these are things like drunk_check(), do_soul(), etc.
 *
 * @see /global/player/new_parse->add_command()
 * @see /global/player->heart_beat()
 * @see query_passed_out_message()
 *
 * @author Pinkfish
 * @changed 3 November 1997 -- Sin
 *      Documented the bejeesus out of this thing.
 * @changed 4 Novemebr 1997 - Pinkfish
 *      Updated the documentation and changed the interrupt system slightly.
 * @changed 22 Feburary 1998 - Pinkfish
 *      Fixed up the problems with the queueing system.
 * @changed 28 Feburary 2002 - Sandoz
 *      Fixed up a problem in _process_input with printf() erroring
 *      on error messages with % in them.
 */

#include <player.h>

inherit "/global/player/alias";
inherit "/global/player/nickname";

nosave int time_left;
nosave int doing_it;
nosave int last_command;
nosave int bypass_queue;
nosave int flush_call_id;
nosave string *queued_commands;
nosave string in_command;
nosave mixed interrupt;
nosave private function cmd = 0;

/** @ignore  */
void create() {
    time_left = ROUND_TIME;
    last_command = time();
    queued_commands = ({ });
} /* create() */

/**
 * To make the next single command be executed directly rather
 * than being placed in the command queue, call this function.
 */
void bypass_queue() { bypass_queue = 1; }

/**
 * Ensure that the player has no more time for executing commands.
 * This will force the next command to be queued.
 */
void no_time_left() {
    time_left = -ROUND_TIME;
} /* no_time_left() */

/**
 * This is a setup function that is called by the player object.
 * It is used to register the lower_check() and drunk_check()
 * functions.  Plus it initializes the alias object, the
 * nickname object, and the history object.
 */
protected int drunk_check( string str );

/** @ignore yes */
void soul_commands() {
    alias_commands();
    nickname_commands();
    history_commands();
} /* soul_commands() */

/**
 * You can use this function to see if there are any commands
 * queued for this player.
 * @return the number of queued commands
 */
int query_queued_commands() {
    return sizeof(queued_commands);
} /* query_queued_commands() */

/**
 * The amount of time units left.  A time unit is 1/40th of a second.
 */
int query_time_left() { return time_left; }

/**
 * Change the amount of time a player has left.  You call this after a command
 * has been executed to make it take more time.
 * @param i the amount of time units to change by
 * @return the amount of time left
 */
int adjust_time_left( int i ) {
    return time_left += i;
} /* adjust_time_left() */

/** @ignore yes */
private void do_flush( int first ) {
    int i;
    string str;

    if( time_left < 1 || !sizeof( queued_commands ) ||
        ( TO->queue_commands(queued_commands[0]) && !TO->query_creator() ) )
        return;

    if( !first ) {
        str = queued_commands[0];
        queued_commands = queued_commands[1..];

        doing_it = 1;
        catch( command(str) );
        doing_it = 0;

        // The end!
        if( !sizeof(queued_commands) ) {
            queued_commands = ({ });
            doing_alias = ([ ]);
        }

        return;
    }

    for( i = 0; i < 2 && i < sizeof(queued_commands); i++ )
        flush_call_id = call_out( (: do_flush(0) :), 1 );

    flush_call_id = call_out( (: do_flush(1) :), 2 );

} /* do_flush() */

/** @ignore yes */
private void call_interrupt( int time_left, object interupter ) {
    mixed stuff;

    stuff = interrupt;
    interrupt = 0;

    // The previous object is the person interupting us.
    if( pointerp(stuff) )
        catch( call_other( stuff[1], stuff[0], time_left, stuff[2], TO,
                           interupter, in_command ) );
    else if( functionp(stuff) )
        catch( evaluate( stuff, time_left, TO, interupter, in_command ) );

} /* call_interrupt() */

/**
 * This method flushes all the queued commands.  It increments the time by the
 * ROUND_TIME define and checks to see if any of the commands now need to be
 * executed.  This should be called each heart beat..
 * @see /global/player->heart_beat()
 */
protected void flush_queue() {
    time_left += ROUND_TIME;

    if( time_left > ROUND_TIME )
        time_left = ROUND_TIME;

    remove_call_out( flush_call_id );
    do_flush(1);

    if( !sizeof(queued_commands) ) {
        // Check to see if an interupt was set up.
        if( interrupt && time_left > 0 )
            call_interrupt( 0, TO );

        in_alias_command = 0;
        doing_alias = ([ ]);

        if( !sizeof(queued_commands) )
            return;

        doing_alias = ([ ]);
        in_alias_command = 0;
    }

} /* flush_queue() */

/**
 * Sets the function to be executed if the command is interrupted.
 * It is also executed if the command finished.  If it is interrupted
 * the first argument to the called function will be the amount of time
 * it had left to complete.  If it complets successfuly, this
 * argument will be 0.  If the first argument is a function pointer,
 * this will be used instead.<p>
 *
 * Eg: set_interupt_command("frog", TO);<p>
 * void frog(int time_left, mixed arg) { <p>
 * ... <p>
 * }
 *
 * @param func the function to call back
 * @param ob the object to call the function on
 * @param arg the argument to pass to the function
 * @example
 * ...
 * void frog(int time_left, mixed arg);
 * ...
 * set_interupt_commant( (: frog :) );
 * ...
 * void frog(int time_left, mixed arg) {
 *    ...
 * } /\* frog() *\/
 */
void set_interupt_command(mixed func, mixed ob, mixed arg) {
    if( !functionp(func) ) {
        interrupt = ({ func, ob, arg });
        if( !stringp( func ) )
            interrupt = 0;
    } else {
        interrupt = func;
    }
} /* set_interupt_command() */

/**
 * This method returns the current value associated with tine interupt
 * command.
 * @return the current interupt command data
 */
mixed query_interupt_command() {
    return interrupt;
} /* query_interupt_command_func() */

/**
 * This one only takes a function pointer as an input.
 * @param func the function pointer to call back with
 */
void set_interrupt_command( function func ) {
    set_interupt_command( func, 0, 0 );
} /* set_interrupt_command() */

/**
 * This is called by the stop command.  It sets the entire queue back to
 * empty.  It calls the interrupt functions and stuff if they need to be
 * called.
 */
void remove_queue() {
    queued_commands = ({ });

    if( interrupt && time_left < 0 )
        call_interrupt( -time_left, TO );

    tell_object( TO, "Removed queue.\n");

    if( lordp(TO) ) {
        // Just in case something really bad happens...  Let lords fix it.
        time_left = 0;
    } else {
        // Make sure they cannot do anything for a heartbeat.
        time_left = -DEFAULT_TIME;
    }
} /* remove_queue() */

/**
 * This method interupts the current command.
 * @param interupter the person interupting the command
 */
void interupt_command( object interupter ) {
    if( interupter )
        call_interrupt( -time_left, interupter );
} /* interupt_command() */

/**
 * Use this function to set a function that is called with the players input
 * before the command handlers get to it, return 1 from the function if the
 * input needs no further parsing (ie the command is handled)
 * @param func function in the players environment to call.
 */

void command_override( function func ) {
    if( !functionp(func) )
        error("command_override needs a function!");
    cmd = func;
} /* command_override() */

/**
 * This poorly named function was originally used to affect the
 * player's behavior when they are drunk, and to prevent any
 * player from doing anything in the event that they are passed
 * out.  Now the function also is responsible for adding commands
 * to the player's command queue, for implementing the 'stop'
 * and 'restart' commands, and for ensuring that the player can quit
 * the game, even when queueing.
 *
 * <p>To see if a player is passed out, it checks the
 * "passed out" property.  If that property is nonzero, then
 * the player will be prevented from doing the command unless
 * that player is also a creator.  By default, it will print
 * a message that says: "You are unconscious.  You can't do
 * anything.\n".  If the function query_passed_out_message() is
 * defined on the player object (usually by a shadow), and
 * returns a string, then that string is printed instead.
 *
 * @return 0 if nothing was done, 1 if drunk_check() blocked
 * the command.
 * @param str the command being executed
 *
 * @see /global/new_parse->add_command()
 */
protected int drunk_check( string str ) {
    string *rabbit, *green, mess, comm, arg;

    if( cmd ) {
        object owner;

        owner = function_owner(cmd);

        if( owner && owner == ENV(TP) ) {
            int ret;

            if( ret = evaluate( cmd, str ) )
                return ret;
        } else {
            cmd = 0;
        }
    }

    if( sizeof( rabbit = explode( str, " ") ) == 1 )
        if( rabbit[0] == ",")
            return 0;

    if( in_command == str ) {
        in_command = 0;
        sscanf( str, "%s %*s", str );
        if( is_doing_alias(str) )
            notify_fail("Recursive aliases. Bad "+({"thing", "boy",
                "girl"})[TO->query_gender()]+".\n");
        return 0;
    }

    if( bypass_queue ) {
        bypass_queue = 0;
        return 0;
    }

    last_command = time();

    if( TO->query_property(PASSED_OUT) || !interactive(TO) ) {
        if( str == "quit")
            return 0;

        if( !stringp( mess = TO->query_passed_out_message() ) )
            mess = "You are unconscious.  You can't do anything.\n";

        write( mess );

        if( !TO->query_creator() )
            return 1;

        write("On the other hand, you're a creator...\n");
    }

    if( str == "stop" || str == "restart") {
        remove_queue();
        return 0;
    }

    if( stringp(str) && str[0..4] == "stop ")
        return 0;

    /*
     * If: there's no time left
     * or: commands are to be queued (e.g. spell casting) and this is a player
     * or: if we have queueing commands and we are not currently executing
     *     a command off the stack
     * or: we are trying to do a flush
     * then queue the command.
     */
    if( time_left < 0 || ( TO->queue_commands(str) && !TO->query_creator() ) ||
        ( !doing_it && ( sizeof( queued_commands ) ||
        find_call_out( flush_call_id ) != -1 ) ) ) {
        // Only print commands which are not in upper case.
        rabbit = explode( str, " ");
        if( rabbit[0] != upper_case( rabbit[0] ) )
            write("Queued command: "+str+"\n");

        if( str == "quit")
            write("If you are trying to quit and it is queueing things, use "
                "\"stop\" to stop your commands, or \"restart\" to restart "
                "your heartbeat.\n");
        // The command should always go on the end because the aliases
        // in the queue are expanded elsewhere...
        queued_commands += ({ str });
        return 1;
    }

    if( interrupt )
        call_interrupt( 0, TO );

    interrupt = 0;
    in_command = str;

    // Get the args and stuff to run the alias.
    if( sscanf( str, "%s %s", comm, arg ) != 2 ) {
        comm = str;
        arg = "";
    }

    if( rabbit = run_alias( comm, arg ) ) {
        // Set us as running the alias.
        set_doing_alias(comm);

        green = queued_commands;
        queued_commands = ({ });

        foreach( comm in rabbit )
            catch( command(comm) );

        queued_commands += green;

    } else if( sizeof(str) > 1024 ) {
        write("Command too long.\n");
    } else {
        if( str[0..8] != "END_ALIAS")
            time_left -= DEFAULT_TIME;
        command(str);
    }

    if( interrupt && time_left >= 0 )
        call_interrupt( 0, TO );

    return 1;

} /* drunk_check() */

/** @ignore yes */
protected string _process_input( string str ) {
    object ob;

    if( str == "" )
        return 0;

    ob = TP;
    set_this_player(TO);
    _notify_fail(0);

    if( !sizeof( explode( str, " " ) - ({"", 0 }) ) || str[0] == ',' ) {
        efun::tell_object( TO, "What?\n");
        set_this_player(ob);
        return 0;
    }

    if( !drunk_check(str) && !TO->exit_command(str) && !TO->cmdAll(str) &&
        !TO->new_parser(str) && !TO->lower_check(str) ) {
        efun::tell_object( TO, query_notify_fail() || "What?\n");
        set_this_player(ob);
        return 0;
    }

    set_this_player(ob);
    return "bing";

} /* _process_input() */

/**
 * This is the command called by the driver on a player object every
 * time a command is executed.  It expands the history comands.
 * @param str the string to expand
 * @return the expanded history string
 */
protected string process_input( string str ) {
    reset_eval_cost();

    if( str[0] == '.' )
        str = expand_history(str[1..]);
    else if( str[0] == '^' )
        str = substitute_history(str[1..]);

    TO->add_history(str);
    _process_input(str);

    return 0;

} /* process_input() */

/**
 * This function will get called when all other commands and actions
 * have refused to do anything for this input from the user.  This
 * function adds some extra time for the user, and then returns.
 * @param str the user's input
 * @return 1 if the user's input is "stop", otherwise 0.
 */
int lower_check( string str ) {
    query_time_left();
    return str == "stop";
} /* lower_check() */
