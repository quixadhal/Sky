/* ed command, trial out by Turrican for a commands daemon. */

#include <creator.h>

inherit COMMAND_BASE;

object *people = ({ });

mixed cmd(string str) {
  string *filenames, spam;
  object *things;

  people += ({ this_player() });

  if ( TP->query_editor() == "magic" )  {
    if ( !str )  str = "";
    else str = implode(explode(str, " "), " ");
    TP->do_edit( 0, "fini_editor", 0, str );
    return 1;
  }
  if (!str) {
    TP->set_in_editor("(hidden)");
    ed("frog", "fini_editor");
    return 1;
  }
  /* dodgy idea, but allows 'ed here' or 'ed strawberry' */        
  if (sizeof(things = WIZ_PRESENT->wiz_present(str, TP))) {
    spam = file_name(things[0]);
    sscanf(spam, "%s#%*d", spam);
    if (file_size(spam) < 0)
      filenames = ({ spam+".c" });
    else
      filenames = ({ spam });
  } else
    filenames = TP->get_files(str);
  if (!sizeof(filenames)) { 
    str = this_player()->get_path(str);
  } else {
    if (sizeof(filenames) > 0) {
      str = filenames[0];
      if (sizeof(filenames) > 1) {
        int loop;

        loop = 0;
        while(loop < sizeof(filenames) && file_size(filenames[loop]) < 0)
          loop++;
        if(loop >= sizeof(filenames)) {
          printf("No such file.\n");
          people -= ({ this_player() });
          return 0;
        }
        else {
          str = filenames[loop];
        } 
        printf("Ambiguous, using : %s\n", str);
      }
    }
  }
  if (file_size(str) == -2) {
    printf("directory\n");
    people -= ({ this_player() });
    return 0;
  }
  TP->set_in_editor(str);
  printf("Editing: %s ", str);
  log_file("EDITS",str+" edited by "+TP->query_name()+" at "+ctime(time())+"\n");
  
  if (!master()->valid_write(str, geteuid(), "frog"))
    printf("[read only]\n");
  else
    printf("\n");
  ed(str, "fini_editor");
  return 1;
}

void fini_editor() {
  if (objectp(TP))  {
    people -= ({ TP });
    TP->set_in_editor(0);
  }
} /* fini_editor() */


int clean_up()  {
   people -= ({ 0 });
   if ( !sizeof( people ) )  {
      ::clean_up();
   }
   return 1;
}

void reset()  {
   people -= ({ 0 });
   if ( !sizeof( people ) )  {
      ::reset();
   }
   return;
}

