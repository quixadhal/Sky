/**
 * The simul_efun object.  This object allows you to create functions which
 * apear to act like an efun.  An efun is a function which is built into
 * the driver.  For functions which are on objects you need to specify the
 * the object name when you call them.
 * ie: "/handlers/autodoc/autodoc_handler"->add_file(frog);
 * With a simuluated efun, you would not need to specifiy a file name.  This is
 * how the functions like match_objects_for_existence() and so on work.
 * @author Pinkfish
 */

/* Please keep the list in alphabetical order! */

#include <rank.h>

#if !efun_defined(add_a) || !efun_defined(vowel)
inherit "/secure/simul_efun/add_a";
#endif
#if !efun_defined(add_action)
inherit "/secure/simul_efun/add_action";
#endif
inherit "/secure/simul_efun/add_command";
#if !efun_defined(find_member) || !efun_defined(delete) || !efun_defined(insert)
inherit "/secure/simul_efun/array";
#endif
inherit "/secure/simul_efun/back_trace";
#if !efun_defined(dump_socket_status)
inherit "/secure/simul_efun/dump_socket_status";
#endif
inherit "/secure/simul_efun/extract";
inherit "/secure/simul_efun/find_other_call_out";
#if !efun_defined(indent)
inherit "/secure/simul_efun/indent";
#endif
inherit "/secure/simul_efun/inside_shorts";
inherit "/secure/simul_efun/modified_efuns";
inherit "/secure/simul_efun/pk_check";
inherit "/secure/simul_efun/obj_parser";
inherit "/secure/simul_efun/process_value";
#if !efun_defined(query_group)
inherit "/secure/simul_efun/query_group";
#endif
inherit "/secure/simul_efun/query_ident";
inherit "/secure/simul_efun/query_number";
#if !efun_defined(roll_MdN)
inherit "/secure/simul_efun/roll_MdN";
#endif
#if !efun_defined(shuffle)
inherit "/secure/simul_efun/shuffle";
#endif
inherit "/secure/simul_efun/snoop_simul";
inherit "/secure/simul_efun/sqrt";
inherit "/secure/simul_efun/string_to_define";
inherit "/secure/simul_efun/strip_colours";
inherit "/secure/simul_efun/debug";
inherit "/secure/simul_efun/thing_to_string";
inherit "/secure/simul_efun/time";
inherit "/secure/simul_efun/unguarded";

void create() {
    seteuid("Root");
    obj_parser::create();
} /* create() */

/**
 * This method returns whether or not the given player is an admin.
 * @param person the player to test
 * @return whether or not the player is an admin
 * @see lordp, seniorp, creatorp, liaisonp, playtesterp
 */
int adminp( mixed person ) {
    if( ( !person && previous_object(-1)[<1] == master() ) ||
        person == "Root")
        return 1;

    if( arrayp(person) )
        return sizeof( filter( person, (: adminp($1) :) ) );

    if( objectp(person) && !query_shadowing(person) )
        person = person->query_name();

    if( !stringp(person) )
        return 0;

    return DOMAIN_H->query_member("admin", person );

} /* adminp() */

/**
 * This method returns whether or not the given player is a creator.
 * @param person the player to test
 * @return whether or not the player is a creator
 * @see adminp, lordp, seniorp, liaisonp, playtesterp
 */
int creatorp( mixed person ) {
    if( arrayp(person) )
        return sizeof( filter( person, (: creatorp($1) :) ) );

    if( objectp(person) && !query_shadowing(person) )
        person = person->query_name();

    if( !stringp(person) )
        return 0;

    return DOMAIN_H->query_creator(person);

} /* creatorp() */

/**
 * This method returns whether or not the given player is a lord.
 * @param person the player to test
 * @return whether or not the player is a lord
 * @see adminp, seniorp, creatorp, liaisonp, playtesterp
 */
int lordp( mixed person ) {
    if( arrayp(person) )
        return sizeof( filter( person, (: lordp($1) :) ) );

    if( objectp(person) && !query_shadowing(person) )
        person = person->query_name();

    if( !stringp(person) )
        return 0;

    if( adminp(person) )
        return 1;

    return DOMAIN_H->query_leader(person);

} /* lordp() */

/**
 * This method returns whether or not the given player is a liaison.
 * @param person the player to test
 * @return whether or not the player is a liaison
 * @see adminp, seniorp, creatorp, lordp, playtesterp
 */
int liaisonp( mixed person ) {
    if( arrayp(person) )
        return sizeof( filter( person, (: liaisonp($1) :) ) );

    if( objectp(person) && !query_shadowing(person) )
        person = person->query_name();

    if( !stringp(person) )
        return 0;

    return DOMAIN_H->query_member("liaison", person );

} /* liaisonp() */

/**
 * This method returns whether or not the given player is a senior creator.
 * @param person the creator to test
 * @return whether or not the player is a senior creator
 * @see adminp, lordp, creatorp, liaisonp, playtesterp
 */
int seniorp( mixed person ) {
    if( arrayp(person) )
        return sizeof( filter( person, (: seniorp($1) :) ) );

    if( objectp(person) && !query_shadowing(person) )
        person = person->query_name();

    if( !stringp(person) )
        return 0;

    if( lordp(person) )
        return 1;

    return DOMAIN_H->query_senior(person);

} /* seniorp() */

/**
 * This method returns whether or not the given player is a playtester.
 * This will return 1 for creators as well, unless the optional nocre
 * flag is set.
 * @param person the player to test
 * @param optional flag to make it return 0 for creators
 * @return whether or not the player is a playtester
 * @see adminp, lordp, seniorp, creatorp, liaisonp
 */
varargs int playtesterp( mixed person, int nocre ) {
    if( objectp(person) )
        person = person->query_name();

    if( !stringp(person) )
        return 0;

    if( !nocre && creatorp(person) )
        return 1;

    return PLAYTESTERS_H->query_playtester(person);

} /* playtesterp() */

/**
 * This method returns the rank level of the player.
 * @param person the player to get the rank for
 * @return the rank of the player as defined in rank.h
 * @see adminp, lordp, seniorp, creatorp, playtesterp, liaisonp
 */
int rank( mixed person ) {
    if( objectp(person) )
        person = person->query_name();

    if( adminp( person ) )
        return ADMIN;

    if( lordp( person ) )
        return LORD;

    if( seniorp( person ) )
        return SENIOR;

    if( DOMAIN_H->query_member("learning", person ) )
        return APPRENTICE;

    if( creatorp( person ) )
        return CREATOR;

    if( PLAYER_H->test_user(person) )
        return PLAYER;

    return 0;

} /* rank() */

/** @ignore yes */
void alt_move( mixed dest, object ob ) {
    if( !ob )
        return;
    evaluate( bind( (: move_object, dest :), ob ) );
} /* alt_move() */

#if !efun_defined(file_exists)
/**
 * This method returns true if the file str exists.
 * @param str the file name to test
 * @return 1 if the file exists
 * @see dir_exists, file_size
 */
int file_exists( string str ) { return ( file_size(str) > -1 ); }
#endif

#if !efun_defined(dir_exists)
/**
 * This method returns true if the directory str exists.
 * @param str the directory path to test
 * @return 1 if the directory exists
 * @see file_exists, file_size
 */
int dir_exists( string str ) { return ( file_size(str) == -2 ); }
#endif
