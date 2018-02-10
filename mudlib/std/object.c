/**
 * The basic object.  This pulls together all the bits needed to create
 * a basic object.
 * @author Pinkfish
 * @changed Many many times by many people
 * @changed Added set_determinate() function - Shaydz - 04/03/2001
 * @changed Changed to inherit the effects inheritable, instead of
 * using a shadow for effects. - Sandoz, 11/04/03
 * @see /std/room/basic_room.c
 */

#include <cmr.h>

inherit "/std/basic/effects";
inherit "/std/basic/extra_look";
inherit "/std/basic/enchant";
inherit "/std/basic/id";
inherit "/std/basic/misc";
inherit "/std/basic/property";
inherit "/std/basic/read_desc";
inherit "/std/basic/auto_load";
inherit "/std/basic/help_files";

#define AUTO_LOAD_TAG "basic.object";

nosave int do_setup;
nosave string create_me;
nosave string colour;
nosave string material;
nosave object player;

void create() {
    id::create();
    misc::create();
    property::create();
    read_desc::create();

    seteuid( master()->creator_file( file_name(TO) ) );

    create_me = ( TP && TP->query_name() ? TP->query_name() : "who knows");

    if( PO )
        create_me += " ("+file_name(PO)+")";

    if( !do_setup )
        TO->setup();

} /* create() */

/**
 * Gives the object a name.  Sets the name to the word and makes it the
 * short description  and attempts to create the pural short description
 * from it.
 * @param name the name to use
 */
void set_name( string word ) {
    if( query_name() && query_name() != "object") {
        ::set_name(word);
        return;
    }

    ::set_name(word);

    if( !short_d )
        short_d = word;

    add_plural( pluralize( word ) );

} /* set_name() */

/** @ignore yes */
void add_effect( string eff, mixed arg ) {
    if( !pointerp(query_effs() ) )
        effects::create();
    effects::add_effect( eff, arg );
} /* add_effect() */

/** @ignore yes */
string query_long_details( string arg, int dark, object looker ) {
    string stuff;

    stuff = "";

    if( TP->query_see_octarine() )
        stuff += enchant_string();

    if( sizeof( query_read_mess() ) ) {
        int group;

        group = query_group(TO);

        stuff += ( group ? "They appear" : "It appears")+" to have "
            "something written on "+( group ? "them" : "it")+".\n";
    }

    return stuff;

} /* query_long_details() */

/**
 * The description of an object when someone looks at it.  This hanedles
 * the extra code for messages, colour and material.
 * @param word the worc it was matched on
 * @param dark is it dark fom the players perspecitive
 * @return the long description
 */
string long( string word, int dark ) {
    string stuff;

    stuff = ::long( word, dark ) + calc_extra_look() +
            query_long_details( word, dark, TP );

    if( colour )
        stuff = replace( stuff, "$colour$",
           (string)CMR_HANDLER->identify_colour( colour, TP ) );

    if( material )
        stuff = replace( stuff, "$material$",
           (string)CMR_HANDLER->identify_material( material, TP, 1 ) );

    return stuff;

} /* long() */

/**
 * This method returns the colour of the object.
 * @return the colour of the object
 */
string query_colour() { return colour; }

/**
 * This method sets the colour of the object.
 * @param word the colour to make the object
 */
void set_colour( string word ) { colour = word; }

/**
 * This method returns the material of the object.
 * @return the name of the material
 */
string query_material() { return material; }
/**
 * This method sets the material of the object.
 * @param word the material to set it to
 */
void set_material( string word ) { material = word; }

/**
 * The pronoun of the object.
 * @return returns "it" or "they"
 */
string query_pronoun() {
    if( query_group(TO) )
        return "they";
     return "it";
} /* query_pronoun() */

/**
 * The possessive of the object.
 * @return returns "its" or "their"
 */
string query_possessive() {
    if( query_group(TO) )
        return "their";
    return "its";
} /* query_possessive() */

/**
 * The objective of the object.
 * @return returns "it" or "them"
 */
string query_objective() {
    if( query_group(TO) )
        return "them";
    return "it";
} /* query_objective() */

/**
 * Who cloned the object.  Used for security reasoin to trace who have
 * been cloning up objects for players.
 * @return the person who cloned the object
 */
string query_cloned_by() { return create_me; }

/** @ignore yes */
mixed stats() {
    return property::stats() + misc::stats() + effects::stats() + ({
        ({"name" , query_name(), }),
        ({"short", short(0), }),
        ({"plural", query_plural(0), }),
        ({"weight", (int)TO->query_weight(), }),
        ({"enchantment", query_enchant(), }),
        ({"colour", colour, }),
        ({"material", material, }),
        ({"cloned by", create_me, }),
     });
} /* stats() */

/** @ignore yes */
mapping int_query_static_auto_load() {
    mapping map;

    map = ([ "name" : query_name(),
             "short" : short_d,
             "long" : long_d,
             "alias" : query_alias(),
             "adjective" : query_adjectives(),
             "plural" : query_plurals(),
             "value info" : value_info + ([ ]),
             "weight" : weight,
             ]);

    if( colour )
        map["colour"] = colour;
    if( material )
        map["material"] = material;
    if( value )
        map["value"] = value;
    if( plural_d )
        map["main plural"] = plural_d;

    return map;

} /* int_query_static_auto_load() */

/** @ignore yes */
mapping query_static_auto_load() {
    if( !query_name() || query_name() == "object")
        return 0;
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return ([ ]);
} /* query_static_auto_load() */

