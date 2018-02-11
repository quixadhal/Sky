/* exits.c
 * Coded by Flode@fr ages ago, now also externalized - 250599
 */

inherit COMMAND_BASE;

string help()
{
    return "Usage: exits - outputs the status of all exits in your room.\n";
}

private string adjust_string_size(string str, int size)
{
    if(sizeof(str) <= size)
        return str;
    return("..." + str[sizeof(str)-(size-3)..sizeof(str)]);
}

int cmd(string str, object me)
{
    int i,j;
    string *dirs, *dirs2;
    mapping opposite = (["north":"south","south":"north","west":"east",
      "east":"west","northwest":"southeast",
      "southeast":"northwest","northeast":"southwest",
      "southwest":"northeast","up":"down","down":"up",
      "out":"in","in":"out"]);
    me = TP;
    if(!ENV(me))
    {
        notify_fail("You have no environment.\n");
        return 0;
    }
    dirs = (string *)ENV(me)->query_dest_dir();
    if(!sizeof(dirs))
    {
        write("There are no exits in this room.\n");
        return 1;
    }
    for(i=0; i<sizeof(dirs); i+=2)
    {
        string match;
        tell_object(me, sprintf("%-10s: ",dirs[i][0..8]));
        sscanf(dirs[i+1],"%s.c",dirs[i+1]);
        dirs[i+1] = dirs[i+1]+".c";
        write(adjust_string_size(dirs[i+1],67)+"\n            ");
        if(file_size(dirs[i+1]) == -1)
            tell_object(me, "The file "+adjust_string_size(dirs[i+1],42)+
              " does not exist.\n");
        else
        {
            if(catch(dirs2 = (string *)dirs[i+1]->query_dest_dir()))
                tell_object(me, adjust_string_size(dirs[i+1],66)+" won't load.\n");
            else
            {
                match = 0;
                if(sizeof(dirs2))
                    for(j=0; j<sizeof(dirs2); j=j+2)
                    {
                        sscanf(dirs2[j+1],"%s.c", dirs2[j+1]); 
                        if(file_name(ENV(me)) == dirs2[j+1])

                            match = dirs2[j];
                    }
                if(!match)
                    tell_object(me, "There is no exit from "+adjust_string_size(
                        dirs[i+1],30)+ " to this room.\n");
                else
                {
                    if(opposite[dirs[i]] == match)
                        tell_object(me, "Ok.\n");
                    else
                        tell_object(me, "The exit from "+adjust_string_size(dirs[i+1],24)+
                          " to this room is: "+match[0..8]+".\n");
                }
            }
        }
    }
    return 1;
}
