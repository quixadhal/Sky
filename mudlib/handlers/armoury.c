/**
 * Keeps track of all the standard armours, weapons and clothing.
 * @index armour
 * @index weapon
 * @index scabard
 * @index clothing
 * @index clothes
 * @index jewellery
 * @index stationery
 * @index instruments
 * @author Furball
 * @changed Many many, by many people
 * @changed 4/1/97 Ceres
 * The discworld armoury was rewritten by Ceres on 4/1/97 to handle
 * subdirectories within the standard dirs.
 * @changed 20/06/01 Shaydz
 * Rewritten to remove a few annoying bugs, including not being able to
 * request items that had more than one word.
 * @changed 26/06/02 Sandoz
 * Removed all the recycling code, because it will never be reliable,
 * because of properties, effects and whatnot.
 * @changed 06/09/02 Sandoz
 * Added stationery and instrument lists, and new recycling code which
 * will hopefully work ( gotta love reload_object() :) ).
 * @changed 26/09/02 Sandoz
 * Changed to only recycle items that have been requested before with
 * request_item(), and to only accept an item for recycling if its
 * miss count is higher than the number of currently recycled items.
 */

#define CONTAINER    "/std/objects/armoury_container"

#define ITEM_LOG

#ifdef ITEM_LOG
#define SAVE_FILE    HANDLER_SAVE_DIR "/armoury_log.o"
#endif

// The maximum number of items of the same time we recycle.
#define MAX_RECYCLE  10

#define O_WEAPON     1
#define O_ARMOUR     2
#define O_CLOTHING   4
#define O_SCABBARD   8
#define O_JEWELLERY  16
#define O_STATIONERY 32
#define O_INSTRUMENT 64
#define O_ACCEPT     128
#define O_ORES       256

inherit ROOM_OBJ;

/**
 * This class stores all the data for a specific armoury item.
 * @member file the file name of the item
 * @member recycled the recycled objects
 * @member hits recycle hits
 * @member misses recycle misses
 */
class item_data {
    string file;
    object *recycled;
    int hits;
    int misses;
    int flags;
}

#ifdef ITEM_LOG
private int call_id;
#endif

// This mapping contains all our items, for faster access.
private nosave mapping item_list;

// This stores our container object, where we keep all out recycled stuff.
private object cont;

// This stores all the items that can be searched up from rooms.
private object *guttered;

#ifdef ITEM_LOG
private void do_logging();
#endif

void rehash();

/** @ignore yes */
int query_keep_room_loaded() { return 1; }

protected void setup() {
    set_short("Armoury" );
    add_property("determinate", "the ");
    set_light(50);

    set_long("You are in the Armoury.  Here you can list all of the "
        "\"weapons\", \"armour\", \"clothing\", \"jewellery\", "
        "\"scabbards\", \"stationery\" items, \"ores\" and "
        "musical \"instruments\" "
        "that are available, or \"request\" one of them.  The armoury "
        "stores its list of items a mapping, which can be updated with "
        "\"rehash\".\n");

    rehash();

    guttered = ({ });

#ifdef ITEM_LOG
    call_id = call_out( (: do_logging :), 1200 );
    if( file_exists(SAVE_FILE) )
        unguarded( (: rm, SAVE_FILE :) );
#endif

} /* setup() */

/** @ignore yes */
private string convert_fname( string file ) {
    return replace_string( explode( file, ".")[0], "_", " ");
} /* convert_fname() */

/**
 * This method recycles a suitable object.
 * It will call reload_object() on it, and make it available for
 * requesting from the armoury.
 * @param ob the object to try to recycle
 * @return 1 if recycled successfully, 0 if not
 */
int recycle_object( object ob ) {
    class item_data data;
    string base, name;
    int i;

    name = convert_fname( explode( base = base_name(ob), "/")[<1] );

    if( classp( data = item_list[name] ) && ( data->flags & O_ACCEPT ) &&
        data->file == base && ( i = sizeof(data->recycled) ) < data->misses &&
        i < MAX_RECYCLE ) {
        object *inv;

        if( sizeof( inv = INV(ob) ) ) {
            inv->move("/room/rubbish");
            if( sizeof( INV(ob) ) )
                return 0;
        }

        if( !cont && !( cont = clone_object(CONTAINER) ) || ob->move(cont) )
            return 0;

        reload_object(ob);
        data->recycled += ({ ob });

        if( ob->query_weight() < roll_MdN( 4, 20 ) &&
            !sizeof( ob->query_value_info() ) )
            guttered += ({ ob });

        return 1;

    }

    return 0;

} /* recycle_object() */

