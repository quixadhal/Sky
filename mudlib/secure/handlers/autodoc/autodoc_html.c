/**
 * This creates an html document from the generated automatic documentation.
 * @see /handlers/autodoc/autodoc_handler
 * @author Pinkfish
 * @started Tue Oct 28 11:39:47 EST 1997
 * @change Sandoz 27 Apr 2002 - Changed to generate html code on the
 * fly from autodoc save files, so we won't have a separate html file
 * for each file.
 */

#include <autodoc.h>
#include <runtime_config.h>

#define SMALL_IMAGE "http://divided-sky.com:7303/images/divided_sky_small.jpg"
#define LARGE_IMAGE "http://divided-sky.com:7303/images/divided_sky.jpg"
#define COLOURS     "bgcolor=\"#FFFFFF\" text=\"#000000\" link=\"#0000FF\" "\
                    "vlink=\"#0033CC\" alink=\"#0033CC\""
#define RED_BALL    "<img src=\"images/red-ball.gif\" width=\"12\" " \
                    "height=\"12\" border=\"0\">"

mapping public_functions,
        protected_functions,
        inherits,
        main_docs,
        define_docs,
        includes,
        class_docs;

string file_name;

/** @ignore yes */
private void setup() {
    main_docs = 0;
    file_name = "";
    public_functions = ([ ]);
    protected_functions = ([ ]);
    inherits = ([ ]);
    define_docs = ([ ]);
    includes = ([ ]);
    class_docs = ([ ]);
} /* setup() */

/** @ignore yes */
void create() {
    seteuid(getuid());
    setup();
} /* create() */

/**
 * Creates a html index file from the input index information.
 * This will contain all the functions starting with a certain letter
 * and the corresponding files they are mentioned in.
 *
 * @param index the values in the index
 * @param letter the letter this is an index of
 * @param output the file to write the output to
 */
void create_html_index_file( string *index, string letter, string output ) {
    string name, bing;
    mixed tmp;


    bing = "<html><head><title>\nIndex of " + letter +
           "\n</title></head><body " + COLOURS + ">\n";
    bing += "<img align=left src=\"" + SMALL_IMAGE + "\">\n";
    bing += "<p align=right><code><a href=\"index.html\">"
            "Main Index</a></code></p>\n"
            "<h2>Index of " + letter + "</h2>\n"
            "<h3><i>Letters can change the world, remove the l "
            "from world and you have word</i></h3>\n"
            "<br clear=both>\n"
            "<hr><dl>\n";

    index = sort_array(index, (: strcasecmp($1[0], $2[0]) ?
                                 strcasecmp($1[0], $2[0]) :
                                 strcasecmp($1[1], $2[1]) :) );

    rm( output );

    foreach( tmp in index ) {
        name = tmp[AUTO_INDEX_FILE_NAME];
        if( sscanf( tmp[AUTO_INDEX_NAME], "%*s.%*s") == 2 ) {
            bing += "<dt><img src=\"images/magenta-ball-small.gif\" "
                    "height=6 width=6 alt=\" . \"><b><a href=\""
                    "../../secure/autodoc.c?file="+name+"\">"+
                    tmp[AUTO_INDEX_NAME]+"</a></b>.\n";
        } else {
            bing += "<dt><img src=\"images/magenta-ball-small.gif\" "
                    "height=6 width=6 alt=\" . \"><b><a href=\""+
                    "../../secure/autodoc.c?file="+
                    name+"#"+tmp[AUTO_INDEX_NAME]+"\">"+
                    tmp[AUTO_INDEX_NAME]+"</a></b>"+
                    tmp[AUTO_INDEX_ARGS]+"; <a href=\""+
                    name+"\">"+tmp[AUTO_INDEX_FILE_NAME]+"</a>\n";
        }

        if( tmp[AUTO_INDEX_SUMMARY] )
            bing += "<dd>"+tmp[AUTO_INDEX_SUMMARY]+"\n";

        // Safety measure, this string can get VERY long, and crash us.
        if( strlen(bing) > 75 * get_config( __MAX_STRING_LENGTH__ ) / 100 ) {
            write_file( output, bing );
            bing = "";
        }
    }

    write_file( output, bing + "</dl>" );
    write_file( output, "</body></html>");

} /* create_html_index_file() */

