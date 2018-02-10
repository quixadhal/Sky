/**
 * This is the item object for add items in rooms.
 * @author Someone from DW, most likely.
 * @change Added built in day/night support and
 * cleaned up the command parsing stuff - Sandoz, April 2002.
 */

#include <parse_command.h>
#include <obj_parser.h>
#include <room.h>

#define POSITION_SIZE 2
#define POSITION_DESC 0
#define POSITION_MULT 1

// Item descriptions.
private string *_shrt;
private mixed _lng;
// Optional things.
private mapping _verb, _plural, _other, _pattern, _gather, _pos_stuff;
// Item indexes.
private int *_cur_desc, *_day, *_night;
// The room that ownz us.
nosave object _room;

/** @ignore yes */
void set_my_room( object ob ) { _room = ob; }

/** @ignore yes */
object query_my_room() { return _room; }

/** @ignore yes */
int query_item_available( int i ) {
    if( WEATHER_H->query_day() )
        return member_array( i, _night ) == -1;
    return member_array( i, _day ) == -1;
} /* query_item_available() */

/** @ignore yes */
int query_common_item( int i ) {
    return member_array( i, ( _day + _night ) ) == -1;
} /* query_day_item() */

/** @ignore yes */
int query_day_item( int i ) {
    return member_array( i, _day ) != -1;
} /* query_day_item() */

/** @ignore yes */
int query_night_item( int i ) {
    return member_array( i, _night ) != -1;
} /* query_day_item() */

/** @ignore yes */
string print_stuff() {
    return sprintf("lng = %O\nshrt = %O\nverb = %O\nplural = %O\nother = %O\n"
        "pattern = %O\ncur_desc = %O\ngather = %O\nday = %O\n"
        "night = %O\n", _lng, _shrt, _verb, _plural, _other, _pattern,
        _cur_desc, _gather, _day, _night );
} /* print_stuff() */

/** @ignore yes */
void init() {
    string name;
    mapping rest;

    foreach( name, rest in _other )
        if( sizeof( filter( keys(rest), (: query_item_available($1) :) ) ) )
            add_command( name, _pattern[name] );

} /* init() */

/**
 * This method is called whenever an item is added with a command
 * pattern, this way if we add an item while someone is already
 * in the room, they will get the command immediately as well.
 * This should also be called whenever day turns to night and
 * vice versa.
 */
void do_init() {
    object *obs;
    string name;
    mapping rest;

    if( !_room || !sizeof( obs = filter( INV(_room), (: living($1) :) ) ) )
        return;

    obs->remove_object2(TO);

    foreach( name, rest in _other )
        if( sizeof( filter( keys(rest), (: query_item_available($1) :) ) ) )
            obs->add_command( name, TO, _pattern[name] );

} /* do_init() */

/** @ignore yes */
void create() {
    _lng = ({""});
    _shrt = ({""});
    _gather = ([ ]);
    _verb = ([ ]);
    _plural = ([ ]);
    _pattern = ([ ]);
    _other = ([ ]);
    _day = ({ });
    _night =  ({ });
    _cur_desc = ({ });
} /* create() */

/** @ignore yes */
int query_visible( object thing ) { return 1; }

/** @ignore yes */
string hide_invis_string() { return ""; }

/**
 * This method returns the base array of shorts to be processed in the
 * other short methods.  This is the short without the leading
 * 'the', 'a' whatever...
 * @return the array of shorts
 */
string *query_short_array() { return map( _cur_desc, (: _shrt[$1] :) ); }

/** @ignore yes */
string short() {
    return query_multiple_short(query_short_array());
} /* short() */

/** @ignore yes */
string pretty_short() {
    string *ret;

    ret = query_short_array();

    return ( sizeof(ret) ? query_multiple_short(ret) : "something");

} /* pretty_short() */

/** @ignore yes */
string a_short() {
    return implode( map( query_short_array(),
           (: "$mirror_short:"+add_a($1)+"$" :) ), "");
} /* a_short() */

/** @ignore yes */
string the_short() {
    return implode( map( query_short_array(),
           (: "$mirror_short:the "+$1+"$" :) ), "");
} /* the_short() */

/** @ignore yes */
string one_short() {
    return implode( map( query_short_array(),
           (: "$mirror_short:the "+$1+"$" :) ), "");
} /* one_short() */

/** @ignore yes */
string poss_short() {
    return implode( map( query_short_array(),
           (: "$mirror_short:the "+$1+"$" :) ), "");
} /* poss_short() */

