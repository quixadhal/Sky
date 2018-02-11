
#include <command.h>
#include <tasks.h>
#include <weapon.h>

// Remove this to disable logging to /log/BAD_WEAPONS
// and creator informs when judging.
#define DEBUG 1

#define COST 25
#define SKILL "general.evaluating.weapons"
#define QUEST_AVE 140
#define QUEST_MAX 570

class weapon_data {
    int ave;
    int max;
    string *cmds;
}

inherit COMMAND_BASE;

private nosave string *_attack_types;
private nosave mapping _special_attacks;

/** @ignore yes */
mapping query_special_attacks() { return copy(_special_attacks); }

void create() {
    string dir, cmd, skill;

    _attack_types = ({"blunt", "sharp", "pierce"});
    _special_attacks = ([ ]);

    dir = DIR_GUILD_CMDS + "/fighting/";

    foreach( cmd in get_dir( dir + "*.c") )
        if( skill = ( dir + cmd[0..<3] )->query_attack_skill() )
            _special_attacks[cmd[0..<3]] = skill;

} /* create() */

/** @ignore yes */
private class weapon_data calc_rating( object weapon, object player ) {
    int i, size;
    int *damage;
    mixed data;
    string *names, name;
    class weapon_data ret;

    ret = new( class weapon_data );

    if( !size = sizeof( data = weapon->query_attack_data() ) )
        return ret;

    names = weapon->query_attack_names();
    ret->cmds = ({ });

    for( i = 0; i < size; i += W_ARRAY_SIZE ) {
        if( member_array( data[ i + W_TYPE ], _attack_types ) == -1 )
            continue;

        name = names[ i / W_ARRAY_SIZE ];

        if( data[ i + W_SKILL ] == _special_attacks[name] &&
            player->query_known_command( name ) )
            ret->cmds += ({ name });

        damage = data[ i + W_DAMAGE ];
        ret->ave += ( data[ i + W_CHANCE ] * ( damage[F_FIXED] +
                    ( damage[F_NUM] * ( 1 + damage[F_DIE] ) ) / 2 ) ) / 100;
        ret->max += damage[F_FIXED] + damage[F_NUM] * damage[F_DIE];
    }

    event( player, "inform", sprintf("%s - ave: %d max: %d",
        weapon->short(), ret->ave, ret->max ), "debug");

    return ret;

} /* calc_rating() */

/** @ignore yes */
void judge_against( object player, object first, object second ) {
    int grain, rating, rating_1, rating_2, difficulty, margin;
    class weapon_data data_1, data_2;

    data_1 = calc_rating( first, player );
    data_2 = calc_rating( second, player );

    difficulty = data_1->ave - data_2->ave;

    if( difficulty > 0 )
        difficulty = QUEST_AVE - difficulty;
    else
        difficulty = QUEST_AVE + difficulty;

    margin = ( difficulty + ( difficulty * ( data_1->max + data_2->max ) ) /
             ( 2 * QUEST_MAX ) ) / 2;

    switch( TASKER->perform_task( player, SKILL, difficulty - 25,
            TM_COMMAND ) ) {
      case AWARD:
        tell_object( player, "%^YELLOW%^"+replace( ({"As you scrutinise "
            "$weapons$, you recognise an aspect of judging that you didn't "
            "use before.", "You realise something else that you can use to "
            "better judge $weapons$.", "As you inspect $weapons$, you feel a "
            "sense of surety in your judgement." })[ random( 3 ) ],
            "$weapons$", query_multiple_short( ({ first, second }), "the") )+
            "%^RESET%^\n");
      case SUCCEED:
      break;
      default:
        tell_object( player, "You examine "+query_multiple_short(
            ({ first, second }), "the")+" closely, but you're not sure which "
            "is better.\n");
            return;
    }
    
    grain = sqrt( player->query_skill_bonus( SKILL ) );

    if( grain > 12 )
        grain = 12;

    rating_1 = QUEST_AVE * data_1->max + QUEST_MAX * data_1->ave;
    rating_1 = ( grain * rating_1 + QUEST_AVE * QUEST_MAX ) /
               ( 2 * QUEST_AVE * QUEST_MAX );
    rating_2 = QUEST_AVE * data_2->max + QUEST_MAX * data_2->ave;
    rating_2 = ( grain * rating_2 + QUEST_AVE * QUEST_MAX ) /
               ( 2 * QUEST_AVE * QUEST_MAX );
    rating = rating_1 - rating_2;

    if( rating > 0 )
        rating++;
    else
        rating--;

    rating = 6 + rating / 2;

    if( rating > 12 )
        rating = 12;

    if( rating < 0 )
        rating = 0;

    if( rating == 6 ) {
        tell_object( player, "You think that "+query_multiple_short(
            ({ first, second }), "the")+" are exactly the same.\n");
        return;
    }

    tell_object( player, "You think that "+first->the_short()+" is "+({
        "amazingly worse", "extremely worse", "very much worse",
        "much worse", "worse", "a bit worse", "!", "a bit better",
        "better", "much better", "very much better",
        "extremely better", "amazingly better" })[ rating ]+" than "+
        second->the_short()+".\n");

} /* judge_against() */