// Move these out to let us use function pointers on them.
private string bing_cre;
private string bing_eff;
private string bing_index;

/**
 * Creates the basic index file which points to all the other index
 * files...  The input letters array is a paired array, the first element
 * is the letter and the second is the file name.
 *
 * @param letters the names of all the other index files
 * @param output_dir the directory to write stuff to
 */
void create_main_index( mapping chars, string output_dir ) {
   int i;
   string output_index, output_eff, output_cre, output_main;
   string str, fname, fluff, bing_main;
   string *files, *index, *tmp;
   mixed stuff;
   mapping summaries;
   function write_all;

   summaries = AUTODOC_H->query_summary_map();
   output_index = output_dir + "index_std.html";
   output_eff = output_dir + "index_eff.html";
   output_cre = output_dir + "index_cre.html";
   output_main = output_dir + "index.html";
   bing_cre = "";
   bing_eff = "";
   bing_index = "";
   bing_main = "";

   /* Rm the files so we write new ones... */
   rm( output_eff );
   rm( output_index );
   rm( output_cre );
   rm( output_main );

   bing_main += "<html><head><title>"+mud_name()+" Autodoc Index</title></head>\n"
                "<body " + COLOURS + ">\n";
   bing_main += "<img align=\"middle\" src=\"" + LARGE_IMAGE + "\">\n";
   bing_main += "<h2>"+mud_name()+" Autodoc Index</h2>\n"
                "<h3><i>Where even death is nervous</i></h3>\n"
                "<br clear=both>\n";
   bing_main += "<h2>Function Index</h2>\n";
   bing_main += "<p>";

   files = sort_array( keys(chars), 1 );

   for( i = 0; i < sizeof(files); i++ ) {
      if( i > 0 )
          bing_main += ", ";
      bing_main += "<a href=\""+chars[files[i]]+"\">"+files[i]+"</a>\n";
   }

   bing_main += "</p>\n";
   bing_main += "<h2>Packages</h2>\n";
   bing_main += "<dl>\n";
   bing_main += "<dt>"+RED_BALL+"\n"
                "<a href=\"index_std.html\">Standard Mudlib Documentation</a>\n";
   bing_main += "<dt>"+RED_BALL+"\n"
                "<a href=\"index_eff.html\">Effect Documentation</a>\n";
   bing_main += "<dt>"+RED_BALL+"\n"
                "<a href=\"index_cre.html\">Creator Created Documentation</a>\n";
   bing_main += "</dl></body></html>";

   write_file( output_main, bing_main, 1 );

   write_all = (: bing_eff += $1,
                  bing_cre += $1,
                  bing_index += $1 :);
   evaluate( write_all, "<html><head><title>\n");
   evaluate( write_all, "Autodoc index\n</title></head>\n"
                        "<body " + COLOURS + ">\n");
   bing_index += "<img align=left src=\"" + SMALL_IMAGE + "\">\n";
   bing_index += "<h2>Creator Documentation</h2>\n"
                 "<h3><i>Where death looks over his shoulder</i></h3>\n"
                 "<br clear=both>\n";
   bing_eff += "<img align=left src=\"" + SMALL_IMAGE + "\">\n";
   bing_eff += "<h2>Creator Effect Documentation</h2>\n"
               "<h3><i>Where your consequences are vanquished</i></h3>\n"
               "<br clear=both>\n";
   bing_cre += "<img align=left src=\"" + SMALL_IMAGE+"\">\n";
   bing_cre += "<h2>Creator Documentation</h2>\n"
               "<h3><i>Code, create and well...code some more</i></h3>\n"
               "<br clear=both>\n";

   evaluate( write_all, "<h2>Function index</h2>\n");

   files = AUTODOC_H->query_files();
   files = sort_array( files, (: strcasecmp :) );
   index = sort_array( keys(chars), (: strcasecmp :) );

   for( i = 0; i < sizeof(index); i++ ) {
      if( i > 0 )
          evaluate( write_all, ", ");
      evaluate( write_all, "<a href=\""+chars[index[i]]+"\">"+
          index[i]+"</a>\n");
   }

   /* Main index */
   bing_index += ", <a href=\"index_eff.html\">Effects</a>";
   bing_index += ", <a href=\"index_cre.html\">Creator</a>";
   /* Effect index */
   bing_eff += ", <a href=\"index.html\">Main</a>";
   bing_eff += ", <a href=\"index_cre.html\">Creator</a>";
   /* Creator index */
   bing_cre += ", <a href=\"index.html\">Main</a>";
   bing_cre += ", <a href=\"index_eff.html\">Effects</a>";

   evaluate( write_all, "<hr><h2>Class index</h2>\n");
   evaluate( write_all, "<dl>\n");

   stuff = unique_array( files, (: $1[0..strsrch( $1, "/", -1 )] :) );

   foreach( tmp in stuff ) {
      str = tmp[0][0..strsrch(tmp[0], "/", -1 )];
      if( str[0..12] == "/std/effects/" ) {
          bing_eff += "<dt><img src=\"images/cyan-ball.gif\" height=12 "
                      "width=12><b><a href=\"#"+str+"\">"+str+"</a></b>\n";
      } else if( str[0..2] == "/w/" ) {
          bing_cre += "<dt><img src=\"images/cyan-ball.gif\" height=12 "
                      "width=12><b><a href=\"#"+str+"\">"+str+"</a></b>\n";
      } else {
          bing_index += "<dt><img src=\"images/cyan-ball.gif\" height=12 "
                      "width=12><b><a href=\"#"+str+"\">"+str+"</a></b>\n";
      }
   }

   evaluate( write_all, "</dl>\n");

   foreach( tmp in stuff ) {
      fluff = "";
      str = tmp[0][0..strsrch(tmp[0], "/", -1 )];
      fluff += "<h2><a name=\""+str+"\">"+str+"</a></h2>\n<dl>\n";
      reset_eval_cost();
      foreach( fname in tmp ) {
         fluff += "<dt><img src=\"images/cyan-ball-small.gif\" height=6 "
                  "width=6><a href=\"../../secure/autodoc.c?file="+fname+"\">"+
                  fname[strsrch( fname, "/", -1 )+1..]+"</a>\n";
         if( summaries[fname] )
             fluff += "<dd>"+summaries[fname]+"\n";
      }
      fluff += "</dl>\n";
      if( tmp[0][0..12] == "/std/effects/")
          bing_eff += fluff;
      else if( tmp[0][0..2] == "/w/")
          bing_cre += fluff;
      else
          bing_index += fluff;
   }

   evaluate( write_all, "</body></html>\n");

   write_file( output_eff, bing_eff, 1 );
   write_file( output_eff, "</body></html>\n");

   write_file( output_index, bing_index, 1 );
   write_file( output_index, "</body></html>\n");

   write_file( output_cre, bing_cre, 1 );
   write_file( output_cre, "</body></html>\n");

} /* create_main_index() */