/**
 * @ignore yes
 * This is used to make the reads of the items not fail.
 */
string query_read_short() {
    return "$name$";
} /* query_read_short() */

/** @ignore yes */
string query_plural() {
    return query_multiple_short( map( _cur_desc,
           (: pluralize( _shrt[$1] ) :) ) );
} /* query_plural() */

/** @ignore yes */
string pretty_plural() {
    int i;

    foreach( i in _cur_desc ) {
        tell_creator("sandoz", "%s called pretty_plural() on %s, "
            "returning %s.", file_name(PO), file_name(TO),
            pluralize(_shrt[i]) );
        return pluralize(_shrt[i]);
    }

    return 0;

} /* query_plural() */

/** @ignore yes */
string long( string name, int dark ) {
    int i;
    string *ret;

    ret = ({ });
    foreach( i in _cur_desc ) {
        if( !_lng[ i ] )
            continue;
        if( functionp(_lng[ i ]) )
            ret += ({ evaluate(_lng[ i ]) });
        else
            ret += ({ _lng[ i ] });
    }

    if( !sizeof( ret ) )
        return "There doesn't appear to be a description for "+
            name+", please contact a creator immediately.\n";

    return implode( evaluate(ret), "\n")+"\n";

} /* long() */

/**
 * This returns the currently matching indexes for the item object.
 * @return the array of currently matchig indexes
 */
int *query_cur_desc() { return _cur_desc; }

/**
 * This method returns the verbs mapping used in the item object.
 * @return the verbs in the item object
 */
mapping query_verbs() { return _verb; }

/**
 * This method returns the plurals mapping used in the item object.
 * @return the plurals in the item object
 */
mapping query_plurals() { return _plural; }

/**
 * This method returns all the long descriptions for the item object.  The
 * positions in the array correspond to the index used to reference the
 * items.
 * @return the array of long descriptions
 */
string *query_lng() { return _lng; }
/**
 * This method returns all the short descriptions for the item object.  The
 * positions in the array correspond to the index used to reference the
 * items.
 * @return the array of long descriptions
 */
string *query_shrt() { return _shrt; }

/**
 * This method figures out what gatherables are available for the
 * current description set.  The array contains the values passed
 * into the add_item stuff with the "gather" index.
 * @return the current gatherables
 */
mixed query_gather() {
    mixed g;
    int i;

    g = ({ });

    foreach( i in _cur_desc )
        if( _gather[i])
            g += ({ _gather[i] });

    return g;

} /* query_gather() */

/** @ignore yes */
int query_item(string str) { return _verb[str]; }

/** @ignore yes */
int *query_day_items() { return _day; }

/** @ignore yes */
int *query_night_items() { return _night; }

/**
 * This method setups all the internal stuff, including the long
 * description.
 * @param bits the bits to look through
 * @param index the index which refers to the item in question
 */
private void setup_bits_for_item( int index, mixed bits ) {
    int i, j, k;
    string str, tmp;
    mixed pat;

    if( !pointerp(bits) ) {
        _lng[index] = bits;
        return ;
    }

    _lng[index] = "You see nothing special.";
    j = sizeof(bits);

    for( i = 0; i < j; i += 2 ) {
        k = sizeof( bits[i+1] );
        /*
         * Figure out what the pattern should be.  We should have a pattern
         * specified if the second element of the array is a function
         * pointer, or the array is three long.
         */
        if( pointerp(bits[i+1]) && ( k == 3 ||
            ( k == 2 && functionp(bits[i+1][0]) ) ) ) {
            pat = bits[i+1][<1];
            if( !pointerp(pat) )
                pat = ({ pat });
        } else {
            pat = ({"<direct:object>"});
        }

        /* Ok.   Now check the other bits. */
        if( !pointerp(bits[i]) )
            bits[i] = ({ bits[i] });

        /* Go through and add all the relevant data for the bit in. */
        foreach( str in bits[i] ) {
            switch( str ) {
                case "long" :
                    _lng[index] = bits[i+1];
                    continue;
                case "gather" :
                    _gather[index] = bits[i+1];
                    continue;
                case "position" :
                case "position multiple" :
                    if( !_pos_stuff )
                        _pos_stuff = ([ ]);
                    if( !_pos_stuff[index] )
                        _pos_stuff[index] = allocate(POSITION_SIZE);
                    _pos_stuff[index][ ( str == "position" ?
                        POSITION_DESC : POSITION_MULT ) ] = bits[i+1];
                    continue;
                default :
                    // We've new command stuff.
                    if( find_call_out("do_init") == -1 )
                        call_out("do_init", 1 );
                    if( !_other[str] ) {
                        _pattern[str] = pat;
                        _other[str] = ([ index : bits[i+1] ]);
                        continue;
                    }
                    foreach( tmp in pat )
                        if( member_array( tmp, _pattern[str] ) == -1 )
                            _pattern[str] += ({ tmp });
                    _other[str][index] = bits[i+1];
            }
        }
    }

} /* setup_bits_for_item() */

