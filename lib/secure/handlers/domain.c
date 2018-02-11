// Domain handler (/secure/handler/domain.c)
// Coded by Donky - 25/09/96
// Converted and redone for here by Shaydz 08/10/00

// WARNING: _Never_ edit this file, edit it under a different filename
//          and make sure it updates before replacing the old version
//          otherwise _IT CAN ONLY BE FIXED FROM THE SHELL_

#include "access.h"

inherit "/secure/std/modules/group";

#define RANK_LOG "/secure/log/RANK"
#define SAVE_FILE "/secure/save/handlers/domain"
#define SECURITY geteuid(PO)

void load_me();
void save_me();

nosave string *leaders;
nosave string *seniors;

private void create() {
    string group;
    leaders = ({ });
    seniors = ({ });
    seteuid(master()->creator_file(file_name(TO)));

    group::mudlib_setup();

    load_me();
    foreach(group in query_groups(0)) {
        leaders += query_leaders(group);
        seniors += query_seniors(group);
    }
    leaders = uniq_array(leaders);
    seniors = uniq_array(seniors);
} /* setup() */

void load_me() {
    unguarded((: restore_object , SAVE_FILE :));
} /* load_me() */

void save_me() {
    unguarded((: save_object, SAVE_FILE :));
} /* save_me() */

// CREATOR: --------------------------------------

// determine whether a player is a creator
// determine whether a creator is a member of a domain
varargs int query_creator(string domain, string name) {
    return group::query_member(domain, name);
}

// list all creators
// list all the creators who belong to a domain
varargs string *query_creators(string domain) {
    return copy(group::query_members(domain));
}

string add_creator(string name) {
    object ob;
    string file, time;

    if( !PRIVILEGE_H->query_privilege("creator.add", SECURITY) ) {
        log_file("$SECURITY", "NOTE (Privilege - %s): promote %s.\n",
            SECURITY, name );
        return "Insufficient privilege to promote.";
    }

    if( !stringp(name) )
        return "Invalid name.";

    ob = find_player(name);

    if( ob && ob->query_property("guest") )
        return "You cannot promote guests.";

    if( !ob && !PLAYER_H->test_user(name) )
        return "User does not exist.";

    if( query_creator(name) )
        return CAP(name) +" is already a creator.";

    if( file_size("/save/creators/"+name+".o") > 0 )
        return "Player file has already been backed up.";

    if( ob )
        ob->save();

    if( file_size("/save/players/"+name[0..0]+"/"+name+".o") < 1 )
        return CAP(name)+" doesn't have a save file!";

    unguarded( (: cp, "/save/players/"+name[0..0]+"/"+name+".o",
        "/save/creators/"+name+".o" :) );

    if( ob ) {
        ob->set_creator(1); // set_creator() saves as well.
        tell_object( ob, "You have been promoted by "+
            CAP(  TP->query_name() ) +".\n");
        tell_object( ob, "Quit and log back in to get the "
          "creator commands.\n");
        tell_object( ob, "Be sure to check \"help "
          "creator start\" when you log back in, as it contains valuable "
          "and useful information.\n");
    } else {
        unguarded( (: write_file, "/save/players/"+name[0..0]+"/"+name+".o",
            "creator 1\nhome_dir \"/w/"+name+"\"\n" :) );
    }

    if( unguarded( (: file_size, CREATOR_DIR "/"+ name :) ) == -1 ) {
        tell_object( TP, "Creating directory entry...\n\n");

        // build standard creator directory structure
        file = unguarded( (: read_file, "/std/creator/defs_standard.h" :) );
        unguarded( (: mkdir, CREATOR_DIR "/"+ name :) );
        unguarded( (: mkdir, CREATOR_DIR "/"+ name +"/weapon" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/weapon/defs.h",
            file :) );

        unguarded( (: mkdir, CREATOR_DIR "/"+ name +"/item" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/item/defs.h",
            file :) );

        unguarded( (: mkdir, CREATOR_DIR "/"+ name +"/armour" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/armour/defs.h",
            file :) );

        unguarded( (: mkdir, CREATOR_DIR "/"+ name +"/clothes" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/clothes/defs.h",
            file :) );

        unguarded( (: mkdir, CREATOR_DIR "/"+ name +"/npc" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/npc/defs.h",
            file :) );

        unguarded( (: mkdir, CREATOR_DIR "/"+ name +"/room" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/room/defs.h",
            file :) );

        unguarded( (: mkdir, CREATOR_DIR "/"+ name +"/secure" :) );
        unguarded( (: mkdir, CREATOR_DIR "/"+ name +"/text" :) );

        file = unguarded( (: read_file, "/std/creator/defs.h" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/defs.h",
            "#define CREATOR   \""+ name +"\"\n" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/defs.h",
            "#define HOME      \"/w/"+ name +"/\"\n" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/defs.h",
            file :) );

        file = unguarded( (: read_file, "/std/creator/workroom.c" :) );
        unguarded( (: write_file, CREATOR_DIR "/"+ name +"/workroom.c",
            file :) );
    } else
        tell_object( TP, "Creator directory already exists.\n\n");

    unguarded( (: write_file, "/log/EMPLOYMENT",
        sprintf( "%s: %s employed by %s\n", ctime( time() ), name,
          (string)TP->query_name() ) :) );

    write( CAP( name )+" promoted.\n");

    if( group::create_member(name) ) {
        time = ctime(time());
        log_file(RANK_LOG, "%s '%s: %s promoted by %s.\n",
            time[4..9], time[22..23], CAP(name), CAP(SECURITY));
        if( ob ) {
            ob->add_property("intercre_off", 1 );
            ob->add_property("intergossip_off", 1 );
        }
        save_me();
        return 0;
    }

    return "Unable to promote, please inform a member of the admin domain";

} /* add_creator() */