/*
 * This method creates a reference string from the input reference.
 * @param str the string to create a reference for
 * @return the reference string
 */
private string create_href( string str ) {
    string name, func;

    if( sscanf( str, "%s#%s", name, func ) == 2 )
        return "<a href=\"autodoc.c?file="+name+"#"+func+"\">"+name+"->"+
               func+"()</a>";

    if( sscanf( str, "%s->%s", name, func ) == 2 ) {
        sscanf( func, "%s(%*s)", func );
        return "<a href=\"autodoc.c?file="+name+"#"+func+"\">"+str+"</a>";
    }

    if( sscanf( str, "%s()", func ) == 1 )
        return "<a href=\"#"+func+"\">"+str+"</a>";

    if( str[0..4] == "http:" )
        return "<a href=\""+str+"\">"+str+"</a>";

    if( AUTODOC_H->is_autodoc_file( str ) )
        return "<a href=\"autodoc.c?file="+str+"\">"+str+"</a>";

    return str;

} /* create_href() */

/** @ignore yes */
private string query_func_def( mixed stuff, string func ) {
   string ret;
   int i, pad;

   ret = implode( stuff[AUTO_TYPE], " ") + " " + func+"(";
   pad = strlen(ret);

   for( i = 0; i < sizeof(stuff[AUTO_ARGS]); i += 2 ) {
      if( i )
          ret += sprintf(",\n%-"+pad+"s", "");
      ret += implode(stuff[AUTO_ARGS][i + AUTO_ARGS_TYPE], " ") + " " +
             stuff[AUTO_ARGS][i + AUTO_ARGS_NAME];
   }

   return ret + ")";

} /* query_func_def() */

