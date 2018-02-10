
#include <housing.h>
#include <login_handler.h>

inherit COMMAND_BASE;

private string get_file(string fname) {
  string *tmp;
  
  tmp = this_player()->get_files(fname);
  if(sizeof(tmp) != 1) {
    tmp = this_player()->get_files(fname + ".c");
    if(sizeof(tmp) != 1)
      return "";
  }
  return tmp[0][0..<3];
}

int do_remove(string house) {
  house = get_file(house);
  
  if(!HOUSING->query_house(house))
    return notify_fail("No such house.\n");
  
  HOUSING->remove_house(house);
  write("House removed.\n");
  return 1;
}

int do_add(string house, string region, int rented, string address) {
  house = get_file(house);
  
  if(house == "")
    return notify_fail("House does not exist.\n");
  
  HOUSING->add_house(house, ({ }), address, region, rented);
  write("House added.\n");
  return 1;
}

int do_modify(string house, string region, int rented, string address) {
  string *rooms;
  
  house = get_file(house);
  if(house == "")
    return notify_fail("House does not exist.\n");
  if(!HOUSING->query_house(house))
    return notify_fail("That house is not registered.\n");

  rooms = HOUSING->query_rooms(house) - ({ house });
  HOUSING->modify_house(house, rooms, address, region, rented);
  write("House modified.\n");
  return 1;
}

int add_room(string house, string room) {
  string *files, file;
  object ob;
  
  house = get_file(house);
  if(!HOUSING->query_house(house))
    return notify_fail("No such house.\n");
  
  files = this_player()->get_files(room);

  if(!sizeof(files))
    return notify_fail("That room doesn't exist.\n");
  
  foreach(file in files) {
    room = file[0..<3];
    if(room == house)
      continue;
    
    house->force_load();
    ob = find_object(house);
    if(!ob || !function_exists("query_owner", ob))
       continue;

    if(!HOUSING->add_rooms(house, ({ room })))
      write("Error adding room " + room + ".\n");
    else
      write("Room added [" + room + "]\n");
  }
  return 1;
}

int remove_room(string house, string room) {
  house = get_file(house);
  if(!HOUSING->query_house(house))
    return notify_fail("No such house.\n");

  if(!HOUSING->remove_rooms(house, ({ room })))
    return notify_fail("Error removing room.\n");

  write("Room removed.\n");
  return 1;
}

int do_owner(string house, string owner) {
  if(owner == "none")
    owner = "Vacant";
  
  if(owner != "Vacant" && !PLAYER_HANDLER->test_user(owner))
    return notify_fail("No such user " + owner + ".\n");

  house = get_file(house);

  if(!HOUSING->set_owner(house, owner))
     return notify_fail("Error changing owner.\n");

  write("Owner set to " + owner + ".\n");
  return 1;
}

int do_list(string search) {
  string house;

  if(search) {
    house = get_file(search);
    if(HOUSING->query_house(house)) {
      printf("House: %s\n  Owner: %s\n  Address: %s\n  Region: %s\n  "
             "Type: %s\n  Value: %d\n  Rooms: %s\n",
             house,
             HOUSING->query_owner(house),
             HOUSING->query_address(house),
             HOUSING->query_region(house),
             (HOUSING->query_type(house) ? "Rented" : "Owned"),
             HOUSING->query_value(house),
             implode(HOUSING->query_rooms(house), ", "));
      return 1;
    }
  }
    
  foreach(house in keys(HOUSING->query_houses())) {
    if(!search ||
       (strsrch(house, search) != -1 ||
        (HOUSING->query_owner(house) &&
         strsrch(HOUSING->query_owner(house), search) != -1))) {
      printf("%s - %s: %s. %d rooms.\n",
             house,
             (HOUSING->query_type(house) ? "Renter" : "Owner"),
             HOUSING->query_owner(house),
             sizeof(HOUSING->query_rooms(house)));
    }
  }
  return 1;
}

mixed *query_patterns() {
  return ({ "list", (: do_list(0) :),
              "list <word'search'>", (: do_list($4[0]) :), 
              "add <word'house'> <word'region'> <number'rented'> "
              "<string'address'>", (: do_add($4[0], $4[1], $4[2], $4[3]) :),
              "modify <word'house'> <word'region'> <number'rented'> "
              "<string'address'>", (: do_modify($4[0], $4[1], $4[2], $4[3]) :),
              "add room <word'house'> <string'room'>",
              (: add_room($4[0], $4[1]) :),
              "remove room <word'house'> <string'room'>",
              (: remove_room($4[0], $4[1]) :),
              "owner <word'house'> <word'owner'>",
              (: do_owner($4[0], $4[1]) :),
              "remove <word'house'>", (: do_remove($4[0]) :),
              });
}
