class item_data {
    string skill;
    int    difficulty;
    int    upper;
    mixed  extra;
    string *season;
    mixed  quant;
    string ob;
}
void dest_me() { destruct( this_object() ); }

class item_data item;
mapping data = ([ ]);

mixed data_return_0() {
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/floppy_madman.ob";
  data["floppy madman"] = item;
  item = new( class item_data );
  item->skill = "";
  item->difficulty = 0;
  item->upper = 0;
  item->extra = 0;
  item->season = ({ "spring", "summer" });
  item->quant = (: random(4) + 5 :);
  item->ob = "/obj/plants/pink_clover.ob";
  data["pink clover"] = item;
  item = new( class item_data );
  item->season = ({ "spring" });
  item->quant = (: random(2) + 3 :);
  item->ob = "/std/plant";
  data["white clover"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn" });
  item->quant = (: random(6) + 20 :);
  item->ob = "/obj/plants/grass.ob";
  data["green grass"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/yarrow.ob";
  data["yarrow"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/comfrey.ob";
  data["comfrey"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/sage.ob";
  data["sage"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/parsley.ob";
  data["parsley"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/basil.ob";
  data["basil"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/rosemary.ob";
  data["rosemary"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/eyebright.ob";
  data["eyebright"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: 15 + random( 15 ) :);
  item->ob = "/obj/plants/fennel_seed.ob";
  data["fennel seeds"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: 15 + random( 15 ) :);
  item->ob = "/obj/plants/oregano.ob";
  data["oregano"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/plants/ivy_seed.ob";
  data["ivy seeds"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/plants/thyme.ob";
  data["thyme"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/plants/foxglove.ob";
  data["foxglove"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random( 3 ) + 3 :);
  item->ob = "/obj/plants/violet.ob";
  data["violets"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/stinking_mayweed.ob";
  data["stinking mayweed"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(3) + 3 :);
  item->ob = "/obj/plants/klatchian_fleabane.ob";
  data["Klatchian fleabane"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/plants/wild_garlic.ob";
  data["wild garlic"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/plants/convolvus.ob";
  data["convolvus"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(1)+3 :);
  item->ob = "/d/ram/Lancre_Kingdom/Blackglass/apples/golden_disagreeable.food";
  data["Golden Disagreeable apple"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(1)+2 :);
  item->ob = "/d/ram/Lancre_Kingdom/Blackglass/apples/blackheart_apple.food";
  data["Lancre Blackheart apple"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/d/ram/Lancre_Kingdom/Blackglass/apples/green_billet.food";
  data["Green Billet apple"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(1)+1 :);
  item->ob = "/d/ram/Lancre_Kingdom/Blackglass/apples/peachface_apple.food";
  data["Blackglass Peachface apple"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(1)+1 :);
  item->ob = "/d/ram/Lancre_Kingdom/Blackglass/apples/gammer_smith.food";
  data["Gammer Smith apple"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = (: random(1)+2 :);
  item->ob = "/d/ram/Lancre_Kingdom/Blackglass/apples/ogg_apple.food";
  data["Gytha Ogg apple"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/cabbage.food";
  data["cabbage"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/dirty_potato.food";
  data["potato"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/tomato.food";
  data["tomato"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/small_radish.food";
  data["small radish"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/celery.food";
  data["celery"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/carrot.food";
  data["carrot"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/small_onion.food";
  data["small onion"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/lettuce.food";
  data["lettuce"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/rhubarb.food";
  data["rhubarb"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/parsnip.food";
  data["parsnip"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/plants/foxglove.ob";
  data["foxglove"] = item;
  item = new( class item_data );
  item->season = ({ "spring", "summer", "autumn", "winter" });
  item->quant = 1;
  item->ob = "/obj/food/jelly_orange.food";
  data["jelly orange"] = item;
}

mixed data_return() {
  data_return_0();
  return data;
}