/** @ignore yes */
private string query_method_description( mapping docs ) {
    string name, desc, bing, tmp;

    bing = "";

    if( docs["main"] )
        bing += "<dd>"+implode( docs["main"], " ")+"<p>\n";

    if( docs["param"] || docs["return"] || docs["see"] || docs["example"] )
        bing += "<dd><dl>\n";

    if( docs["param"] ) {
        bing += "<dt><b>Parameters:</b>\n";
        foreach( tmp in docs["param"] ) {
            if( sscanf( tmp, "%s %s", name, desc ) == 2 )
                bing += "<dd>"+name+" - "+(desc)+"\n";
        }
    }

    if( docs["return"] )
        bing += "<dt><b>Returns:</b>\n<dd>"+implode(docs["return"], " ")+"\n";

    if( docs["see"] )
        bing += "<dt><b>See also:</b>\n<dd>"+query_multiple_short(
                map(docs["see"], (: create_href($1) :) ) );

    if( docs["example"] )
        bing += "<dt><b>Example:</b>\n<dd><pre>"+implode( docs["example"],
                "</pre><dd><pre>\n")+"</pre>";

    if( docs["param"] || docs["return"] || docs["see"] || docs["example"] )
        bing += "</dl>\n";

    return bing;

} /* query_method_description() */

/** @ignore yes */
private string query_methods( int prot ) {
    mixed stuff;
    string bing, *funcs, func;
    function f;

    funcs = keys( prot ? protected_functions : public_functions );
    funcs = sort_array( funcs, (: strcasecmp( $1, $2 ) :) );

    f = function( mixed stuff, string func ) {
            string ret;
            int i, pad;

            ret = implode( stuff[AUTO_TYPE], " ")+" "+func+"(";
            pad = strlen(ret);
            for( i = 0; i < sizeof(stuff[AUTO_ARGS]); i += 2 ) {
                if( i )
                    ret += sprintf(",\n%-"+pad+"s", "");
                ret += implode( stuff[AUTO_ARGS][i + AUTO_ARGS_TYPE], " ")+
                       " "+stuff[AUTO_ARGS][i + AUTO_ARGS_NAME];
            }
            ret += ")";
            return ret;
        };

    bing = "";

    foreach( func in funcs ) {
        stuff = ( prot ? protected_functions : public_functions )[func];
        bing += "<dt><a name=\""+func+"\">\n";
        bing += "<dt><img src=\"autodoc/images/"+( prot ? "magenta" : "cyan")+
                "-ball.gif\" height=12 width=12 alt=\" * \">";
        bing += "<b>"+func+"</b></a><pre>\n"+evaluate( f, stuff, func )+
                "\n</pre>";
        if( stuff[AUTO_DOCS]["main"] )
            bing += query_method_description( stuff[AUTO_DOCS] );
        bing += "\n";
    }

    return bing;

} /* query_methods() */

/** @ignore yes */
private string query_class_def( mixed stuff, string name ) {
    string ret;
    int pad;
    mixed member;

    ret = "class "+name+" {\n";
    pad = strlen(ret);

    foreach( member in stuff[AUTO_CLASS_MEMBERS] ) {
        ret += sprintf("%-"+pad+"s", "");
        ret += implode( member[AUTO_CLASS_TYPE], " ")+" "+
               member[AUTO_CLASS_NAME]+";\n";
    }

    return ret+"}\n";

} /* query_class_def() */

/** @ignore yes */
private string query_class_description( mapping docs ) {
    string name, desc, bing, tmp;

    bing = "";

    if( docs["main"] )
        bing += "<dd>"+implode( docs["main"], " ")+"<p>\n";

    if( docs["member"] || docs["see"] )
        bing += "<dl>\n";

    if( docs["member"] ) {
        bing += "<dt><b>Members:</b>\n";
        foreach( tmp in docs["member"] ) {
            if( sscanf( tmp, "%s %s", name, desc ) == 2 )
                bing += "<dd>"+name+" - "+(desc)+"\n";
        }
    }

    if( docs["see"] )
        bing += "<dt><b>See also:</b>\n<dd>"+query_multiple_short(
                map(docs["see"], (: create_href($1) :) ) );

    if( docs["member"] || docs["see"] )
        bing += "</dl>\n";

    return bing;

} /* query_class_description() */

