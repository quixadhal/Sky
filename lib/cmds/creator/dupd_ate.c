/* dupdate command by wodan */

inherit COMMAND_BASE;

#include <creator.h>

string other;

int do_it( string str ) {
  object ob, *obs, env;
  string *all, *dil, file, fnob;
  mixed static_arg, dynamic_arg;
  int clone;

  obs = WIZ_PRESENT->wiz_present( str, TP );

  if( !sizeof(obs) )
      return 0;

  foreach( ob in obs ) {
    env = ENV(ob);
    if( clone = clonep(ob) ) {
        static_arg = ob->query_static_auto_load();
        dynamic_arg = ob->query_dynamic_auto_load();
    }
    fnob = ( clone ? base_name(ob) : file_name(ob) );
    all = inherit_list(ob);
    foreach( file in all ) {
      if( other ) {
          dil = deep_inherit_list( find_object( file ) );
          if( !( ( member_array( other, dil ) != -1 ) || other == file ) )
              continue;
      }
      do_it( file );
    }
    ob->dest_me();
    if( ob )
        ob->dwep();
    if( ob )
        destruct(ob);

    ob = ( clone ? clone_object(fnob) : load_object(fnob) );

    if( !ob ) {
        printf("Could not load "+fnob+".\n");
        return 1;
    }

    if( static_arg )
        ob->init_static_arg( static_arg );
    if( dynamic_arg )
        ob->init_dynamic_arg( dynamic_arg );
    if( env )
        ob->move(env);
    printf("Updated %s.\n", WIZ_PRESENT->desc_f_object( ob ) );

  }

  return 1;

} /* do_it() */

int cmd( string str ) {
  object *tmp;

  other = 0;
  notify_fail("usage: dupdate <object> [<object>]\n");
  if( !str )
      return 0;

  sscanf( str, "%s %s", str, other );

  if( other ) {
      tmp = WIZ_PRESENT->wiz_present( other, TP );
      if( sizeof(tmp) )
          other = file_name( tmp[0] )+".c";
      else
          return 0;
  }

  return do_it(str);

} /* cmd() */