/**
* This is a helper function for the delete function, it removes everything
* not in the standard static arrays.
* @param index the index to remove
*/
private void remove_bits_for_item( int index ) {
    string str;
    mixed value;
    int frog, bing, i;

    /* Fix up the gather mapping. */
    map_delete( _gather, index );
    foreach( frog, bing in _gather ) {
        if( frog > index ) {
            map_delete( _gather, frog );
            _gather[frog-1] = bing;
        }
    }

    /* Fix up the position mapping. */
    if( _pos_stuff ) {
        map_delete( _pos_stuff, index );
        foreach( frog, bing in _pos_stuff ) {
            if( frog > index ) {
                map_delete( _pos_stuff, frog );
                _pos_stuff[frog-1] = bing;
            }
        }
    }

    /* Fix up the parse_command mapping. */
    foreach( str, value in _other ) {
        if( value ) {
            map_delete( value, index );
            if( !sizeof(value) ) {
                map_delete( _other, str );
                map_delete( _pattern, str );
            } else {
                foreach( frog, bing in value ) {
                    if( frog > index ) {
                        map_delete( value, frog );
                        value[frog-1] = bing;
                    }
                }
            }
            if( find_call_out("do_init") == -1 )
                call_out("do_init", 1 );
        }
    }

    /* Fix up the verbs array. */
    foreach( str, value in _verb ) {
        for( i = 0; i < sizeof(value); i += 2 ) {
            if( value[i+1] > index ) {
                value[i+1]--;
            } else if( value[i+1] == index ) {
                value = value[0..i-1] + value[i+2..];
                i -= 2;
            }
        }
        if( !sizeof(value) )
            map_delete( _verb, str );
        else
            _verb[str] = value;
    }

    /* Fix up the plural array. */
    foreach( str, value in _plural ) {
        for( i = 0; i < sizeof(value); i += 2 ) {
            if( value[i+1] > index ) {
                value[i+1]--;
            } else if( value[i+1] == index ) {
                value = value[0..i-1] + value[i+2..];
                i -= 2;
            }
        }
        if( !sizeof(value) )
            map_delete( _plural, str );
        else
            _plural[str] = value;
    }

    // Update day items.
    i = sizeof(_day);
    while( i-- )
        if( _day[i] > index )
            _day[i]--;

    // Update night items.
    i = sizeof(_night);
    while( i-- )
        if( _night[i] > index )
            _night[i]--;

} /* remove_bits_for_item() */

/**
 * This method sets up all the parse command handling stuff that is needed
 * for the given name.
 * @param index the index to associate it with
 * @param name the name to add in
 * @param no_plural do not add in any plurals
 */
private void add_name_reference( int index, string name, int no_plural ) {
    string *bits, plural, s;

    bits = explode( name, " " );
    s = bits[<1];

    if( !_verb[s] )
        _verb[s] = ({ bits[0..<2], index });
    else
        _verb[s] += ({ bits[0..<2], index });

    if( !no_plural ) {
        plural = pluralize(s);
        if( !_plural[plural] )
            _plural[plural] = ({ bits[0..<2], index });
        else
            _plural[plural] += ({ bits[0..<2], index });
    }

} /* add_name_reference() */

/**
 * This method setups the item initially.   This is called by the
 * add_item method in the room itself.
 * @param nam the name(s) to add the item for
 * @param long the long description of the item
 * @param no_plural do not add a plural flag
 * @return the index associated with the item
 */
int setup_item( mixed nam, mixed long, int no_plural, int time ) {
    int index;

    index = sizeof(_lng);

    if( pointerp(nam) ) {
        if( sizeof(nam) > 0 )
            _shrt += ({ nam[0] });
        map( nam, (: add_name_reference( $2, $1, $3 ) :), index, no_plural );
    } else {
        _shrt += ({ nam });
        add_name_reference( index, nam, no_plural );
    }

    _lng += ({ 0 });
    setup_bits_for_item( index, long );

    switch( time ) {
      case ITEM_DAY :
        _day += ({ index });
      break;
      case ITEM_NIGHT :
        _night += ({ index });
      break;
      default:
    }

    return index;

} /* setup_item() */