string remove_creator( string str ) {
    mapping mine;
    string time, name, reason;
    int last_log_on, time_on;
    object ob;

    if( !PRIVILEGE_H->query_privilege("creator.remove", SECURITY ) ) {
        log_file("SECURITY", sprintf("NOTE (Privilege - %s): demote %s.\n",
            SECURITY, name ) );
        return "Insufficient privilege to demote.";
    }

    if( sscanf(str, "%s %s", name, reason) != 2 ) {
        return "You need to give a reason.";
    }

    if( !stringp(name) )
        return "Invalid name.";

    if( !query_creator(name) )
        return capitalize(name)+" is not a creator.";

    if( lordp(name) && !adminp(TP) ) {
        return "You cannot demote Lords.";
    }

    if( catch( last_log_on = PLAYER_H->test_last(name) ) ||
        catch( time_on = PLAYER_H->test_age(name) ) )
        return "You cannot demote anyone while the player handler is broken.";

    PLAYER_H->delete_from_cache(name);

    unguarded( (: write_file, "/log/DISMISSALS",
        sprintf( "%s: %s demoted by %s\nreason: %s\n",
            ctime( time() ), name, (string)TP->query_name(), reason ) :) );

    write( CAP( name )+" dismissed.\n");

    if( ob = find_player(name) ) {
        ob->set_creator(0);
        ob->save();
        tell_object( ob, "You have just been demoted by "+
            CAP( (string)TP->query_name() )+".\n");
        ob->quit();
    } else if( file_exists("/save/players/"+name[0..0]+"/"+name+".o") ) {
        if( name == "default")
            unguarded( (: rm, "/save/players/"+name[0..0]+"/"+name+".o" :) );
        else
            unguarded( (: write_file, "/save/players/"+name[0..0]+"/"+name+".o",
                "creator 0\nhome_dir 0\n" :) );
    }

    if( file_exists("/save/creators/"+name+".o") ) {
        unguarded( (: write_file, "/save/creators/"+name+".o",
            "last_log_on "+last_log_on+"\n"
            "time_on "+time_on+"\n" :) );
        unguarded( (: cp, "/save/creators/"+name+".o",
            "/save/players/"+name[0..0]+"/"+name+".o" :) );
        unguarded((: rm, "/save/creators/"+name+".o" :) );
    }

    if( file_size("/w/"+name) == -2 && file_size("/w/.old_creators/"+name ) != -2 )
        unguarded( (: rename, "/w/"+name, "/w/.old_creators/"+name :) );

    PROJECT_H->remove_member(name);

    mine = ([ ]);

    foreach( string path, mapping people in master()->query_permissions() )
        if( people[name] )
            mine[path] = people[name];

    if( sizeof(mine) )
        map( mine, (: master()->remove_permission( $(name), $1, $2 ) :) );

    if( group::query_senior(name) )
        map( filter( group::query_groups(),
            (: group::query_senior( $1, $(name) ) :) ),
            (: group::remove_senior( $1, $(name) ) :) );

    if( group::delete_member(name) ) {
        time = ctime(time());
        log_file( RANK_LOG, "%s '%s: %s demoted by %s.\n", time[4..9],
            time[22..23], CAP(name), ( SECURITY == "Root" ? "retiring" :
            CAP(SECURITY) ) );

        leaders = filter( leaders, (: $1 != $2 :), name );
        seniors = filter( seniors, (: $1 != $2 :), name );

        save_me();
        return 0;
    }

    return "Unable to demote, please inform a member of the admin domain.";

} /* remove_creator() */

