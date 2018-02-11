/* command to update the TYPOS file
 *
 * Shaydz - 30 Mar 2000
 * based loosely on changes command.
 */

inherit COMMAND_BASE;

#define CHANGEFILE "/d/mudlib/changes/TYPOS"

varargs string wrap(string str, int wrap_column, int indent) {

  if (!wrap_column)
    wrap_column = 79;

  return terminal_colour(str, ([]), wrap_column, indent);
} /* wrap() */

int cmd(string args) {
    string filename, comment, time, date, prefix, inform_comment;

    notify_fail("Usage: typolog <filename> <comment>\n"
                "       typolog tail\n"
                "       typolog list\n");

    if( !args ){
        return 0;
    }

    if( args == "tail" ){
        "/secure/cmds/creator/ta_il.c"->cmd(CHANGEFILE);
        return 1;
    }

    if( args == "list" ){
        TP -> more_file( CHANGEFILE );
        return 1;
    }

    if (sscanf(args, "%s %s", filename, comment) != 2){
        return 0;
    }

      filename = master()->make_path_absolute(filename);

      if (!file_exists(filename))
          return notify_fail("File: '"+ filename +"' does not exist.\n");


    time = ctime(time());
    date = time[8..10] + time[4..7] +"'"+ time[22..23];
    prefix = TP -> query_cap_name()+" - "+date;
    comment = filename +" - "+comment;
    inform_comment = comment;

    if (strlen(comment) <= 78) {
        comment = comment +"\n";
    } else
        comment = wrap(comment, 78, 4) +"\n";

    unguarded((: write_file, CHANGEFILE,comment+"    "+prefix+"\n", 0 :));

    tell_object(TP,CHANGEFILE+" updated.\n");
    event(users(),"inform",TP -> query_cap_name() +": "+ inform_comment+" (Typo)","typolog");

    return 1;
}