/** @ignore yes */
private string query_short_desc( mapping docs ) {
    mixed main;
    int pos, tmp_pos;

    if( !main = docs["main"] )
        return 0;

    /* Look for the first sentance... */
    main = implode( main, " ");
    pos = strlen(main);
    tmp_pos = strsrch( main, ".");

    if( tmp_pos < pos && tmp_pos != -1 )
        pos = tmp_pos;

    tmp_pos = strsrch( main, "?");

    if( tmp_pos < pos && tmp_pos != -1 )
        pos = tmp_pos;

    tmp_pos = strsrch( main, "!");

    if( tmp_pos < pos && tmp_pos != -1 )
        pos = tmp_pos;

    return main[0..pos];

} /* query_short_desc() */

/** @ignore yes */
private string query_short_args_def( mixed args ) {
    string ret;
    int i;

    ret = "";

    for( i = 0; i < sizeof(args); i += 2 ) {
        if( i )
            ret += ", ";
        ret += implode( args[i + AUTO_ARGS_TYPE], " ");
    }

    return "(" + ret + ")";

} /* query_short_args_def() */

/**
 * This method returns the html code for the autodoc page
 * for the specified file.
 * @param file the file to process
 * @see /handlers/autodoc/autodoc_handler
 */
string create_html_docs( string file ) {
    int i;
    string *bits;
    string name, ret, tmp;

    file_name = file;

    name = AUTODOC_SAVE_DIR + replace_string( file_name, "/", ".");
    if( name[<2..] == ".c" )
        name = name[0..<3];

    if( !unguarded( (: file_exists( $(name)+".o" ) :) ) )
        return "Sorry, there is no documentation available for "+file+".";

    unguarded( (: restore_object( $(name) ) :) );

    if( !main_docs )
        main_docs = ([ ]);
    if( !public_functions )
        public_functions = ([ ]);
    if( !protected_functions )
        protected_functions = ([ ]);
    if( !inherits )
        inherits = ([ ]);
    if( !define_docs )
        define_docs = ([ ]);
    if( !includes )
        includes = ([ ]);
    if( !class_docs )
        class_docs = ([ ]);

    /*
     * Ok, we do the alphabetised list, first a short list with the
     * first sentence of each description, then a longer expanded list..
     */
    bits = explode( file_name, "/" );
    name = bits[sizeof(bits)-1];

    ret = "<html><head><title>File "+name+"</title></head><body "+COLOURS+">";
    ret += "[ <a href=\"autodoc/index.html\">Package Index</a></code> | ";
    ret += "<a href=\"autodoc/index_std.html\">Mudlib Index</a></code> | ";
    ret += "<a href=\"autodoc/index_eff.html\">Effect Index</a></code> ]<br>";

    if( main_docs["main"]) {
        ret += "<h2>File "+file_name+"</h2>"+implode( main_docs["main"], " ");
        if( main_docs["see"] )
            ret += "<p><b>See also:</b><br>"+query_multiple_short( map(
                   main_docs["see"], (: create_href($1) :) ) );

        if( main_docs["author"] )
            ret += "<p>Written by "+query_multiple_short(main_docs["author"]);

        if( main_docs["started"] )
            ret += "<p>Started "+query_multiple_short(main_docs["started"]);

        if( main_docs["example"] ) {
            if( sizeof( main_docs["example"] ) > 1 ) {
                for( i = 0; i < sizeof( main_docs["example"] ); i++ ) {
                    ret += "<h3>Example "+( i + 1 )+"</h3>\n";
                    ret += "<pre>"+main_docs["example"][i]+"</pre>\n";
                }
            } else {
                ret += "<h3>Example</h3>\n<pre>"+main_docs["example"][0]+
                       "</pre>\n";
            }
        }

        if( main_docs["changed"] ) {
             ret += "<h3>Change history</h3>\n<dl>"+implode( map(
                    main_docs["changed"], function( string str ) {
                    string start, rest;
                        if( sscanf( str, "%s\n%s", start, rest ) == 2 )
                            return "<dt>"+start+"<dd>"+rest;
                        return "<dd>"+str;
                    } ), "\n")+"</dl>\n";
        }
    }

    if( sizeof( inherits ) ) {
        ret += "<hr><h2>Inherits</h2>\n";
        ret += "This class inherits the following classes "+
               query_multiple_short( map( keys( inherits ),
               (: create_href($1) :) ) );
    }

    if( sizeof( includes ) ) {
        ret += "<hr><h2>Includes</h2>\n";
        ret += "This class includes the following files "+
               query_multiple_short( map( keys( includes ),
               (: create_href($1) :) ) );
    }

    if( sizeof( class_docs ) ) {
        string short_desc;

        bits = sort_array( keys(class_docs), (: strcasecmp :) );
        ret += "<hr><h2>Class Index</h2>\n<dl>";

        foreach( tmp in bits ) {
            ret += "<dt><img src=\"autodoc/images/cyan-ball-small.gif\" "
                    "height=6 width=6 alt=\" o \">\n";
            ret += "<a href=\"#class_"+tmp+"\"><b>"+tmp+"</b></a>\n";
            short_desc = query_short_desc( class_docs[tmp][AUTO_CLASS_DOCS] );
            if( short_desc )
                ret += "<dd>" + short_desc + "\n";
        }
        ret += "</dl>";
    }

    /*
     * Create a summary list of the methods at the top of the file.
     * with pointers to a more indepth explantion.
     */
    if( sizeof( bits = keys( public_functions + protected_functions ) ) ) {
        string short_desc;

        ret += "<hr><h2>Method index</h2>\n<dl>";
        bits = sort_array( bits, (: strcasecmp :) );

        foreach( tmp in bits ) {
            if( public_functions[tmp] )
                i = 0;
            else if( protected_functions[tmp] )
                i = 1;
            ret += "<dt><img src=\"autodoc/images/"+( i ? "magenta" : "cyan")+
                   "-ball-small.gif\" height=6 width=6 alt=\" o \">\n";
            ret += "<a href=\"#"+tmp+"\"><b>"+tmp+"</b></a>"+
                   query_short_args_def( ( i ? protected_functions :
                   public_functions )[tmp][AUTO_ARGS] );
            short_desc = query_short_desc( ( i ? protected_functions :
                   public_functions )[tmp][AUTO_DOCS] );
            if( short_desc )
                ret += "<dd>"+short_desc+"\n";
        }
        ret += "</dl>";
    }

    /*
     * Create the more indepth explanation.
     */
    if( sizeof( public_functions ) ) {
        ret += "<hr><h2>Public Functions</h2>\n";
        ret += "These are functions that everyone can access.<p><dl>\n.";
        ret += query_methods( 0 )+"</dl>\n";
    }

    if( sizeof( protected_functions ) ) {
        ret += "<hr><h2>Protected Functions</h2>\n";
        ret += "These are functions that only objects inheriting the "
                "class can access.<p>\n";
        ret += query_methods( 1 )+"\n";
    }

    /* If it is an include file.  Do fun stuff with defines. */
    if( sscanf( name, "%*s.h") == 1 && sizeof( define_docs ) ) {
        bits = sort_array( keys(define_docs), (: strcasecmp( $1, $2 ) :) );
        ret += "<hr><h2>Defines</h2>\n";
        foreach( tmp in bits ) {
            ret += "<dt><a name=\""+tmp+"\">\n";
            ret += "<img src=\"autodoc/images/red-ball.gif\" width=12 height=12 "
                   "alt=\" * \"><b>"+tmp+"</b></a>\n";
            if( define_docs[tmp]["main"] )
                ret += query_method_description( define_docs[tmp] );
        }
    }

    if( sizeof( class_docs ) ) {
        ret += "<hr><h2>Classes</h2>\n";
        ret += "These are nice data types for dealing with...  Data!<p>\n";
        bits = sort_array( keys(class_docs), (: strcasecmp :) );
        ret += "<dl>\n";
        foreach( tmp in bits ) {
            ret += "<dt><a name=\"class_"+tmp+"\">\n";
            ret += "<img src=\"autodoc/images/cyan-ball.gif\" width=12 height=12 "
                   "alt=\" * \"><b>"+tmp+"</b></a><pre>\n"+
                   query_class_def( class_docs[tmp], tmp )+"\n</pre>";
            if( class_docs[tmp][AUTO_CLASS_DOCS]["main"] )
                ret += query_class_description( class_docs[tmp][AUTO_CLASS_DOCS] );
            ret += "\n";
        }
        ret += "</dl>\n";
    }

    setup();
    return ret + "</body></html>";

} /* create_html_docs() */