/** @ignore yes */
void judge_weapon( object player, object weapon ) {
    int grain, rating, margin;
    class weapon_data data;

    data = calc_rating( weapon, player );
    margin = ( data->ave + ( data->ave * data->max ) / QUEST_MAX ) / 2;

    switch( TASKER->perform_task( player, SKILL, data->ave + 25,
            TM_COMMAND ) ) {
      case AWARD:
        tell_object( player, "%^YELLOW%^"+replace( ({"As you scrutinise "
            "$weapon$, you recognise an aspect of judging that you didn't use "
            "before.", "You realise something else that you can use to judge "
            "$weapon$.", "As you inspect $weapon$, you feel a sense of surety "
            "in your judgement." })[ random( 3 ) ], "$weapon$",
            weapon->the_short() )+"%^RESET%^\n");
      case SUCCEED:
      break;
      default:
        tell_object( player, "You think that "+weapon->the_short()+" is a "
            "good weapon, but you're not sure.\n");
        return;
    }

    grain = sqrt( player->query_skill_bonus( SKILL ) / 2 );

    if( grain > 14 )
        grain = 14;

    rating = QUEST_AVE * data->max + QUEST_MAX * data->ave;
    rating = ( grain * rating + QUEST_AVE * QUEST_MAX ) /
             ( 2 * QUEST_AVE * QUEST_MAX );

    rating += 14 - grain;

    if( rating > 14 ) {
#ifdef DEBUG
        log_file( "BAD_WEAPON", "%s exceeds weapon specifications, rating is "
            "%d.\n", file_name( weapon ), rating );
#endif
        tell_object( player, "Please inform a creator that "+
            weapon->the_short()+" exceeds weapon specifications.\n");
        return;
    }

    tell_object( player, "You think that "+weapon->the_short()+" is "+({
        "an atrocius", "an extremely poor", "a very poor", "a rather poor",
        "a poor", "a pretty poor", "a quite poor", "a reasonable",
        "a quite good", "a pretty good", "a good", "a rather good",
        "a very good", "an extremely good", "an excellent" })[ rating ]+
        " weapon"+( sizeof(data->cmds) ? ", that can be used to "+
        query_multiple_short(data->cmds) : "")+".\n");

} /* judge_weapon() */

/** @ignore yes */
int valid_weapon( object player, object ob ) {
    if( living( ob ) ) {
        if( ob == player ) {
            add_failed_mess("You stare at your muscles for a moment, "
                "and decide that you're more than capable of destroying "
                "any enemy that decides to interfer with you.\n");
            return 0;
        }

        add_failed_mess("You stare at $I for a while, but decide it "
            "would be better if you considered them instead.\n", ({ ob }) );
        return 0;
    }

    if( !ob->query_weapon() ) {
        add_failed_mess("$C$$I is not a weapon.\n", ({ ob }) );
        return 0;
    }

    return 1;

} /* valid_weapon() */

/** @ignore yes */
int cmd( object first, object second ) {
    object *indirect_obs;
    function judge_func;
    int cost;

    if( !valid_weapon( TP, first ) )
        return 0;

    cost = COST;

    if( second ) {
        if( !valid_weapon( TP, second ) )
            return 0;

        cost *= 2;

        indirect_obs = ({ first, second });
        judge_func = (: judge_against :);
    } else {
        indirect_obs = ({ first });
        judge_func = (: judge_weapon :);
    }

    if( !TASKER->point_tasker( TP, "general", cost ) ) {
        add_failed_mess("You can't muster the energy to scrutinise "
            "$I at the moment.\n", indirect_obs );
        return 0;
    }

    call_out( judge_func, 0, TP, first, second );
    add_succeeded_mess("$N look$s closely at $I.\n", indirect_obs );
    return 1;

} /* cmd() */

/** @ignore yes */
mixed query_patterns() {
    return ({
        "<indirect:object:me'weapon'>", (: cmd( $1[0], 0 ) :),
        "<indirect:object:me'weapon'> against <indirect:object:me'weapon'>",
        (: cmd( $1[0][0], $1[1][0] ) :),
    });
} /* query_patterns() */