/**
 * This method non-destructively modifies the items values.  It will not
 * remove the value for this item, remember this!  If you add again
 * and again you will end up with multiple of the same object.
 * @see /std/room->modify_item_by_name()
 * @param str the name to reference the object by
 * @param long the long bits to change on the item
 * @param time the item (day/night/common) to modify
 * @return 1 on success, 0 on failure
 */
int modify_item_by_index( int index, mixed long, int time ) {
    if( index >= sizeof(_lng) )
        return 0;

    switch( time ) {
      case ITEM_DAY :
        if( !query_day_item(index) )
            return 0;
      break;
      case ITEM_NIGHT :
        if( !query_night_item(index) )
            return 0;
      break;
      default:
        if( !query_common_item(index) )
            return 0;
    }

    setup_bits_for_item( index, long );
    return 1;

} /* modify_item_by_index() */

/**
 * This method non-destructively modifies the items values.  It will not
 * remove the value for this item, remember this!  If you add again
 * and again you will end up with multiple of the same object.
 * @see /std/room->modify_item_by_name()
 * @param str the name to reference the object by
 * @param long the long bits to change on the item
 * @param time the item (day/night/common) to modify
 * @return 1 on success, 0 on failure
 */
int modify_item_by_name( string str, mixed long, int time ) {
    int index;

    if( ( index = member_array( str, _shrt ) ) == -1 )
        return 0;

    return modify_item_by_index( index, long, time );

} /* modify_item_by_name() */

/** @ignore yes */
int modify_item( mixed str, mixed long, int time ) {
    if( stringp(str) )
        return modify_item_by_name( str, long, time );

    if( intp(str) )
        return modify_item_by_index( str, long, time );

    return 0;

} /* modify_item() */

/**
 * This method removes an item by the returned index number from the
 * add_item method.
 * @param index the index number to remove
 * @param time the item (day/night/common) to remove
 * @return 1 on success, 0 on failure
 * @example
 * int _item_number;
 *
 * void bing {
 *    _item_numer = add_item("frog", "It wombles!");
 * } /\* bing() *\/
 *
 * void remove_bing() {
 *    remove_item(_item_number);
 * } /\* remove_bing() *\/
 */
int remove_item_by_index( int index, int time ) {
    if( index >= sizeof(_lng) )
        return 0;

    switch( time ) {
      case ITEM_DAY :
        if( !query_day_item(index) )
            return 0;
        _day -= ({ index });
      break;
      case ITEM_NIGHT :
        if( !query_night_item(index) )
            return 0;
        _night -= ({ index });
      break;
      default:
        if( !query_common_item(index) )
            return 0;
    }

    _shrt = _shrt[0..index-1] + _shrt[index+1..];
    _lng = _lng[0..index-1] + _lng[index+1..];

    remove_bits_for_item( index );
    return 1;

} /* remove_item_by_index() */

/**
 * This method removes an item by the short description.
 * @param index the index number to remove
 * @param time the item (day/night/common) to remove
 * @return 1 on success, 0 on failure
 */
int remove_item_by_name( string str, int time ) {
    int index, last = 0, sz = sizeof( _shrt );

    while( last < sz && ( index = member_array( str, _shrt, last ) ) != -1 ) {
        if( remove_item_by_index( index, time ) )
            return 1;
        last = index + 1;
    }

    return 0;

} /* remove_item_by_name() */

/** @ignore yes */
int remove_item( mixed str, int time ) {
    if( stringp(str) )
        return remove_item_by_name( str, time );

    if( intp(str) )
        return remove_item_by_index( str, time );

    return 0;

} /* remove_item() */

/** @ignore yes */
string *parse_command_id_list() {
    tell_creator("sandoz", "%s called parse_command_id_list() on %s.",
        file_name(PO), file_name(TO) );
    return keys(_verb);
} /* parse_command_id_list() */

/** @ignore yes */
string *parse_command_plural_id_list() {
    string *_plu, word;
    mixed words;
    int i;

    tell_creator("sandoz", "%s called parse_command_plural_id_list() on %s.",
        file_name(PO), file_name(TO) );

    _plu = ({ });

    foreach( word, words in _plural ) {
        _plu += ({ word });
        for( i = 0; i < sizeof(words); i += 2 ) {
            if( sizeof(words[i]) )
                _plu += ({ implode( words[i], " " )+" "+word });
        }
    }

    return _plu;

} /* parse_command_plural_id_list() */

