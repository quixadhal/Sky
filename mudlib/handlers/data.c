/**
 * Universal data initialization handler.
 * The data initializer can be used to initialize databases for use in other
 * handlers.  The database is defined in a user-provided text file, with
 * a format similar to the virtual object compiler.  The input file is
 * converted to a temporary object, which allows fairly complicated
 * expressions to be used as data values.  The initializer can handle
 * arrays and mappings (nested to any level (theoretically)), with a base
 * element of any type, including mixed and classes.
 * <p>
 * To initialize a variable, assign it the value returned by
 * compile_data() in the initializer.  compile_data() takes an array of
 * filenames as its only argument.
 * <p>
 * <b>File format</b>
 * The data file uses the following keywords.  Each keyword is followed
 * by the required data.  The data can be spread over multiple lines,
 * following the same rules as for LPC code, but the keywords must be at
 * the beginning of the line (preceded by optional whitespace).
 * <dl>
 * <dt>::data:: < array | mapping > [ of ] ... < base >
 * <dd>
 * This keyword defines the structure of the data.  The word "of" is
 * optional.  "array" and "mapping" may be repeated as many times as
 * desired.  "base" is the base type of the data.  For classes, it would
 * be of the form "class <classname>".  For types other than classes,
 * the base isn't really
 * used at this time, but something needs to be there to keep the parser
 * in line.  Some examples:
 * <b>
 * <pre>::data:: array of mapping of array of int
 * ::data:: mapping of mapping of mixed
 * ::data:: mapping of array of class myclass</pre>
 * There should only be one ::data:: keyword in the list of files passed
 * to compile_data().  Also, note that classes need to be defined before
 * this statement.  This can be done either with ::quote:: or
 * ::#include::.
 * <dt>::item <index> [,] ... :: [ value ]
 * <dd>This keyword defines the value for one item of the data.  <index> is
 * repeated as often as necessary, given the structure declared in the
 * ::data:: statement.  For mappings, the index can be any valid mapping key.
 * For arrays, the index can be either a number, or the strings i, i++,
 * or ++i, for current index, current index (incrementing afterwards), or
 * next index.  The value can (and probably should) be omitted for
 * classes, with the field values specified with the "->" keyword below.
 * Examples (corresponding to the three ::data:: examples above):
 * <pre>
 * ::item 0, "item 1", 2:: 42
 * ::item "a", "b":: ({ 1, "fred", ({ 2, 3 }) })
 * ::item "x" i++::
 * </pre>
 * <dt> ::-><field>:: <value>
 * <dd>This allows the fields of items of type class to be assigned
 * individually.  In general, the preceding ::item:: keyword should not
 * have been given a value.  The class must have been defined previously,
 * either with an ::#include:: directive, or with the ::quote:: keyword.
 * Examples:
 * <pre>
 * ::Quote::
 * class item_data {
 *     string *season;
 *     mixed  quant;
 *     string ob;
 * }
 * 
 * ::Data:: mapping of class item_data

 * :item "white clover"::
 * ::->season:: ({ "spring" })
 * ::->quant:: ({ 3, 4 })
 * ::->ob:: "/std/plant"
 * </pre>
 * These statements set the season, quant, and ob fields of the mapping
 * <dt>::quote:: <LPC code>
 * <dd>This keyword allows specific LPC statements to be inserted in the
 * object that creates the database.  To use this effectively requires a
 * little understanding of the translation process.  First, all lines
 * associated with a given keyword are folded into one line.  This means
 * that using the "//" comment delimiter in a ::quote:: will cause the
 * remainder of the quoted statements to be ignored.  Second, the
 * prototype of the function that returns the data isn't written until
 * the ::data:: keyword is encountered.  Therefore, any "global"
 * statements (such as class definitions) should be included or quoted
 * before the ::data:: line.  The easiest way to see what's going on is
 * to try a few examples and look at the resulting .c file (which is the
 * first data file name with ".c" stuck on the end).
 * </dl>
 * @example
 * mixed data;
 * data = "/handlers/data"->compile_data(({ "/save/file1.dat",
 *      "/save/file2.dat" }));
 * // This will catenate the two files into one, translate it, and return
 * // the data.  Of course, someone has to create the data files also.
 * @author Jeremy
 */