/**
 * General item retreival function.  It returns a new item
 * it handles any sort of item.  The percentage is the
 * percentage condition in which it is returned.
 * The "word" parameter is the name of the requested item, essentially
 * the filename without directory and with "_"'s changed to " "'s. The
 * "percent" parameter is used as measure of how good it
 * is compared to a fully functional one. So a 50% dagger is only 50% as
 * good as a 100% one.
 * <p>
 * The file armoury.h defines the ARMOURY variable.  You need to include
 * this into your file to use it.  You should include this file and
 * uyse ythis define rather than the full path to the armoury, as
 * otherwise your code could stuff up :)
 * @param word the name of the item to retreive
 * @param percent the percentage condition to be in
 * @see /include/armoury.h
 * @return the requested object, or 0 on failure
 * @example
 * // get a slightly worn long sword and give it to the npc.
 * ARMOURY->request_item("long sword", 85)->move(npc);
 * @example
 * // Get a random condition armour.
 * armour = ARMOURY->request_item("leather jacket", random(100));
 */
object request_item( string word, int percent ) {
    class item_data data;
    object thing, *obs;
    int i;
    if( !word )
        error("No item name specified in request_item().\n");

    if( percent < 1 )
        error("Zero or negative item condition in request_item().\n");

    if( !classp( data = item_list[word] ) )
        return 0;

    if( cont && ( i = sizeof( obs = data->recycled ) ) ) {
        while( i-- ) {
            if( ( thing = obs[i] ) && ENV(thing) == cont ) {
                data->recycled = obs[0..i-1];
                data->hits++;
                thing->set_percentage( percent );
                guttered -= ({ 0, thing });

                return thing;
            }
        }
        // We found nothing of use.
        data->recycled = ({ });
    }

    if( !thing = clone_object( data->file ) ) {
        printf("The armoury failed to clone %O.\n", word );
        log_file("ARMOURY", "%s - failed to clone %O (%s).\n",
            ctime(time()), word, file_name(PO) );

        return 0;
    }

    data->misses++;

    if( !( data->flags & O_ACCEPT ) &&
        !thing->query_property(NO_RECYCLE) )
        data->flags |= O_ACCEPT;

    thing->set_percentage( percent );

    return thing;

} /* request_item() */

/** @ignore yes */
void dest_me() {
    if( cont )
        cont->dest_me();
    ::dest_me();
} /* dest_me() */

/**
 * Returns the current list of all items.
 * @return the items mapping
 */
mapping query_item_list() { return copy(item_list); }

/**
 * This method returns the recycle hits count for a specific file.
 * @param word the name of the object to get the hits count for
 * @return the hits count
 */
int query_hits( string word ) {
    if( item_list[ word ] ||
        item_list[ word = convert_fname( explode( word, "/")[<1] ) ] )
        return item_list[word]->hits;
    return 0;
} /* query_hits() */

/**
 * This method returns the recycle miss count for a specific file.
 * @param word the name of the object to get the misses count for
 * @return the misses count
 */
int query_misses( string word ) {
    if( item_list[ word ] ||
        item_list[ word = convert_fname( explode( word, "/")[<1] ) ] )
        return item_list[word]->misses;
    return 0;
} /* query_misses() */

/**
 * This method returns the recycled items for a specific file.
 * @param word the name of the object to get the recycled objects for
 * @return the recycled objects
 */
object *query_recycled( string word ) {
    if( word && ( item_list[ word ] ||
        item_list[ word = convert_fname( explode( word, "/")[<1] ) ] ) )
        return item_list[word]->recycled;
    return 0;
} /* query_recycled() */

/**
 * This method returns whether or not the armoury is currently accepting
 * the specified item for recycling.
 * @param word the name of the object to check
 * @return 1 if we are currently accepting the item, 0 if not
 */