/** @ignore yes */
string *parse_command_adjective_id_list() {
    string *_adjs, word;
    mixed words;
    int i, j;

    tell_creator("sandoz", "%s called parse_command_adjective_id_list() "
        "on %s.", file_name(PO), file_name(TO) );

    _adjs = ({ });

    foreach( word, words in _verb ) {
        j = sizeof(words);
        for( i = 0; i < j; i += 2 )
            if( sizeof(words[i]) )
                _adjs += words[i];
    }

    return uniq_array( _adjs );

} /* parse_command_adjective_id_list() */

/** @ignore yes */
object query_parse_id( mixed arr ) {
    string *bits;
    mixed stuff;
    int i, j, match, all_match;

    bits = explode(arr[P_STR], " ");
    bits -= ({"a", "an", "the"});

    tell_creator("sandoz", "%s called query_parse_id() on %s\n"
        "Bits : %O", file_name(PO), file_name(TO), bits );

    /* all case */
    if( arr[P_THING] == 0 ) {
        stuff = _plural[bits[<1]];
        if( !stuff ) {
            stuff = _verb[bits[<1]];
            if( !stuff )
                return 0;
        }
        _cur_desc = ({ });
        all_match = 0;
        for( j = 0; j < sizeof(stuff); j += 2 ) {
            match = 1;
            for( i = 0; i < sizeof(bits) - 1; i++ ) {
                if( member_array( bits[i], stuff[j] ) == -1 ) {
                    match = 0;
                    break;
                }
            }
            if( match && query_item_available(stuff[j+1]) ) {
                all_match = 1;
                if( member_array( stuff[j+1], _cur_desc ) == -1 )
                    _cur_desc += ({ stuff[j+1] });
            }
        }
        if( all_match )
            return TO;
        else
            return 0;
    }

    /* Specific object case. */
    if( arr[P_THING] < 0 ) {
        stuff = _verb[bits[<1]];
        if( !stuff )
            return 0;
        for( j = 0; j < sizeof(stuff); j += 2 ) {
            match = 1;
            for( i = 0; i < sizeof(bits) - 1; i++ ) {
                if( member_array( bits[i], stuff[j] ) == -1 ) {
                    match = 0;
                    break;
                }
            }

            if( ++arr[P_THING] != 0 )
                continue;

            /* Get the current thingy out of the list. */
            if( match && query_item_available(stuff[j+1]) ) {
                _cur_desc = ({ stuff[j+1] });
                arr[P_THING] = -10321;
                return TO;
            }
        }
        return 0;
    }

    /* Lots of objects case.   The objects are specified though. */
    stuff = _plural[bits[<1]];
    if( !stuff ) {
        stuff = _verb[bits[<1]];
        if( !stuff )
            return 0;
    }

    _cur_desc = ({ });
    for( j = 0; j < sizeof(stuff); j += 2 ) {
        match = 1;
        for( i = 0; i < sizeof(bits)-1; i++ ) {
            if( member_array( bits[i], stuff[j] ) == -1 ) {
                match = 0;
                break;
            }
        }
        if( match && query_item_available(stuff[j+1]) ) {
            if( member_array( stuff[j+1], _cur_desc ) == -1 )
                _cur_desc += ({ stuff[j+1] });
            arr[P_THING]--;
            if( arr[P_THING] <= 0 ) {
                arr[P_THING] = -10786;
                return TO;
            }
        }
    }

    return 0;

} /* query_parse_id() */