#define DEBUG !
#define DC_DELIM 0
#define DC_ARRAY 1
#define DC_MAPPING 2
#define DC_OF 3
#define DC_CLASS 4
#define DC_STRING 5
#define DC_ITEM 6
#define DC_NUMBER 7
#define DC_GREY 8

// This is the number of characters it reads before starting a new
// function.  I set it to half of the biggest set of files that caused
// me problems.  It can be adjusted as needed.
#define MAX_SUBF_SIZE 16000

#define WHITESPACE(c) (c == 10 || c == 32 || c == '\n')

#ifdef DEBUG
#  define Error(s) write(s); log_file( "DATA_COMPILER", s);
#else
#  define Error(s) ;
#endif

string std_euid;

void create() {
    std_euid = "/secure/master"->creator_file(file_name(this_object()));
    seteuid(std_euid);
    //Error("Note: euid at creation is " + geteuid() + "\n");
}

int tmp_file_no;

private string strip_string( string str ) {
    int     i, j;
 
    if (!str || str == "") return "";
    j = strlen( str ) - 1;
    for( ; WHITESPACE( str[ i ] ) && i <= j; i++ ) ;
    for( ; WHITESPACE( str[ j ] ) && j > i; j-- ) ;
    return str[ i..j ];
} /* strip_space() */

private mixed cleanup_assoc( mixed parse ) {
    int j;

    // Clean out whitespace (seems like there should be an easier
    // way to do this...)
    for (j = 0; j < sizeof(parse[0]); j++) {
        if ((parse[1][j] == DC_DELIM) || (parse[1][j] == DC_OF)) {
            parse[0][j] = 0;
            parse[1][j] = 0;
        }
    }
    parse[0] -= ({ 0 });
    parse[1] -= ({ 0 });
    return parse;
} /* cleanup_assoc */

/**
 * Actualy compiles the files down.
 * See the header file for a more detailed explaination
 * @param path the files to parse
 */
