/**
 * Keeps track of all the PT armours, weapons and clothing.
 * Based on the regular armoury - Sandoz.
 */

inherit ROOM_OBJ;

#define QC_DIR      "/d/qc/standard/"
#define ARMOUR_D    QC_DIR "armour/"
#define WEAPON_D    QC_DIR "weapons/"
#define CLOTHING_D  QC_DIR "clothes/"
#define JEWELLERY_D QC_DIR "jewellery/"
#define SCABBARD_D  QC_DIR "scabbards/"
#define MISC_D      QC_DIR "misc/"

nosave mapping armour, weapon, clothing, jewellery, scabbard, misc;

int query_keep_room_loaded() { return 1; }

/** @ignore yes */
mapping make_list( string base ) {
    string *dirs, dir, file_name;
    mixed file;
    mapping list;

    dirs = ({ });

    foreach( file in unguarded( (: get_dir( $(base), -1 ) :) ) )
        if( file[1] == -2 )
            dirs += ({ file[0] });

    // Make a mapping of names to filenames.
    list = ([ ]);
    foreach( dir in dirs ) {
        foreach( file_name in unguarded( (: get_dir( $(base+dir+"/*.c") ) :) ) )
            if( file_name[0] != '.')
                list[ replace_string( explode( file_name, ".")[0],
                    "_", " ") ] = base+dir+"/"+file_name;
    }

    foreach( file_name in unguarded( (: get_dir, base+"*.c" :) ) ) {
        if( file_name[0] != '.') {
            list[ replace_string( explode( file_name, ".")[0],
                "_", " ") ] = base+file_name;
        }
    }

    return list;

} /* make_list() */

/** @ignore yes */
int do_rehash() {
    armour = make_list(ARMOUR_D);
    weapon = make_list(WEAPON_D);
    clothing = make_list(CLOTHING_D);
    jewellery = make_list(JEWELLERY_D);
    scabbard = make_list(SCABBARD_D);
    misc = make_list(MISC_D);
    if( creatorp(TP) )
        write("Rehash completed.\n");
    return 1;
} /* do_rehash() */

void setup() {
    set_light(60);
    set_short("PT Armoury");
    set_determinate("the ");

    set_long("This is the PT Armoury.  From here you can list all of the PT "
        "\"weapons\", \"armour\", \"clothing\", \"jewellery\", "
        "\"scabbards\" and \"misc\" that are available, and \"request\" one "
        "of them.  The armoury stores its list of items in mappings, which "
        "can be updated with \"rehash\".\n");

    armour = weapon = clothing = jewellery = scabbard = misc = ([ ]);

    do_rehash();

} /* setup() */

/**
 * Returns the current list of known weapons.
 * @return the weapons mapping
 */
mapping query_weapons() { return weapon; }

int query_is_weapon( string str ) { return !undefinedp( weapon[str] ); }

/**
 * Returns the current list of known armours.
 * @return the armours mapping
 */
mapping query_armour() { return armour; }

int query_is_armour( string str ) { return !undefinedp( armour[str] ); }

/**
 * Returns the current list of known clothing.
 * @return the clothing mapping
 */
mapping query_clothing() { return clothing; }

int query_is_clothing( string str ) { return !undefinedp( clothing[str] ); }

/**
 * Returns the current list of known jewelley.
 * @return the jewelley mapping
 */
mapping query_jewellery() { return jewellery; }

int query_is_jewellery( string str ) { return !undefinedp( jewellery[str] ); }

/**
 * Returns the current list of known scabards.
 * @return the scabards mapping
 */
mapping query_scabbards() { return scabbard; }

int query_is_scabbard( string str ) { return !undefinedp( scabbard[str] ); }

/**
 * Returns the current list of known misc items.
 * @return the scabards mapping
 */
mapping query_misc() { return misc; }

int query_is_misc( string str ) { return !undefinedp( misc[str] ); }