/** @ignore yes */
mapping query_dynamic_auto_load() {
    mapping map;

    if( !query_name() || query_name() == "object")
        return 0;

    if( query_enchant() > TO->query_max_enchant() )
        set_enchant( TO->query_max_enchant() );

    map = ([ "cloned by" : create_me ]);

    if( sizeof( query_read_mess() ) )
        map["read mess"] = query_read_mess();

    if( query_real_enchant() )
        map["enchantment"] = query_real_enchant();

    if( query_enchant_set_time() )
        map["enchantment time"] = query_enchant_set_time();

    if( query_my_light() )
        map["light"] = query_my_light();

    if( timed_properties && sizeof(timed_properties) ) {
        // Ready the timed props for saving.
        save_player_properties();
        map["timed properties"] = copy(timed_properties);
        // Fix up the timed props again.
        reload_player_properties();
    }

    if( map_prop && sizeof(map_prop) )
        map["properties"] = copy( map_prop );

    if( sizeof( query_effs() ) ) {
        effect_freeze();
        effects_saving();
        map["effects"] = ({ query_effs(), query_eeq() });
        effect_unfreeze();
    }

    return map;

} /* query_dynamic_auto_load() */

/** @ignore yes */
void init_static_arg( mapping map ) {
    if( !mapp( map ) )
        return;

    if( !undefinedp( map["name"] ) )
        set_name( map["name"] );
    if( !undefinedp( map["short"] ) )
        set_short( map["short"] );
    if( !undefinedp( map["main plural"] ) )
        set_main_plural( map["main plural"] );
    if( !undefinedp( map["long"] ) )
        set_long( map["long"] );
    if( map["alias"] )
        set_aliases( map["alias"] );
    if( map["adjective"] )
        set_adjectives( map["adjective"] );
    if( map["plural"] )
        set_plurals( map["plural"] );
    if( !undefinedp( map["value"] ) )
        set_value( map["value"] );
    if( !undefinedp( map["value info"] ) )
        value_info = map["value info"];
    if( !undefinedp( map["weight"] ) )
        set_weight( map["weight"] );
    if( !undefinedp( map["colour"] ) )
        set_colour( map["colour"] );
    if( !undefinedp( map["material"] ) )
        set_material( map["material"] );

} /* init_static_arg() */

/**
 * Sets the player associated with the object.  This is used during the
 * autoload sequence.
 * @param thing the player object we belong to
 * @see /global/auto_load.c
 */
void set_player( object thing ) { player = thing; }

/**
 * This method returns the player associated with the object.
 * @return the player who loaded the object
 */
object query_player() { return player; }

/** @ignore yes */
void init_dynamic_arg( mapping map ) {
    int recycle;

    if( !mapp( map ) )
        return;
    /*
     * Because recycling needs to be preserved into new objects from
     * olds ones (generally speaking).
     */
    recycle = query_property(NO_RECYCLE);

    if( map["properties"] )
        map_prop = map["properties"];

    if( recycle )
        add_property( NO_RECYCLE, recycle );

    if( map["timed properties"] ) {
        timed_properties = map["timed properties"];
        reload_player_properties();
    }

    if( sizeof( map["read mess"] ) )
        set_read_mess( map["read mess"] );
    if( map["enchantment"] )
        set_enchant(map["enchantment"]);
    if( map["enchantment time"] )
        set_enchant_set_time( map["enchantment time"] );
    if( map["light"] )
        set_light( map["light"] );

    create_me = map["cloned by"];

    if( map["effects"] ) {
        set_effs( map["effects"][ 0 ] );
        set_eeq( map["effects"][ 1 ] );
        init_after_save();
    }

} /* init_dynamic_arg() */

/** @ignore yes */
string query_readable_message( object player, int ignore_labels ) {
    string ret, bing;
    object *labels, ob, store;
    int *enums;

    ret = ::query_readable_message(player);

    if( !ignore_labels ) {
        enums = TO->effects_matching("object.label");
        if( !sizeof( enums ) )
            labels = ({ });
        else {
            store = (object)TO->arg_of( enums[ 0 ] )[ 0 ];
            labels = ( !objectp( store ) ? ({ }) :
                     (object *)store->find_inv_match("") );
        }
        if( sizeof(labels) && !ret )
            ret = "";
        foreach( ob in labels ) {
            ret += "On "+ob->the_short()+" is written:\n";
            ret += ( ( bing = ob->query_readable_message( player, 0 ) ) ?
                       bing : "Nothing.\n" );
        }
    }

    return ret;

} /* query_readable_message() */

/** @ignore yes */
string query_read_short( object player, int ignore_labels ) {
   string ret;
   object *labels, store;
   int *enums;

   ret = ::query_read_short(player);

   if( !ignore_labels ) {
       enums = TO->effects_matching("object.label");
       if( !sizeof( enums ) )
           labels = ({ });
       else {
           store = (object)TO->arg_of( enums[ 0 ] );
           labels = ( !objectp( store ) ? ({ }) :
                    (object *)store->find_inv_match("") );
       }

       if( sizeof(labels) ) {
           if( ret )
               ret += " and " + query_multiple_short( labels, "the")+
                      " stuck on $name$";
           else
               ret = query_multiple_short( labels, "the")+
                     " stuck on $name$";
       }
   }

   return ret;

} /* query_read_short() */

/**
 * This method adds a determinate to the object.
 * Generally this should be set to "the " or "" for unique items,
 * "a " or "an " for non-uniques.
 * If no determinate is set for an object, a/an will be used
 * automatically as needed.
 * @param str the string to set as the determinate
 */
void set_determinate( string str ) { add_property("determinate", str ); }

/** @ignore yes */
string query_help_file_directory() { return "/doc/object/"; }