int query_accepting( string word ) {
    if( item_list[ word ] ||
        item_list[ word = convert_fname( explode( word, "/")[<1] ) ] )
        return ( item_list[word]->flags & O_ACCEPT ) > 0;
    return 0;
} /* query_accepting() */

/**
 * This method returns the absolute filename of an item.
 * @param name the name of the item
 * @return the absolute filename of the item
 */
string query_item_filename( string item ) {
    class item_data data;

    if( classp( data = item_list[item] ) )
        return data->file;

    return 0;

} /* query_item_filename() */

/** @ignore yes */
string *item_list_filter( int type ) {
    return keys( filter( item_list, (: $2->flags & $3 :), type ) );
} /* item_list_filter() */

/**
 * Returns the current list of known weapons.
 * @return the weapons array
 */
string *query_weapons() { return item_list_filter( O_WEAPON ); }

/**
 * Returns the current list of known armours.
 * @return the armours array
 */
string *query_armour() { return item_list_filter( O_ARMOUR ); }

/**
 * Returns the current list of known clothing.
 * @return the clothing array
 */
string *query_clothing() { return item_list_filter( O_CLOTHING ); }

/**
 * Returns the current list of known jewelley.
 * @return the jewellery array
 */
string *query_jewellery() { return item_list_filter( O_JEWELLERY ); }

/**
 * Returns the current list of known scabbards.
 * @return the scabbards array
 */
string *query_scabbards() { return item_list_filter( O_SCABBARD ); }

/**
 * Returns the current list of known stationery items.
 * @return the stationery items array
 */
string *query_stationery() { return item_list_filter( O_STATIONERY ); }

/**
 * Returns the current list of known musical instruments.
 * @return the musical instruments array
 */
string *query_instruments() { return item_list_filter( O_INSTRUMENT ); }

/**
 * Returns the current list of known ores.
 * @return the ores instruments array
 */
string *query_ores() { return item_list_filter( O_ORES ); }

/** @ignore yes */
private void make_list( string base_dir, int type ) {
    string *dirs, dir, fname, tmp;
    mixed file;

    dirs = ({ });

    foreach( file in get_dir( base_dir, -1 ) )
        if( file[1] == -2 )
            dirs += ({ base_dir+file[0]+"/" });

    dirs += ({ base_dir });

    foreach( dir in dirs ) {
        foreach( fname in get_dir( dir+"*.c") ) {
            if( undefinedp( item_list[ tmp = convert_fname(fname) ] ) ) {
                item_list[tmp] = new( class item_data );
                item_list[tmp]->file = dir+fname[0..<3];
                item_list[tmp]->recycled = ({ });
                item_list[tmp]->flags = type;
            } else {
                printf("Failed to add %s (%s) - %s already exists in the "
                    "mapping.\n", tmp, dir+fname[0..<3],
                    item_list[tmp]->file );
                log_file("ARMOURY", "Failed to add %s (%s) - %s already "
                    "exists in the mapping.\n", tmp, dir+fname[0..<3],
                    item_list[tmp]->file );
            }
        }
    }

} /* make_list() */

/** @ignore yes */
int clean_up( int parent ) { return 0; }

/**
 * This method returns the list of items that could be
 * searched up from gutters.
 * @return the guttered items list
 */
object *query_guttered() {
    if( cont )
        guttered = filter( guttered, (: $1 && ENV($1) == $2 :), cont );
    else
        guttered = ({ });

    return guttered;

} /* query_guttered() */

/**
 * This method gets a guttered item from the armoury.
 * This is used by the search function in city rooms,
 * and returns a random smaller item from the armoury's
 * recycled items list.  The condition percentage should
 * not be set too high for guttered items, and has been
 * capped at 50% for that very reason.
 * @param percent the percentage condition the item should be in
 * @return a guttered object or 0 if there are none
 */
object get_guttered( int percent ) {
    int i;

    if( percent < 1 )
        error("Zero or negative item condition in get_guttered().\n");

    if( percent > 10 )
        percent = 10;

    if( cont && ( i = sizeof( query_guttered() ) ) ) {
        object ob;

        ob = guttered[ i = random( i ) ];
        ob->set_percentage( percent );
        guttered = delete( guttered, i, 1 );
        return ob;
    }

    return 0;

} /* get_guttered() */