/**
 * General item retreival function.  It returns a new item.
 * The "word" parameter is the name of the requested item, essentially
 * the filename without directory and with "_"'s changed to " "'s.
 * <p>
 * @param word the name of the item to retreive
 * @return the requested object, or 0 on failure
 */
object request_item( string word ) {
    object thing;
    string filename;

    if( !word )
        return 0;

    if( weapon[word] )
        filename = weapon[word];
    else if( armour[word] )
        filename = armour[word];
    else if( clothing[word] )
        filename = clothing[word];
    else if( jewellery[word] )
        filename = jewellery[word];
    else if( scabbard[word] )
        filename = scabbard[word];
    else if( misc[word] )
        filename = misc[word];
    else {
        printf("No file for item \"%s\".\n", word );
        log_file("PT_ARMOURY", "No file for %s (%s)\n", word, file_name(PO) );
        return 0;
    }

    if( file_exists( filename ) )
        thing = clone_object(filename);

    if( !thing ) {
        printf("Cannot find item \"%s\".\n", word );
        log_file("PT_ARMOURY", "Cannot find %s (%s)\n", word, file_name(PO) );
        return 0;
    }

    return thing;

} /* request_item() */

/** @ignore yes */
int do_request( string word ) {
    object thing;

    if( !word || word == "" )
        return notify_fail("Request what?\n");

    thing = request_item( word );

    if( !thing )
        return notify_fail( word+" not found.\n");

    if( (int)thing->move(TP) ) {
        write( (string)thing->the_short()+" has been placed in here.\n");
        thing->move(TO);
    } else
        write( (string)thing->the_short()+" has been placed in your "
            "inventory.\n");

    return 1;

} /* do_request() */

/** @ignore yes */
void init() {
    ::init();
    add_command("request", "<string'item'>", (: do_request($4[0]) :) );
    add_command("weapons", "");
    add_command("armour", "");
    add_command("clothing", "");
    add_command("jewellery", "");
    add_command("scabbards", "");
    add_command("misc", "");
    add_command("rehash", "");
} /* init() */

/** @ignore yes */
int do_armour() {
    if( !sizeof(armour) )
        return notify_fail("There are no armours in PT right now.\n");
    printf("Armours available for PTs are:\n%-*#s\n",
        (int)TP->query_cols(), implode(keys(armour), "\n") );
    return 1;
} /* do_armour() */

/** @ignore yes */
int do_weapons() {
    if( !sizeof(weapon) )
        return notify_fail("There are no weapons in PT right now.\n");
    printf("Weapons available for PTs are:\n%-*#s\n",
        (int)TP->query_cols(), implode(keys(weapon), "\n") );
    return 1;
} /* do_weapons() */

/** @ignore yes */
int do_clothing() {
    if( !sizeof(clothing) )
        return notify_fail("There are no clothes in PT right now.\n");
    printf("Clothes available for PTs are:\n%-*#s\n",
        (int)TP->query_cols(), implode(keys(clothing), "\n") );
    return 1;
} /* do_clothing() */

/** @ignore yes */
int do_jewellery() {
    if( !sizeof(jewellery) )
        return notify_fail("There are no jewellery in PT right now.\n");
    printf("Jewellery available for PTs are:\n%-*#s\n",
        (int)TP->query_cols(), implode(keys(jewellery), "\n") );
    return 1;
} /* do_jewellery() */

/** @ignore yes */
int do_scabbards() {
    if( !sizeof(scabbard) )
        return notify_fail("There are no scabbards in PT right now.\n");
    printf("Scabbards available for PTs are:\n%-*#s\n",
        (int)TP->query_cols(), implode(keys(scabbard), "\n") );
    return 1;
} /* do_scabbards() */

/** @ignore yes */
int do_misc() {
    if( !sizeof(misc) )
        return notify_fail("There are no misc items in PT right now.\n");
    printf("Misc items available for PTs are:\n%-*#s\n",
        (int)TP->query_cols(), implode(keys(misc), "\n") );
    return 1;
} /* do_misc() */
