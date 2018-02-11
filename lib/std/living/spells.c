#include <spells.h>

mapping spells;

void create() {
//    spells = ([ ]); // Don't do it, saves memory.
} /* create() */

/** @ignore yes */
string help_spell( string str ) {
    if( !mapp(spells) || !sizeof(spells) || !spells[str] )
        return 0;
    return (string)spells[str][S_OBJECT]->help(str);
} /* help_spell() */

/**
 * This method returns the spells mapping of the living object.
 * @return the spells mapping
 */
mixed query_spells() { return spells || ([ ]); }

/**
 * This method adds a spell to a living object's known spells mapping
 * @param name the name of the spell that can be used to access the
 * spell while casting
 * @param ob the actual spell object
 * @param func a spell function (unused?)
 * @return 1 if the spell was added successfully, 0 if not
 * @example player->add_spell("Ethereal Light", "/obj/spells/ethereal_light");
 */
int add_spell( string name, mixed ob, mixed func ) {
    if( file_size( ob+".c" ) < 0 )
        return 0;

    if( !mapp(spells) )
        spells = ([ ]);

    spells[name] = ({ ob, func });

    return 1;

} /* add_spell() */

/**
 * This method removes a spell from the spells mapping of a living object
 * @param name the name of the spell to remove
 * @return always returns 1
 */
int remove_spell( string name ) {
    if( !mapp(spells) )
        return 0;
    map_delete( spells, name );
    return 1;
} /* remove_spell() */

/**
 * This method returns the specified spell's spell object and function
 * @param name the name of the spell to get the data for
 * @return the spell object and function
 */
mixed query_spell( string name ) {
    if( !mapp( spells ) )
        return 0;
    return spells[ (string)TO->expand_nickname( name ) ];
} /* query_spell() */
