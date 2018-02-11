/**
 * Handles commands from directories for living objects.
 *    /global/player/command.c
 *    from the Nightmare IVr1 Object Library
 *    handles commands of living objects
 *    created by Descartes of Borg 950323
 *    Hacked for Discworld by Turrican 4-11-95
 */

#include <command.h>

nosave string current_verb;
private nosave int Forced;
private nosave string *SearchPath;

protected void create() {
    current_verb = "";
    SearchPath = ({ DIR_LIVING_CMDS });
} /* create() */

/** What is the current verb? */
string query_current_verb() { return current_verb; }

/** @ignore yes */
void set_current_verb( string str ) {
    string *bits;

    if( !str || str == "" || str[0] == ',' )
        return;

    bits = explode( str, " ") - ({"", 0 });
    current_verb = bits[0];

} /* set_current_verb() */

/**
 * Try and execute the command.  It calls the function command_shadowed
 * with the verb and args as parameters if the command is found to exist.
 * @param args the args to use for the verb
 * @return 1 on success, 0 on failure
 */
protected int cmdAll( string args ) {
    class command verb;
    int ret;

    verb = new( class command );
    verb->args = args;

    if( !CMD_D->GetCommand( verb, SearchPath ) )
        return 0;

    current_verb = verb->verb;

    if( CMD_D->IsGRCommand( verb->filepart ) &&
        !TO->query_known_command( verb->verb ) )
        return 0;

    if( TO->command_shadowed( verb->verb, verb->args ) )
        return 1;

    if( !ret = call_other( verb->file, "cmd", verb->args, verb->verb ) )
        return 0;

    return 1;

} /* cmdAll() */

/**
 * This method tries to lookup the patterns for the given verb.
 * @param verb the verb to get the patterns for
 * @return the array of patterns
 * @see /handlers/pattern_compiler.c
 * @see /gloal/new_parse->add_command()
 */
protected mixed cmdPatterns( string verb ) {
    if( CMD_D->IsGRCommand(verb) && !TO->query_known_command(verb) )
        return 0;
    return CMD_D->GetCommandPatterns( verb, SearchPath );
} /* cmdPatterns() */

/**
 * A force event.
 * @param cmd the command to force
 * @return 0 on failure and 1 on success
 */
int eventForce( string cmd ) {
    string err;
    int res;

    if( member_array( call_stack(0)[1], ({"/cmds/creator/showhelp.c",
        "/handlers/new_soul.c", "/std/living/force.c"}) ) == -1 )
        error("Illegal force.\n");

    Forced = 1;
    err = catch( res = command(cmd) );
    Forced = 0;

    if( err )
        error(err);

    return res;

} /* eventForce() */

/**
 * Add a directory(s) into the search path for commands.  If the input is
 * an array then the members of that are added to the search path.
 * @param val the path to array
 * @return the new search path
 */
string *AddSearchPath( mixed val ) {
    if( stringp(val) )
        val = ({ val });
    else if( !pointerp(val) )
        error("Bad argument 1 to AddSearchPath()\n");

    SearchPath = uniq_array( SearchPath + val );

    return SearchPath;

} /* AddSearchPath() */

/**
 * Remove a directory(s) from the search path.  If the input is
 * an array then the members of that are added to the search path.
 * @param val the directory(s) to remove
 * @return the new search path
 */
string *RemoveSearchPath( mixed val ) {
    if( stringp(val) )
        val = ({ val });
    else if( !pointerp(val) )
        error("Bad argument 1 to RemoveSearchPath()\n");

    SearchPath -= val;

    return SearchPath;

} /* RemoveSearchPath() */

/**
 * This method returns the current search path
 * @return the current search path
 */
string *GetSearchPath() { return SearchPath; }

/**
 * This method returns if we are being forced or not
 * @return 1 if we are being forced
 */
int GetForced() { return Forced; }

/**
 * This tells us if we are a client.
 * @return always returns 0
 */
string GetClient() { return 0; }