/** @ignore yes */
void rehash() {
    mapping tmp;
    string file;
    class item_data data;

    if( sizeof(item_list) )
        tmp = item_list;

    item_list = ([ ]);

    //make_list("/obj/dwweapons/", O_WEAPON );
    make_list("/obj/weapons/", O_WEAPON );

    //make_list("/obj/dwarmours/", O_ARMOUR );
    make_list("/obj/armours/", O_ARMOUR );

    //make_list("/obj/dwclothes/", O_CLOTHING );
    make_list("/obj/clothes/", O_CLOTHING );

    //make_list("/obj/dwjewellery/", O_JEWELLERY );
    make_list("/obj/jewellery/", O_JEWELLERY );

    //make_list("/obj/dwscabbards/", O_SCABBARD );
    make_list("/obj/scabbards/", O_SCABBARD );

    make_list("/obj/stationery/", O_STATIONERY );

    make_list("/obj/instruments/", O_INSTRUMENT );
    
    make_list("/obj/ores/", O_ORES);

    if( sizeof(tmp) ) {
        string *keys;

        // Transfer the accept flag, recycled objects and hits/misses
        // from the old mapping.
        foreach( file, data in item_list ) {
            if( classp( tmp[file] ) ) {
                data->hits = tmp[file]->hits;
                data->misses = tmp[file]->misses;

                if( data->file == tmp[file]->file )
                    data->recycled = tmp[file]->recycled - ({ 0 });
                else if( cont && sizeof( tmp[file]->recycled -= ({ 0 }) ) )
                    map( filter( tmp[file]->recycled, (: ENV($1) == $2 :),
                        cont ), (: $1->dest_me() :) );

                if( tmp[file]->flags & O_ACCEPT )
                    data->flags |= O_ACCEPT;
            }
        }

        // Clean up recycled items that are no longer needed.
        if( cont && sizeof( keys = keys(tmp) - keys(item_list) ) )
            foreach( file in keys )
                if( sizeof( tmp[file]->recycled -= ({ 0 }) ) )
                    map( filter( tmp[file]->recycled, (: ENV($1) == $2 :),
                        cont ), (: $1->dest_me() :) );
    }

} /* rehash() */

/** @ignore yes */
private int print_list( string str, string *arr ) {
    printf( str, TP->query_cols(), implode( sort_array( arr, 1 ), "\n") );
    return 1;
} /* print_list() */

/** @ignore yes */
int weapons() {
    return print_list("Weapons:\n%-*#s\n", query_weapons() );
} /* weapons() */

/** @ignore yes */
int armour() {
    return print_list("Armour:\n%-*#s\n", query_armour() );
} /* armour() */

/** @ignore yes */
int clothing() {
    return print_list("Clothes:\n%-*#s\n", query_clothing() );
} /* clothing() */

/** @ignore yes */
int jewellery() {
    return print_list("Jewellery:\n%-*#s\n", query_jewellery() );
} /* jewellery() */

/** @ignore yes */
int scabbards() {
    return print_list("Scabbards:\n%-*#s\n", query_scabbards() );
} /* scabbards() */

/** @ignore yes */
int stationery() {
    return print_list("Stationery items:\n%-*#s\n", query_stationery() );
} /* scabbards() */

/** @ignore yes */
int instruments() {
    return print_list("Musical instruments:\n%-*#s\n", query_instruments() );
} /* scabbards() */

/** @ignore yes */
int ores() {
    return print_list("Ores:\n%-*#s\n", query_ores() );
} /* ores() */

/** @ignore yes */
int request( string word ) {
    int percent, i;
    object thing;
    string *words;

    if( !word || word == "")
        return notify_fail("Request what?\n");

    words = explode( word, " ");

    if( sizeof(words) < 2 )
        return notify_fail("Syntax: request <item name> <percent>\n");

    i = sizeof(words);

    if( !percent = to_int(words[i-1]) )
        return notify_fail("Syntax: request <item name> <percent>\n");

    words = words - ({ words[i-1] });
    word = implode( words, " ");
    thing = request_item( word, percent );

    if( !thing )
        return notify_fail( word+" not found.\n");

    if( thing->move(TP) ) {
        write( thing->the_short()+" has been placed in here.\n");
        thing->move(TO);
    } else {
        write( thing->the_short()+" has been placed in your inventory.\n");
    }

    return 1;

} /* request() */