/** @ignore yes */
mixed parse_match_object( string *input, object player,
                          class obj_match_context context ) {
    int *stuff, i, j, match, ret, pl, sn, inp;
    mixed plural, sing;

    inp = sizeof(input);

    if( input[<1] == "here" && inp > 1 )
        input = input[0..<2];

    _cur_desc = filter( _cur_desc, (: query_item_available($1) :) );

    /* context */
    if( TO == context->it && inp == 1 && input[0] == "it") {
        sing = map( _cur_desc, (: explode(_shrt[$1], " ")[<1] :) ) +
            ({ _cur_desc[0] });
        plural = ({ });
    } else if( input[<1] == "them" && context->plural &&
        member_array( TO, context->plural ) != -1 ) {
        plural = map( _cur_desc, (: explode(_shrt[$1], " ")[<1] :) ) +
            ({ _cur_desc[0] });
        sing = ({ });
    } else {
        plural = _plural[input[<1]];
        sing = _verb[input[<1]];
    }

    pl = sizeof(plural);
    sn = sizeof(sing);

    if( !pl && !sn )
        return 0;

    stuff = ({ });

    // Check singular matches.
    if( sn > 1 ) {
        for( i = 0; i < sn; i += 2 ) {
            match = 1;
            for( j = 0; j < inp - 1; j++ ) {
                if( member_array( input[j], sing[i] ) == -1 ) {
                    match = 0;
                    break;
                }
            }
            if( match ) {
                if( member_array( sing[i+1], stuff ) == -1 &&
                    query_item_available(sing[i+1]) ) {
                    stuff += ({ sing[i+1] });
                    ret |= OBJ_PARSER_MATCH_SINGULAR;
                }
            }
        }
    }

    // Check plural matches.
    for( i = 0; i < pl; i += 2 ) {
        match = 1;
        for( j = 0; j < inp - 1; j++ ) {
            if( member_array( input[j], plural[i] ) == -1 ) {
                match = 0;
                break;
            }
        }
        if( match ) {
            if( member_array( plural[i+1], stuff ) == -1 &&
                query_item_available(plural[i+1]) ) {
                stuff += ({ plural[i+1] });
                ret |= OBJ_PARSER_MATCH_PLURAL;
            }
        }
    }

    // We matched, so see if we should have.
    if( inp = sizeof(stuff) ) {
        if( context->ordinal ) {
            if( context->ordinal > inp ) {
                context->ordinal -= inp;
                return 0;
            }
            context->ignore_rest = 1;
            context->ordinal--;
            _cur_desc = stuff[context->ordinal..context->ordinal];
        } else if( context->number_included ) {
            _cur_desc = stuff[0..context->number_included];
            context->number_included -= inp;
            if( context->number_included <= 0 )
                context->ignore_rest = 1;
        } else if( ret & OBJ_PARSER_MATCH_PLURAL ) {
            _cur_desc = stuff;
        } else {
            _cur_desc = stuff[0..0];
        }
        return ({ ret, ({ TO }) });
    }

    return 0;

} /* parse_match_object() */

/** @ignore yes */
int command_control( string command, object *indir, string arg1,
                     string arg2, string *args, string pattern ) {
    int desc, num;
    mixed tmp;
    int i;

    if( !_other[command] )
        return 0;

    foreach( desc in _cur_desc ) {
        i = sizeof( tmp = _other[command][desc] );

        if( pointerp(tmp) && pattern == tmp[<1] ) {
            if( i == 3 || ( i == 2 && !functionp(tmp[0]) ) ) {
                // Took out 'command' as the first argument for add_command
                // compliance, you can always use query_verb() - Sandoz.
                num += call_other( tmp[0], tmp[1], indir, arg1, arg2,
                    args, pattern );
            } else if( i == 2 && functionp(tmp[0]) ) {
                // See above.
                num += evaluate( tmp[0], indir, arg1, arg2, args, pattern );
            }
        } else if( pattern == "<direct:object>") {
            if( functionp(tmp) ) {
                // See above.
                num += evaluate( tmp, indir, arg1, arg2, args, pattern );
            } else if( stringp(tmp) ) {
                add_succeeded_mess( ({ tmp, ""}) );
                num++;
            } else if( pointerp(tmp) && i == 2 && stringp(tmp[0]) &&
                stringp(tmp[1]) ) {
                add_succeeded_mess( tmp );
                num++;
            }
        }
    }

    return num;

} /* command_control() */

/** @ignore yes */
mapping query_other_things() { return _other; }

/** @ignore yes */
mapping query_pattern() { return _pattern; }

/** @ignore yes */
string query_position_string( string pos ) {
    int i;

    if( !_pos_stuff )
        return 0;

    foreach( i in _cur_desc )
        if( _pos_stuff[i] )
            return _pos_stuff[i][POSITION_DESC];

    return 0;

} /* query_position_string() */

/** @ignore yes */
int query_position_multiple( string pos ) {
    int i;

    if( !_pos_stuff )
        return 0;

    foreach( i in _cur_desc )
        if( _pos_stuff[i])
            return _pos_stuff[i][POSITION_MULT];

    return 0;

} /* query_position_multiple() */

/** @ignore yes */
mapping query_position_stuff() { return _pos_stuff; }

/** @ignore yes */
int drop() { return 1; }
/** @ignore yes */
int get() { return 1; }

/** @ignore yes */
void dest_me() { destruct(TO); }

/** @ignore yes */
void dwep() { destruct(TO); }

/** @ignore yes */
int move() { return 1; }