// DOMAIN: --------------------------------------
int query_domain(string name) {
    return group::query_group(name);
} /* query_domain() */

string *query_domains(string name) {
    return copy(group::query_groups(name));
} /* query_domains() */

string add_domain(string name) {
    int i;

    if (!PRIVILEGE_H -> query_privilege("domain.add", SECURITY))
        return "Insufficient privilege to create a domain";

    if (i = group::add_group(name))
        save_me();

    return (i ? 0 : "Unable to create domain");
} /* add_domain() */

string remove_domain(string name) {
    int i;

    if (!PRIVILEGE_H -> query_privilege("domain.remove", SECURITY))
        return "Insufficient privilege to delete a domain";

    if (i = group::remove_group(name))
        save_me();

    return (i ? 0 : "Unable to delete a domain");
} /* remove_domain() */

// SENIOR: --------------------------------------

varargs int query_senior(string domain, string name) {
    return group::query_senior(domain, name);
} /* query_senior() */

string *query_seniors(string domain) {
    if (!stringp(domain))
        return copy(seniors);

    return copy(group::query_seniors(domain));
} /* query_seniors() */

mixed add_senior(string domain, string name) {
    if (!PRIVILEGE_H -> query_privilege("domain."+ domain +".senior.add",
        SECURITY))
        return "Insufficient privilege to add a domain senior";

    if (group::add_senior(domain, name)) {
        if (member_array(name, seniors) == -1)
            seniors += ({ name });

        save_me();
        return 0;
    }

    return "Unable to add a domain senior";
} /* add_senior() */

mixed remove_senior(string domain, string name) {
    if (!PRIVILEGE_H -> query_privilege("domain."+ domain +".senior.remove",
        SECURITY))
        return "Insufficient privilege to remove a domain senior";

    if (group::remove_senior(domain, name)) {
        if (!query_senior(name))
            seniors -= ({ name });

        save_me();
        return 0;
    }

    return "Unable to remove a domain senior";
} /* remove_senior() */

// LEADER: --------------------------------------

varargs int query_leader(string domain, string name) {
    return group::query_leader(domain, name);
} /* query_leader() */

string *query_leaders(string domain) {
    if (!stringp(domain))
        return copy(leaders);

    return copy(group::query_leaders(domain));
} /* query_leaders() */

mixed add_leader(string domain, string name) {
    if (!PRIVILEGE_H -> query_privilege("domain."+ domain +".leader.add",
        SECURITY))
        return "Insufficient privilege to add a domain leader";

    if (group::add_leader(domain, name)) {
        if (member_array(name, leaders) == -1)
            leaders += ({ name });

        save_me();
        return 0;
    }

    return "Unable to add a domain leader";
} /* add_leader() */