/** @ignore yes */
int do_rehash() {
    rehash();
    write("Rehash completed.\n");
    return 1;
} /* do_rehash() */

/** @ignore yes */
void init() {
    ::init();

    add_command("rehash", "");

    add_command("request", "<string'item name> <percent'>",
        (: request($4[0]) :) );

    foreach( string comm in ({"weapons", "armour", "clothing", "jewellery",
        "scabbards", "stationery", "instruments", "ores"}) )
        add_command( comm, "", (: call_other( TO, $(comm) ) :) );

} /* init() */

#ifdef ITEM_LOG
/** @ignore yes */
private void do_logging() {
    mapping misses, unique, saved;
    string str;
    string missing = "", extra = "";
    int i, total_missing, total_extra, changed;

    if( find_call_out(call_id) == -1 )
        call_id = call_out( (: do_logging :), 3600 );

    misses = ([ ]);

    if( file_exists(SAVE_FILE) )
        saved = restore_variable( unguarded( (: read_file, SAVE_FILE :) ) );

    if( !mapp(saved) )
        saved = ([ ]);

    // First create a mapping of - ([ filename : misses ])
    foreach( str, class item_data data in item_list )
        misses[data->file] = data->misses;

    // Make a mapping of all cloned armoury objects based on their
    // base name - ([ basename : ({ objects }) ])
    unique = unique_mapping( objects( (: clonep($1) &&
        !undefinedp( $(misses)[base_name($1)] ) :) ),
        (: base_name($1) :) );

    // Now check how many extra objects we have (bad, because something
    // is cloning items, but not through the armoury), or how many we are
    // missing (bad, because something is destructing items instead of
    // sending them to the rubbish room).
    // This will break a little when someone with a lot of armoury items
    // logs on though... But... nothing helps against that.
    foreach( str in sort_array( keys(misses), 1 ) ) {
        if( i = misses[str] - sizeof( unique[str] ) ) {
            if( saved[str] != i ) {
                changed = 1;
                saved[str] = i;
            }

            if( i > 4 ) {
                missing += sprintf("%5d %s\n", i, str );
                total_missing += i;
                continue;
            }

            if( i < -4 ) {
                extra += sprintf("%5d %s\n", -i, str );
                total_extra -= i;
                continue;
            }
        }
    }

    if( changed ) {
        if( sizeof(saved) )
            unguarded( (: write_file, SAVE_FILE, save_variable(saved), 1 :) );

        if( missing != "")
            log_file("ARMOURY_RECYCLE_LOG", "%s - %i missing "
                "items:\n%s", ctime(time()), total_missing, missing );

        if( extra != "")
            log_file("ARMOURY_RECYCLE_LOG", "%s - %i extra "
                "items:\n%s", ctime(time()), total_extra, extra );
    }

} /* do_logging() */

/** @ignore yes */
void force_logging() {
    if( adminp( previous_object(-1) ) )
        do_logging();
} /* force_logging() */
#endif

/** @ignore yes */
mixed stats() {
    mixed arr;
    int misses, hits;
    class item_data data;

    foreach( data in values(item_list) ) {
        hits += data->hits;
        misses += data->misses;
    }

    arr = ({ ({"total hits", hits }), ({"total misses", misses }) });

    if( cont ) {
        arr += ({
            ({"container", cont }),
            ({"recycled", sizeof(INV(cont)) }),
        });
    }

    arr += ({
        ({"weapons", sizeof( query_weapons() ) }),
        ({"armour", sizeof( query_armour() ) }),
        ({"clothes", sizeof( query_clothing() ) }),
        ({"jewellery", sizeof( query_jewellery() ) }),
        ({"scabbards", sizeof( query_scabbards() ) }),
        ({"stationery", sizeof( query_stationery() ) }),
        ({"instruments", sizeof( query_instruments() ) }),
        ({"ores", sizeof( query_ores() ) }),
        ({"total items", sizeof( item_list) }),
        ({"guttered items", sizeof( query_guttered() ) }),
    });

#ifdef ITEM_LOG
    if( ( hits = find_call_out(call_id) ) != -1 )
        arr += ({ ({"next logging", hits }) });
#endif

    return arr;

} /* stats() */
