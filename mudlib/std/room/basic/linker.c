#include <room.h>

nosave int av_flag, ad_flag, rm_flag;

string d_prep, s_prep, r_name, *rooms;

object room;

void setup_shadow(object thing, string *words, string word1,
  string word2, string word3) {
  shadow(thing, 1);
  room = thing;
  rooms = words;
  if(!word1) {
    d_prep = "into";
  } else {
    d_prep = word1;
  }
  if(!word2) {
    s_prep = "in";
  } else {
    s_prep = word2;
  }
  r_name = word3;
} /* setup_shadow() */

void destruct_shadow(object thing) {
  if(thing == TO) {
    destruct(TO);
  } else {
    thing->destruct_shadow(thing);
  }
} /* destruct_shadow() */

string *query_rooms() {
  return rooms;
} /* query_rooms() */

string query_dynamic_preposition() {
  return d_prep;
} /* query_dynamic_preposition() */

string query_static_preposition() {
  return s_prep;
} /* query_static_preposition() */

string query_room_name() {
  return r_name;
} /* query_room_name() */

string area_name() {
  string *bits;
  if(r_name) {
    return r_name;
  }
  bits = explode((string)room->query_short(), " of ");
  if(sizeof(bits) > 1) {
    return bits[ sizeof( bits ) - 1];
  }
  return "the area";
} /* area_name() */

string compose_start() {
  if(s_prep == "") {
    return CAP((string)room->the_short());
  }
  return CAP(s_prep)+" "+(string)room->the_short();
} /* compose_start() */

void event_person_say( object person, string start, string rest,
                       string language ) {
   int i;
   if ( start[ 0 .. 3 ] == "the " )
      start = explode( (string)person->a_short(), " " )[ 0 ] +
            start[ 3 .. strlen( start ) - 1 ];
   for ( i = 0; i < sizeof( rooms ); i++ )
      if ( find_object( rooms[ i ] ) )
         INV( find_object( rooms[ i ] ) )->event_person_say( person,
                compose_start() +", "+ start, rest, language );
    room->event_person_say( person, start, rest, language );
} /* event_person_say() */

void event_exit(object person, string message, object to) {
  int ad_flag, rm_flag, i;
  string arrive, direc, *exits, link_mess, *link_array;

  room->event_exit(person, message, to);
  if(!message) {
    return;
  }
  arrive = "";
  exits = (string *)room->query_dest_dir();
  for(i = 1; i < sizeof(exits); i += 2) {
    if(to == find_object(exits[i])) {
      direc = exits[i - 1];
      ad_flag = 1;
    }
  }
  if(arrayp(room->query_dest_other())) {
    if(member_array(direc, room->query_dest_other()) != -1) {
      i = member_array(direc, room->query_dest_other());
      link_array = room->query_dest_other()[i + 1][11];
    }
  }
  if(arrayp(link_array)) {
    link_mess = link_array[0];
    link_mess = replace(link_mess, ({
      "$N", person->a_short(),
      "$A", area_name(),
      "$D", direc,
      "$P", query_dynamic_preposition(),
      "$S", query_static_preposition(),}));
  }
  if(link_mess) {
    for(i = 0; i < sizeof(rooms); i++) {
      if(to != find_object(rooms[i])) {
        tell_room(find_object(rooms[i]), link_mess);
      }
    }
    return;
  }
  for(i = 0; i < sizeof(rooms); i++) {
    if(to == find_object(rooms[i])) {
      arrive = (string)to->query_dynamic_preposition()+" "+
        (string)to->the_short();
      rm_flag = 1;
    }
  }
  if(ad_flag) {
    message = (string)person->a_short()+" $V$0=moves,move$V$ "+direc;
    message += " "+arrive;
    if(rm_flag) {
      message += ".\n";
    } else {
      message += "and $V$0=leaves,leave$V$ "+ area_name() +".\n";
    }
  } else {
    message = (string)person->a_short() +" $V$0=teleports,teleport$V$";
    if(person->query_property("npc"))
       return;
    if(rm_flag) {
      message += " "+ arrive +".\n";
    } else {
      message += " out of "+area_name()+".\n";
    }
  }
  for(i = 0; i < sizeof(rooms); i++) {
    if(to != find_object(rooms[i])) {
      tell_room(find_object(rooms[i]), message);
    }
  }
} /* event_exit() */

void event_enter( object person, string message, object from ) {
  int ad_flag, rm_flag, i;
  string direc, *exits, link_mess, *link_array;

  room->event_enter(person, message, from);
  if(!message || !from) {
    return;
  }
  for(i = 0; i < sizeof(rooms); i++) {
    if(from == find_object(rooms[i])) {
      rm_flag = 1;
    }
  }
  if(rm_flag) {
    return;
  }
  exits = (string *)room->query_dest_dir();
  for(i = 1; i < sizeof(exits); i += 2) {
    if(from == find_object(exits[i])) {
      direc = exits[i - 1];
      ad_flag = 1;
    }
  }
  if(member_array(direc, room->query_dest_other()) != -1) {
    i = member_array(direc, room->query_dest_other());
    link_array = room->query_dest_other()[i + 1][11];
  }
  if(arrayp(link_array)) {
    link_mess = link_array[1];
    link_mess = replace(link_mess, ({
      "$N", person->a_short(),
      "$A", area_name(),
      "$D", direc,
      "$P", query_dynamic_preposition(),
      "$S", query_static_preposition(),}));
  }
  if(link_mess) {
    for(i = 0; i < sizeof(rooms); i++) {
      tell_room(find_object(rooms[i]), link_mess);
    }
    return;
  }

  if(ad_flag) {
    message = (string)person->a_short()+" $V$0=moves,move$V$ into "+
      area_name()+" from the "+direc+".\n";
  } else {
    message = (string)person->a_short()+
      " $V$0=teleports,teleport$V$ into "+area_name() +".\n";
  }
  for(i = 0; i < sizeof(rooms); i++) {
    tell_room(find_object(rooms[i] ), message);
  }
} /* event_enter() */

int query_linked() { return 1; }