mixed remove_leader(string domain, string name) {
    if (!PRIVILEGE_H -> query_privilege("domain."+ domain +".leader.remove",
        SECURITY))
        return "Insufficient privilege to remove a domain leader";

    if (group::remove_leader(domain, name)) {
        if (!query_leader(name))
            leaders -= ({ name });

        save_me();
        return 0;
    }

    return "Unable to remove a domain leader";
} /* remove_leader() */

// MEMBER: --------------------------------------

int query_member(string domain, string name) {
    if (!stringp(domain) || !stringp(name))
        return 0;

    return group::query_member(domain, name);
} /* query_member() */

string *query_members(string domain) {
    string *arr;

    if (!stringp(domain))
        return ({ });

    if (!arrayp(arr = copy(group::query_members(domain))))
        return ({ });

    return arr;
} /* query_members() */

mixed add_member(string domain, string name) {
    int i;

    if (!PRIVILEGE_H -> query_privilege("domain."+ domain +".member.add",
        SECURITY))
        return "Insufficient privilege to add "+ capitalize(name) +" to the "+
        domain +" domain";

    if (i = group::add_member(domain, name))
        save_me();

    return (i ? 0 : "Unable to add "+ capitalize(name) +" to the "+
      domain +" domain");
} /* add_member() */

mixed remove_member(string domain, string name) {
    int i;

    if (!PRIVILEGE_H -> query_privilege("domain."+ domain +".member.remove",
        SECURITY))
        return "Insufficient privilege to remove "+ capitalize(name) +" from "
        "the "+ domain +" domain";

    if (i = group::remove_member(domain, name))
        save_me();

    return (i ? 0 : "Unable to remove "+ capitalize(name) +" from the "+ domain +
      " domain");
} /* remove_member() */

// Error reports: return creator(s) to receive them

mixed log_who(string domain, string file) {
    return query_leaders(domain);
}

varargs string query_short(string domain, string user) {
    int d, p, dl, pl;
    string ret, name;
    string *prjs, *dmns, *lprj = ({ }), *ldmn = ({ });

    if (stringp(domain) && !stringp(user)) {
        // Quick hack.
        return capitalize(domain);
    }

    if (!stringp(domain) && stringp(user)) {
        dmns = query_domains(user);
        prjs = PROJECT_H -> query_projects(user);

        foreach (name in dmns)
        if (query_leader(name, user))
            ldmn += ({ name });

        if (dl = sizeof(ldmn))
            dmns -= ldmn;

        foreach (name in prjs)
        if (PROJECT_H -> query_leader(name, user))
            lprj += ({ name });

        if (pl = sizeof(lprj))
            prjs -= lprj;

        d  = sizeof(dmns);
        p  = sizeof(prjs);

        ret = "";

        if (dl || pl)
            ret += "leader of the "+
            (dl ? query_multiple_short(ldmn) +" domain"+ (dl > 1 ? "s" : "") +
              (pl ? " and the " : "") : "") +
            (pl ? query_multiple_short(lprj) +" project"+
              (pl > 1 ? "s" : "") : "") + (d || p ? ", " : "");

        if (d || p)
            ret += "member of the "+
            (d ? query_multiple_short(dmns) +" domain"+ (d > 1 ? "s" : "") +
              (p ? " and the " : "") : "") +
            (p ? query_multiple_short(prjs) +" project"+ (p > 1 ? "s" : "") : "");

        return ret;
    }

    if (stringp(domain) && stringp(user)) {
        // return ctrs domain specific title
    }

    return 0;
}

// Storing the domain description in the handler turned unwieldy, so we simply
// read a file and return that.

string query_long(string name) {
    if (file_exists(DOMAIN_DIR "/"+ name +"/ABOUT"))
        return read_file(DOMAIN_DIR "/"+ name +"/ABOUT");
    else
        return "This domain hasn't been documented yet.  "
        "Please mail "+ query_multiple_short(query_leaders(name)) +
        " to get this fixed.\n";
} /* query_long */

nomask void dwep() {
  object my_ob = TO;

  if (my_ob)
    destruct(my_ob);
} /* dwep() */

void dest_me() { dwep(); }