mixed compile_data( string *path ) {
    string tmp_name, data = "", file_data = "", s1, tmp_val, base, keyword;
    string *segments, *ind, *val, cur_index;
    int i, j, t, debug_file, class_pending, stat, subfunc_cnt,
        subfunc_char_cnt, data_keyword_found;
    int *index_types;
    mixed parse, index_max;

    // Most of this is blatantly stolen from /global/virtual/compiler.c
    if (!sizeof(path))
      return 0;
    tmp_name = path[0] + "_dc.c";
    if( find_object( tmp_name ) )
        tmp_name->dest_me();
    if (file_size(tmp_name) > 0) {
        if ((stat = seteuid("Root")) == 0) {
            // This always happens.  But everything seems to work,
            // so I'll just take it out.
            // Error("Error: couldn't set euid to Root (" + stat + ", " +
            //       "secure/master"->valid_seteuid(this_object(), "Root") +
            //       ")\n");
        }
        stat = unguarded((: rm, tmp_name :));
        if (!stat) {
            Error("Error: couldn't remove old .c file (" +
                  geteuid(this_object()) + ", " +
                  "secure/master"->valid_seteuid(this_object(), "Root") +
                  ")\n");
            seteuid(std_euid);
            return 0;
        }
        //Error("Note: " + tmp_name + " removed (supposedly).\n");
    }
    seteuid(std_euid);
    for (i = 0; i < sizeof(path); i++) {
        if (file_size(path[i]) <= 0)
          continue;
        data += read_file( path[i] );
    }
    if (!data) {
        Error("Error: file(s) not found.\n");
        return 0;
    }
    /* Lines beginning with a # are a comment... */
    /* Break into segments at comments */
    segments = explode( "$\n" + data, "\n#" );
    if( !segments ) {
        Error( "prop_to_fun() : Nothing but comments?\n" );
        return 0;
    }
    /* Remove dummy $ (?) */
    segments[ 0 ] = segments[ 0 ][ 1..(sizeof(segments[ 0 ]) - 1) ];
    /* Remove comment lines */
    for( i = 1; i < sizeof( segments ); i++ ) {
        if( sscanf( segments[ i ], "%s\n%s", s1, segments[ i ] ) != 2 ) {
            segments[ i ] = "";
        }
    }
    /* Join segments together again */
    data = implode( segments, "\n" );
    
    /* See example file for explanation of syntax. */
 
    segments = explode( strip_string( data ), "::" );
    /* sizeof(segments) can be odd if the last line has no argument */
    if (sizeof( segments ) % 2) {
      segments += ({""});
    }
    ind = allocate( sizeof( segments ) / 2 );
    val = allocate( sizeof( segments ) / 2 );
 
    for( i = 0; i < sizeof( ind ); i++ ) {
        ind[ i ] = segments[ i * 2 ];
        //val[ i ] = replace( segments[ i * 2 + 1 ], "\n", " " );
        // This preserves the newlines; it makes it more readable and avoids
        // line length problems.
        val[ i ] = strip_string( segments[ i * 2 + 1 ] );
        /* look for virtual compiler meta char */
        if( ind[ i ][ 0..0 ] == "#" ) {
            ind[ i ] = lower_case( ind[ i ] );
            if( ind[ i ] == "#debug" ) {
                /* debug errent virtual programs, ie, don't rm */
                /* the .c file if debug_file is non-zero */
                sscanf( val[ i ], "%d", debug_file );
            } else if( ind[ i ] == "#include" ) {
                /* include the file in setup(), just before the */
                /* object is cloned.  */
                tmp_val = val[i];
                file_data += "#include " + replace( tmp_val, " ", "" ) + "\n";
            }
        }
    }
    for( i = 0; i < sizeof( ind ); i++ ) {
        keyword = lower_case( ind[ i ] );
        if( keyword[ 0..0 ] == "#" ) {
            /* it's a virtual keyword don't stick it in the .c file */
            continue;
        }
        // Keep track of how big the function is.  The easiest way
        // to judge the amount of code is by counting the characters.
        subfunc_char_cnt += sizeof(ind[i]) + sizeof(val[i]);
        if (keyword == "data") {
            if (data_keyword_found) {
                Error("Error: more than one data keyword found.\n");
                return 0;
            }
            data_keyword_found = 1;
            // This declares the structure of the database
            file_data += "void dest_me() { destruct( this_object() ); }\n\n";
            // Break up code into multiple functions; big files can
            // run into trouble compiling.
            parse = reg_assoc(val[i],
                              ({ "array", "mapping", "of", "class +[^\t ]+",
                                 "[^\t ]+" }),
                              ({ DC_ARRAY, DC_MAPPING, DC_OF, DC_CLASS,
                                 DC_GREY }),
                              DC_DELIM);
            parse = cleanup_assoc( parse );
            //printf("parse = %O\n", parse);
            for (j = 0; (j < sizeof(parse[0])) && !index_max; j++) {
                switch (parse[1][j]) {
                  case DC_ARRAY:
                    break;
                  case DC_MAPPING:
                    break;
                  case DC_CLASS:
                    base = implode(parse[0][j..], " ");
                    file_data += base + " item;\n";
                  case DC_GREY:
                    index_types = parse[1][0..j-1];
                    index_max = allocate(sizeof(index_types));
                    break;
                  default:
                    Error("Error: data parse error 1 (" + parse[0][j] + ")\n");
                    return 0;
                }
            }
            if (index_types[0] == DC_MAPPING)
              file_data += "mapping data = ([ ]);\n\n";
            else
              file_data += "mixed data;\n\n";
            for (j = 0; j < sizeof(index_types); j++) {
                switch (index_types[j]) {
                  case DC_ARRAY:
                    index_max[j] = -1;
                    break;
                  case DC_MAPPING:
                    //index_max[j] = ([ ]);
                    break;
                  default:
                    Error("Error: illegal index type found ("
                          + index_types[j] + ")\n");
                    return 0;
                }
            }
            file_data += "mixed data_return_" + subfunc_cnt + "() {\n";
            subfunc_cnt++;
            continue;
        }
        if (keyword[0..3] == "item") {
            // This is where the actual array values get assigned
            // First take care of pending class assignments
            if (class_pending) {
                file_data += "  data" + cur_index + " = item;\n";
                class_pending = 0;
            }
            // Check to see if we should break to a new function (this
            // is the safest place, since we can't break up an item
            // definition).
            if (subfunc_char_cnt > MAX_SUBF_SIZE) {
                // This could theoretically fail if we somehow got more than
                // MAX_SUBF_SIZE characters before the first "data" keyword.
                // So it goes...
                file_data += "}\n\n";
                file_data += "mixed data_return_" + subfunc_cnt + "() {\n";
                subfunc_cnt++;
                subfunc_char_cnt = 0;
            }
            parse = reg_assoc( ind[i], ({ "\"[^\"]*\"", "item", "[0-9]+",
                                      "[^,\t ]+"}),
                              ({ DC_STRING, DC_ITEM, DC_NUMBER, DC_GREY }));
            parse = cleanup_assoc( parse );
            //printf("parse = %O\nindex_max = %O\n", parse, index_max);
            cur_index = "";
            for (j = 0; j < sizeof(index_types); j++) {
                switch (index_types[j]) {
                  case DC_ARRAY:
                    // Index can be a number, "i", "i++", or "++i"
                    if (parse[1][j+1] != DC_NUMBER) {
                        if (parse[0][j+1] == "i") {
                            parse[0][j+1] = index_max[j] + "";
                        } else if (parse[0][j+1] == "i++") {
                            parse[0][j+1] = index_max[j] + "";
                            index_max[j]++;
                        } else if (parse[0][j+1] == "++i") {
                            index_max[j]++;
                            parse[0][j+1] = index_max[j] + "";
                        } else {
                          Error("Error: illegal index for array (" +
                                  parse[0][j+1] + ")\n");
                            return 0;
                        }
                    }
                    if ((t = to_int(parse[0][j+1])) > index_max[j]) {
                        file_data += "  data" + cur_index
                          + " = allocate(" + (t-index_max[j]) + ");\n";
                        index_max[j] = t;
                    }
                    break;
                  case DC_MAPPING:
                    if (j) {
                        file_data += "  if (!mappingp(data" + cur_index + "))"
                          + " data" + cur_index + " = ([]);\n";
                    }
                    break;
                  default:
                    Error("Error: illegal index type found (" + index_types[j]
                          + ")\n");
                    return 0;
                } /* switch */
                cur_index += "[" + parse[0][j+1] + "]";
            } /* for */
            if (strip_string(val[i]) != "") {
                //printf("val[i] = %O\n", val[i]);
                file_data += "  data" + cur_index + " = " + val[i] + ";\n";
            }
            continue;
        } /* if */
        if (keyword[0..1] == "->") {
            // This is for handling fields of classes
            if (!class_pending) {
                file_data += "  item = new( " + base + " );\n";
                class_pending = 1;
            }
            file_data += "  item->" + ind[i][2..] + " = " + val[i] + ";\n";
            continue;
        }
        if (keyword == "quote") {
            // A backdoor for putting in specific LPC lines
            file_data += val[i] + "\n";
            continue;
        }
    } /* for */
    if (class_pending) {
        file_data += "  data" + cur_index + " = item;\n";
    }
    file_data += "}\n\n";
    file_data += "mixed data_return() {\n";
    for (i = 0; i < subfunc_cnt; i++) {
        file_data += "  data_return_" + i + "();\n";
    }
    file_data += "  return data;\n}\n";
    seteuid("Root");
    unguarded((: write_file, tmp_name, file_data, 1 :));
    seteuid(std_euid);
    return tmp_name->data_return();
} /* compile_data() */
