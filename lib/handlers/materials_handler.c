/**
 * Blah, standard header, hopefully something good. :(
 * Fill this out when I get something actually working.
 */

#define MAT_DIR "/std/artisans/materials/"

nosave mapping material_down;
nosave mapping material_values;

void create() {
  material_down = ([ "leather" : MAT_DIR+"leather.c" ]);
  material_values = ([ "leather" : 10 ]); //Units value per unit weight: may need changing.
}

string query_material_down(string material) {
  return material_down[material];
}

mapping query_materials_down() { //This would be needed when?
  return material_down;
}

int query_material_value(string material) {
  return material_values[material];
}

mapping query_materials_value() { //See query_materials_down().
  return material_values;
}

//Ok, you think of a better name! :)
mapping query_create_object_mapping(mapping material_amounts, object who) {
  object *inventory;
  object ob;
  string material;
  int amount;
  mapping objects = ([]);
  inventory = all_inventory(who);
  foreach(ob in inventory) {
    foreach(material in keys(material_amounts)) {
      if(ob->query_material()[material]) {
        amount = (ob->query_weight() * ob->query_material()[material]) / 100; //Percent * weight.
        if(amount >= material_amounts[material]) {
          objects += ([ ob : material_amounts[material] ]);
          map_delete(material_amounts, material); //Fullfilled.
        }
        else {
          objects += ([ ob : ob->query_weight() ]);
          material_amounts[material] -= amount;
        }
      continue;
      }
    }
  }
  if(sizeof(keys(material_amounts)) < 1) {
    objects += ([ "SUCCESS" : 1 ]);
  }
  else {
    objects += ([ "SUCCESS" : 0 ]);
  }
  return objects;
}