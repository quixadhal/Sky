#include <reaction.h>
void dest_me() { destruct( this_object() ); }

class reaction item;
mapping data = ([ ]);

mixed data_return_0() {
  if (!mappingp(data["GroundDeadlyNightshade"])) data["GroundDeadlyNightshade"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 1.0/600, 1.0/1000 });
  item->result = "/obj/reagents/transient_liquid.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("coffee") :),
        (: $3->set_short("coffee") :),
        (: $3->set_long("This is some freshly made coffee, $amount_size$.\n")
            :),
        (: $3->set_progress_msg( ({
            ({0, 0}),
            ({0, 0}),
            ({"The liquid in the @@short:#env#@@ darkens slightly.\n", ({
                "set_medium_alias:#obj#|TransientCoffee"})}),
            ({0, 0}),
            ({0, 0})
            }) ) :),
        (: $3->set_medium_alias("PreTransientCoffee") :),
        (: $3->add_eat_effect("/std/effects/ingested/poison.c", 10) :),
        (: $3->set_duration(30) :),
        (: $3->set_final("morph") :),
        (: $3->set_result("/obj/reagents/poison_coffee.ob") :),
        (: $3->set_result_amt(1.0) :),
        (: $3->set_result_alias("PoisonCoffee") :),
        (: $3->set_continuous() :),
        (: $3->set_liquid() :),
        (: $3->start_reaction() :)
    });
  data["GroundDeadlyNightshade"]["LiquidCoffee"] = item;
  if (!mappingp(data["TransientCoffee"])) data["TransientCoffee"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 0, 2500 });
  item->result = "/obj/reagents/shimmer.ob";
  item->result_amt = 1.0;
  item->message = "Something begins to happen in the @@short:#env#@@...\n";
  item->func = ({
        (: $3->set_short("dimly glowing liquid") :),
        (: $3->set_duration(20) :),
        (: $3->set_final("morph") :),
        (: $3->set_result("/obj/reagents/panic.ob") :),
        (: $3->set_result_amt(1.0) :),
        (: $3->set_result_alias("PanicPotion") :),
        (: $3->start_reaction() :)
    });
  data["TransientCoffee"]["TransientPanic"] = item;
  if (!mappingp(data["TransientCoffee"])) data["TransientCoffee"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 0, 2500 });
  item->result = "/obj/reagents/shimmer.ob";
  item->result_amt = 1.0;
  item->message = "Something begins to happen in the @@short:#env#@@...\n";
  item->func = ({
        (: $3->set_short("dimly glowing liquid") :),
        (: $3->set_duration(20) :),
        (: $3->set_final("morph") :),
        (: $3->set_result("/obj/reagents/perception.ob") :),
        (: $3->set_result_amt(1.0) :),
        (: $3->set_result_alias("PerceptionPotion") :),
        (: $3->start_reaction() :)
    });
  data["TransientCoffee"]["TransientPerception"] = item;
  if (!mappingp(data["GroundCoffee"])) data["GroundCoffee"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 1.0/60, 1.0/40 });
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("coffee") :),
        (: $3->set_short("coffee") :),
        (: $3->set_long("This is some freshly made coffee, $amount_size$.\n")
            :),
        (: $3->add_plural("coffee") :),
        (: $3->set_medium_alias("LiquidCoffee") :),
        (: tell_room($5, "The " + $2->short() + " in " + $4->the_short() +
            " turns brown.\n") :),
        (: $3->set_medium_alias("LiquidCoffee") :)
    });
  data["GroundCoffee"]["Water"] = item;
  if (!mappingp(data["GroundComfreyLeaf"])) data["GroundComfreyLeaf"] = ([]);
  item = new( class reaction );
  item->ratio = 1.0/400;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("tea") :),
        (: $3->set_short("pale tea") :),
        (: $3->set_long("This is a pale coloured tea, $pile_size$.") :),
        (: $3->set_medium_alias("ComfreyTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/basic_healing",
                this_player()->query_skill_bonus(
                        "magic.methods.physical.brewing")/10)
        :)
    });
  data["GroundComfreyLeaf"]["Water"] = item;
  if (!mappingp(data["ComfreyTea"])) data["ComfreyTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 400.0, 2400.0 });
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("tea") :),
        (: $3->set_short("pale tea") :),
        (: $3->set_long("This is a pale coloured tea, $pile_size$.") :),
        (: $3->set_medium_alias("ComfreyYarrowTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/basic_healing",
                $1->query_eat_effects()["/std/effects/ingested/basic_healing"]
                  * 2 * $2->query_amount())
        :)
    });
  data["ComfreyTea"]["GroundYarrowLeaf"] = item;
  if (!mappingp(data["GroundGarlic"])) data["GroundGarlic"] = ([]);
  item = new( class reaction );
  item->ratio = 200/2800.0;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
     (: $3->set_name( "syrup" ) :),
     (: $3->set_short( "golden syrup" ) :),
     (: $3->set_long( "This is a sticky looking golden syrup.  "
                      "It smells strongly of garlic.\n") :),
     (: $3->set_medium_alias( "ColdCureSyrup" ) :),
     (: $3->add_eat_effect("/std/effects/ingested/cold_cure",
          ((this_player()->query_skill_bonus(
            "magic.methods.physical.healing")/5)+ 
           (this_player()->query_skill_bonus(
            "magic.methods.physical.brewing")/3))) :),
     (: $3->add_eat_effect("/std/effects/ingested/garlic_breath",
          this_player()->query_skill_bonus(
            "magic.methods.physical.healing")) :)
   });
  data["GroundGarlic"]["Honey"] = item;
  if (!mappingp(data["GroundCabbage"])) data["GroundCabbage"] = ([]);
  item = new( class reaction );
  item->ratio = 1;
  item->result = "/obj/reagents/generic_solid.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "vegetable" ) :),
        (: $3->set_short( "shredded vegetables" ) :),
        (: $3->add_adjective(({"shredded"})) :),
        (: $3->add_alias("vegetables") :),
        (: $3->set_long("This is some shredded vegetables.  Needs something more...\n") :),
        (: $3->set_medium_alias("ShreddedVegetables") :),
    });
  data["GroundCabbage"]["GroundCarrot"] = item;
  if (!mappingp(data["RanchSaladDressing"])) data["RanchSaladDressing"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.1,0.5});
  item->result = "/obj/reagents/generic_solid.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "coleslaw" ) :),
        (: $3->set_short( "coleslaw" ) :),
        (: $3->set_long("This is some delicious coleslaw.  Yum!!!\n") :),
        (: $3->set_medium_alias("Coleslaw") :),
    });
  data["RanchSaladDressing"]["ShreddedVegetables"] = item;
  if (!mappingp(data["GroundMolassesCookie"])) data["GroundMolassesCookie"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.7, 3.0});
  item->result = "/obj/reactions/generic_dough.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "dough" ) :),
        (: $3->set_short( "medicinal dough" ) :),
        (: $3->add_adjective(({"medicinal"})) :),
        (: $3->set_long("This is a medicinal dough.  It smells of molasses.\n") :),
        (: $3->set_medium_alias("MedicinalDough") :),
    });
  data["GroundMolassesCookie"]["LispMedicine"] = item;
  if (!mappingp(data["GroundRadish"])) data["GroundRadish"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.4,0.6});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "pale pink tea" ) :),
        (: $3->add_adjective(({"pale", "pink"})) :),
        (: $3->set_long("This is a pale pink tea.\n") :),
        (: $3->set_medium_alias("RadishTea") :),
    });
  data["GroundRadish"]["Water"] = item;
  if (!mappingp(data["GroundParsnip"])) data["GroundParsnip"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.4,0.6});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "milky white tea" ) :),
        (: $3->add_adjective(({"milky", "white"})) :),
        (: $3->set_long("This is a milky white tea.\n") :),
        (: $3->set_medium_alias("ParsnipTea") :),
    });
  data["GroundParsnip"]["Water"] = item;
  if (!mappingp(data["GroundRadish"])) data["GroundRadish"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.2,0.6});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "milky pink tea" ) :),
        (: $3->add_adjective(({"milky","pink"})) :),
        (: $3->set_long("This is a milky pink tea.\n") :),
        (: $3->set_medium_alias("ParsnipRadishTea") :),
    });
  data["GroundRadish"]["ParsnipTea"] = item;
  if (!mappingp(data["GroundParsnip"])) data["GroundParsnip"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.2,0.6});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "milky pink tea" ) :),
        (: $3->add_adjective(({"milky","pink"})) :),
        (: $3->set_long("This is a milky pink tea.\n") :),
        (: $3->set_medium_alias("ParsnipRadishTea") :),
    });
  data["GroundParsnip"]["RadishTea"] = item;
  if (!mappingp(data["ParsnipTea"])) data["ParsnipTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.8,1.2});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "milky pink tea" ) :),
        (: $3->add_adjective(({"milky","pink"})) :),
        (: $3->set_long("This is a milky pink tea.\n") :),
        (: $3->set_medium_alias("ParsnipRadishTea") :),
    });
  data["ParsnipTea"]["RadishTea"] = item;
  if (!mappingp(data["GroundOregano"])) data["GroundOregano"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.1, 0.9});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "olive-green tea" ) :),
        (: $3->add_adjective(({"olive-green"})) :),
        (: $3->set_long("This is an olive-green tea.\n") :),
        (: $3->set_medium_alias("LispMedicine") :),
    });
  data["GroundOregano"]["ParsnipRadishTea"] = item;
  if (!mappingp(data["FungoidJuice"])) data["FungoidJuice"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 1.8/1, 2.2/1 });
  item->result = "/obj/reagents/generic_salve.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("paste") :),
        (: $3->set_short("sticky grey paste") :),
        (: $3->set_long("This is $pile_size$ of sticky grey paste, " +
                        "$amount_size$.\n") :),
        (: $3->set_medium_alias("PastyFungoidJuice") :),
        (: $3->add_adjective("sticky") :),
        (: $3->add_adjective("grey") :),
        (: $3->add_apply_effect("/std/effects/external/fungus_cure",
                this_player()->query_skill_bonus(
                        "magic.methods.physical.brewing")/10)
        :)
    });
  data["FungoidJuice"]["Water"] = item;
  if (!mappingp(data["GroundComfrey"])) data["GroundComfrey"] = ([]);
  item = new( class reaction );
  item->ratio = 1.0/400;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("tea") :),
        (: $3->set_short("pale tea") :),
        (: $3->set_long("This is a pale coloured tea, $pile_size$.") :),
        (: $3->set_medium_alias("ComfreyTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/basic_healing",
                this_player()->query_skill_bonus(
                        "magic.methods.physical.brewing")/10)
        :)
    });
  data["GroundComfrey"]["Water"] = item;
  if (!mappingp(data["ComfreyTea"])) data["ComfreyTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 400.0, 2400.0 });
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("tea") :),
        (: $3->set_short("pale tea") :),
        (: $3->set_long("This is a pale coloured tea, $pile_size$.") :),
        (: $3->set_medium_alias("ComfreyYarrowTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/basic_healing",
                $1->query_eat_effects()["/std/effects/ingested/basic_healing"]
                  * this_player()->query_skill_bonus(
                        "magic.methods.physical.brewing")/10)
        :)
    });
  data["ComfreyTea"]["GroundYarrow"] = item;
  if (!mappingp(data["Kibrit"])) data["Kibrit"] = ([]);
  item = new( class reaction );
  item->ratio = 1;
  item->result = "/obj/reagents/generic_solid.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "powder" ) :),
        (: $3->set_short( "light yellow powder" ) :),
        (: $3->add_adjective(({"light", "yellow"})) :),
        (: $3->set_long("This is some light yellow powder, $amount_size$.\n") :),
        (: $3->set_medium_alias("KibritTalcum") :),
    });
  data["Kibrit"]["TalcumPowder"] = item;
  if (!mappingp(data["AurumPotable"])) data["AurumPotable"] = ([]);
  item = new( class reaction );
  item->ratio = 1;
  item->result = "/obj/reagents/generic_solid.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "solution" ) :),
        (: $3->set_short( "smelly milky-golden solution" ) :),
        (: $3->add_adjective(({"smelly", "milky-golden"})) :),
        (: $3->set_long("This is a smelly, milky-golden solution, "
             "$amount_size$.\n") :),
        (: $3->add_eat_effect("/std/effects/ingested/frog_repellent",
              100) :),
        (: $3->set_medium_alias("FrogRepellent") :),
    });
  data["AurumPotable"]["KibritTalcum"] = item;
  if (!mappingp(data["GroundVenom"])) data["GroundVenom"] = ([]);
  item = new( class reaction );
  item->ratio = 1.0/400;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("poison") :),
        (: $3->set_short("deadly poison") :),
        (: $3->set_long("This is a pale coloured tea, $pile_size$.") :),
        (: $3->set_medium_alias("VenomPoison") :),
        (: $3->add_eat_effect("/std/effects/ingested/basic_healing",
                this_player()->query_skill_bonus(
                        "magic.methods.physical.brewing")/10)
        :)
    });
  data["GroundVenom"]["Water"] = item;
  if (!mappingp(data["VenomPoison"])) data["VenomPoison"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 400.0, 2400.0 });
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("poison") :),
        (: $3->set_short("deadly poison") :),
        (: $3->set_long("This is a pale coloured tea, $pile_size$.") :),
        (: $3->set_medium_alias("VenomCatalystPoison") :),
        (: $3->add_eat_effect("/std/effects/ingested/basic_healing",
                $1->query_eat_effects()["/std/effects/ingested/basic_healing"]
                  * this_player()->query_skill_bonus(
                        "magic.methods.physical.brewing")/10)
        :)
    });
  data["VenomPoison"]["GroundCatalyst"] = item;
  if (!mappingp(data["PoisonFang"])) data["PoisonFang"] = ([]);
  item = new( class reaction );
  item->ratio = 1.0/400;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("poison") :),
        (: $3->set_short("deadly poison") :),
        (: $3->set_long("This is a pale coloured tea, $pile_size$.") :),
        (: $3->set_medium_alias("FangExtract") :),
        (: $3->add_eat_effect("/std/effects/ingested/basic_healing",
                this_player()->query_skill_bonus(
                        "magic.methods.physical.brewing")/10)
        :)
    });
  data["PoisonFang"]["Water"] = item;
  if (!mappingp(data["FangExtract"])) data["FangExtract"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 400.0, 2400.0 });
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("poison") :),
        (: $3->set_short("deadly poison") :),
        (: $3->set_long("This is a pale coloured tea, $pile_size$.") :),
        (: $3->set_medium_alias("FangLeafPoison") :),
        (: $3->add_eat_effect("/std/effects/poisons/spider_venom",
                $1->query_eat_effects()["/std/effects/poisons/spider_venom"]
                  * this_player()->query_skill_bonus(
                        "covert.items.poisons")/10)
        :)
    });
  data["FangExtract"]["PoisonLeaf"] = item;
  if (!mappingp(data["Coal"])) data["Coal"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 1, 3 });
  item->result = "/obj/mineral.c";
  item->result_amt = 0.33;
  item->func = ({ 
              (: $3->set_name("ore") :),
              (: $3->set_short("refined iron ore") :),
              (: $3->add_adjective(({"refined", "iron"})) :),
              (: $3->set_long("This iron ore has been roasted to "
                       "remove some impurities, $pile_size$.\n") :),
              (: $3->set_medium_alias("RefinedIronOre") :),
              (: $3->set_material("refined iron ore") :)
           });
  data["Coal"]["RawIronOre"] = item;
  if (!mappingp(data["GroundKlatchianFleabane"])) data["GroundKlatchianFleabane"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 1.0/50, 5.0/50 });
  item->result = "/obj/reagents/generic_salve.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("paste") :),
        (: $3->set_short("lavender paste") :),
        (: $3->set_long("This is $pile_size$ of lavender paste, " +
                        "$amount_size$.\n") :),
        (: $3->set_medium_alias("PastyKlatchianFleabane") :),
        (: $3->add_adjective("lavender") :)
    });
  data["GroundKlatchianFleabane"]["Water"] = item;
  if (!mappingp(data["GroundStinkingMayweed"])) data["GroundStinkingMayweed"] = ([]);
  item = new( class reaction );
  item->ratio = ({ 1.0/50, 5.0/50 });
  item->result = "/obj/reagents/flea_salve.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_medium_alias("FleaSalve") :),
        (: $3->add_apply_effect("/std/effects/external/flea_cure",
                this_player()->query_skill_bonus(
                        "magic.methods.physical.healing")/20)
        :)
    });
  data["GroundStinkingMayweed"]["PastyKlatchianFleabane"] = item;
  if (!mappingp(data["GroundParsley"])) data["GroundParsley"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.1,0.2});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "dark green tea" ) :),
        (: $3->add_adjective(({"dark", "green"})) :),
        (: $3->set_long("This is a dark green, fresh-smelling tea.\n") :),
        (: $3->set_medium_alias("ParsleyTea") :),
        (: $3->add_property("potion",1) :),
        (: $3->add_eat_effect("/std/effects/ingested/breath_fresh") :),
    });
  data["GroundParsley"]["Water"] = item;
  if (!mappingp(data["GroundSage"])) data["GroundSage"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.4,0.6});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "grey-green tea" ) :),
        (: $3->add_adjective(({"grey-green"})) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_long("This is a grey-green tea.\n") :),
        (: $3->set_medium_alias("SageTea") :),
    });
  data["GroundSage"]["Water"] = item;
  if (!mappingp(data["FeatherStir"])) data["FeatherStir"] = ([]);
  item = new( class reaction );
  item->ratio = 0.25;
  item->result = LIQUID_OBJ;
  item->result_amt = 0.8;
  item->func = ({
        (: $3->set_name( "potion" ) :),
        (: $3->set_short( "teal green potion" ) :),
        (: $3->add_adjective(({"teal","green"})) :),
        (: $3->set_long("This is a teal green potion.\n") :),
        (: $3->set_medium_alias("SageIntPotion") :),
        (: $3->add_eat_effect("/std/effects/ingested/int_bonus",
    this_player()->
    query_skill_bonus("magic.methods.physical.brewing")/30) :),
    });
  data["FeatherStir"]["SageTea"] = item;
  if (!mappingp(data["GroundSpider"])) data["GroundSpider"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.1,0.2});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "purple tea" ) :),
        (: $3->add_adjective(({"purple"})) :),
        (: $3->set_long("This is a purple tea.\n") :),
        (: $3->set_medium_alias("SpiderTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/cum_poison", 5) :),
    });
  data["GroundSpider"]["Water"] = item;
}

mixed data_return_1() {
  if (!mappingp(data["GroundBasil"])) data["GroundBasil"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.1,0.2});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "green-blue tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->add_adjective(({"green-blue"})) :),
        (: $3->set_long("This is a green-blue tea.\n") :),
        (: $3->set_medium_alias("BasilTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["GroundBasil"]["Water"] = item;
  if (!mappingp(data["GroundThyme"])) data["GroundThyme"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.3,0.6});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_short( "greenish tea" ) :),
        (: $3->add_adjective(({"greenish"})) :),
        (: $3->set_long("This is a greenish tea.\n") :),
        (: $3->set_medium_alias("ThymeTea") :),
    });
  data["GroundThyme"]["Water"] = item;
  if (!mappingp(data["BasilTea"])) data["BasilTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({5,10});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "brown tea" ) :),
        (: $3->add_adjective(({"brown"})) :),
        (: $3->set_long("This is a brown tea.\n") :),
        (: $3->set_medium_alias("BasilSpiderTea") :),
        (: $3->add_property("potion",1) :),
        (: $3->add_eat_effect("/std/effects/ingested/cum_poison", 5) :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["BasilTea"]["GroundSpider"] = item;
  if (!mappingp(data["FennelTea"])) data["FennelTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({5,10});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "muddy tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->add_adjective(({"muddy"})) :),
        (: $3->set_long("This is a muddy tea.\n") :),
        (: $3->set_medium_alias("FennelSpiderTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/cum_poison", 5) :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["FennelTea"]["GroundSpider"] = item;
  if (!mappingp(data["GroundFennel"])) data["GroundFennel"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.1,0.2});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "muddy tea" ) :),
        (: $3->add_adjective(({"muddy"})) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_long("This is a muddy tea.\n") :),
        (: $3->set_medium_alias("FennelSpiderTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/cum_poison", 5) :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["GroundFennel"]["SpiderTea"] = item;
  if (!mappingp(data["BasilSpiderTea"])) data["BasilSpiderTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({6.67,20});
  item->result = "/obj/reagents/antidote_elixir";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->add_property("potion",1) :),
        (: $3->add_eat_effect("/std/effects/ingested/poison_antidote",
    this_player()->
    query_skill_bonus("magic.methods.physical.brewing")/2 +
    random(this_player()->
    query_skill_bonus("magic.methods.physical.brewing") ) ) :),
    });
  data["BasilSpiderTea"]["GroundFennel"] = item;
  if (!mappingp(data["BasilFennelTea"])) data["BasilFennelTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({6.67,20});
  item->result = "/obj/reagents/antidote_elixir";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->add_property("potion",1) :),
        (: $3->add_eat_effect("/std/effects/ingested/poison_antidote",
    this_player()->
    query_skill_bonus("magic.methods.physical.brewing")/2 +
    random(this_player()->
    query_skill_bonus("magic.methods.physical.brewing") ) )  :),
    });
  data["BasilFennelTea"]["GroundSpider"] = item;
  if (!mappingp(data["FennelSpiderTea"])) data["FennelSpiderTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({6.67,20});
  item->result = "/obj/reagents/antidote_elixir";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->add_property("potion",1) :),
        (: $3->add_eat_effect("/std/effects/ingested/poison_antidote",
    this_player()->
    query_skill_bonus("magic.methods.physical.brewing")/2 +
    random(this_player()->
    query_skill_bonus("magic.methods.physical.brewing") ) ) :),
    });
  data["FennelSpiderTea"]["GroundBasil"] = item;
  if (!mappingp(data["BasilSpiderTea"])) data["BasilSpiderTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({1.7,2.3});
  item->result = "/obj/reagents/antidote_elixir";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->add_property("potion",1) :),
        (: $3->add_eat_effect("/std/effects/ingested/poison_antidote",
    this_player()->
    query_skill_bonus("magic.methods.physical.brewing")/2 +
    random(this_player()->
    query_skill_bonus("magic.methods.physical.brewing") ) ) :),
    });
  data["BasilSpiderTea"]["FennelTea"] = item;
  if (!mappingp(data["BasilFennelTea"])) data["BasilFennelTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({1.7,2.3});
  item->result = "/obj/reagents/antidote_elixir";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->add_property("potion",1) :),
        (: $3->add_eat_effect("/std/effects/ingested/poison_antidote",
    this_player()->
    query_skill_bonus("magic.methods.physical.brewing")/2 +
    random(this_player()->
    query_skill_bonus("magic.methods.physical.brewing") ) ) :),
    });
  data["BasilFennelTea"]["SpiderTea"] = item;
  if (!mappingp(data["BasilTea"])) data["BasilTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({1.7,2.3});
  item->result = "/obj/reagents/antidote_elixir";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->add_property("potion",1) :),
        (: $3->add_eat_effect("/std/effects/ingested/poison_antidote",
    this_player()->
    query_skill_bonus("magic.methods.physical.brewing")/2 +
    random(this_player()->
    query_skill_bonus("magic.methods.physical.brewing") ) ) :),
    });
  data["BasilTea"]["FennelSpiderTea"] = item;
  if (!mappingp(data["GroundFennel"])) data["GroundFennel"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.1,0.2});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "pale yellow tea" ) :),
        (: $3->add_adjective(({"pale yellow"})) :),
        (: $3->set_long("This is a pale yellow tea.\n") :),
        (: $3->set_medium_alias("FennelTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["GroundFennel"]["Water"] = item;
  if (!mappingp(data["BasilTea"])) data["BasilTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({5,10});
  item->result = LIQUID_OBJ;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "light green tea" ) :),
        (: $3->add_adjective(({"light","green"})) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_long("This is a light green tea.\n") :),
        (: $3->set_medium_alias("BasilFennelTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["BasilTea"]["GroundFennel"] = item;
  if (!mappingp(data["GroundBasil"])) data["GroundBasil"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.1,0.2});
  item->result = LIQUID_OBJ;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "brown tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->add_adjective(({"brown"})) :),
        (: $3->set_long("This is a brown tea.\n") :),
        (: $3->set_medium_alias("BasilSpiderTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/cum_poison", 5) :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["GroundBasil"]["SpiderTea"] = item;
  if (!mappingp(data["FennelTea"])) data["FennelTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({5,10});
  item->result = LIQUID_OBJ;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "light green tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->add_adjective(({"light","green"})) :),
        (: $3->set_long("This is a light green tea.\n") :),
        (: $3->set_medium_alias("BasilFennelTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["FennelTea"]["GroundBasil"] = item;
  if (!mappingp(data["BasilTea"])) data["BasilTea"] = ([]);
  item = new( class reaction );
  item->ratio = 1;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "light green tea" ) :),
        (: $3->add_adjective(({"light","green"})) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_long("This is a light green tea.\n") :),
        (: $3->set_medium_alias("BasilFennelTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["BasilTea"]["FennelTea"] = item;
  if (!mappingp(data["BasilTea"])) data["BasilTea"] = ([]);
  item = new( class reaction );
  item->ratio = 1;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "brown tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->add_adjective(({"brown"})) :),
        (: $3->set_long("This is a brown tea.\n") :),
        (: $3->set_medium_alias("BasilSpiderTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/cum_poison", 5) :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["BasilTea"]["SpiderTea"] = item;
  if (!mappingp(data["FennelTea"])) data["FennelTea"] = ([]);
  item = new( class reaction );
  item->ratio = 1;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "muddy tea" ) :),
        (: $3->add_adjective(({"muddy"})) :),
        (: $3->set_long("This is a muddy tea.\n") :),
        (: $3->set_medium_alias("FennelSpiderTea") :),
        (: $3->add_property("potion",1) :),
        (: $3->add_eat_effect("/std/effects/ingested/cum_poison", 5) :),
        (: $3->add_eat_effect("/std/effects/ingested/weak_poison_cure") :),
    });
  data["FennelTea"]["SpiderTea"] = item;
  if (!mappingp(data["GroundEyebright"])) data["GroundEyebright"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.6,0.8});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "yellow tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->add_adjective(({"yellow"})) :),
        (: $3->set_long("This is a yellow tea.\n") :),
        (: $3->set_medium_alias("EyebrightTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/blind_protect"):),
    });
  data["GroundEyebright"]["Water"] = item;
  if (!mappingp(data["GroundCarrot"])) data["GroundCarrot"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.6,0.8});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "orange tea" ) :),
        (: $3->add_adjective(({"orange"})) :),
        (: $3->set_long("This is a orange tea.\n") :),
        (: $3->set_medium_alias("CarrotTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/blind_protect") :),
    });
  data["GroundCarrot"]["Water"] = item;
  if (!mappingp(data["CarrotTea"])) data["CarrotTea"] = ([]);
  item = new( class reaction );
  item->ratio = 1;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_short( "yellow-orange tea" ) :),
        (: $3->add_adjective(({"yellow-orange"})) :),
        (: $3->set_long("This is a yellow-orange tea.\n") :),
        (: $3->set_medium_alias("EyebrightCarrotTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/blind_protect"):),
    });
  data["CarrotTea"]["EyebrightTea"] = item;
  if (!mappingp(data["CarrotTea"])) data["CarrotTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({1.25,1.6667});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "yellow-orange tea" ) :),
        (: $3->add_adjective(({"yellow-orange"})) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_long("This is a yellow-orange tea.\n") :),
        (: $3->set_medium_alias("EyebrightCarrotTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/blind_protect") :),
    });
  data["CarrotTea"]["GroundEyebright"] = item;
  if (!mappingp(data["EyebrightTea"])) data["EyebrightTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({1.25,1.6667});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "yellow-orange tea" ) :),
        (: $3->add_adjective(({"yellow-orange"})) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_long("This is a yellow-orange tea.\n") :),
        (: $3->set_medium_alias("EyebrightCarrotTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/blind_protect") :),
    });
  data["EyebrightTea"]["GroundCarrot"] = item;
  if (!mappingp(data["EyebrightCarrotTea"])) data["EyebrightCarrotTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({3.0,4.0});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_short( "red-orange tea" ) :),
        (: $3->add_adjective(({"red-orange"})) :),
        (: $3->set_long("This is a red-orange tea.\n") :),
        (: $3->set_medium_alias("EyebrightCarrotBunnyEyeballTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/blind_protect",
    this_player()->
    query_skill_bonus("magic.methods.physical.brewing")/10) :),
    });
  data["EyebrightCarrotTea"]["GroundBunnyEyeball"] = item;
  if (!mappingp(data["BunnyEyeballTea"])) data["BunnyEyeballTea"] = ([]);
  item = new( class reaction );
  item->ratio = ({1.5,2.0});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->set_short( "red-orange tea" ) :),
        (: $3->add_adjective(({"red-orange"})) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_long("This is a red-orange tea.\n") :),
        (: $3->set_medium_alias("EyebrightCarrotBunnyEyeballTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/blind_protect",
    this_player()->
    query_skill_bonus("magic.methods.physical.brewing")/10) :),
    });
  data["BunnyEyeballTea"]["EyebrightCarrotTea"] = item;
  if (!mappingp(data["GroundBunnyEyeball"])) data["GroundBunnyEyeball"] = ([]);
  item = new( class reaction );
  item->ratio = 1.0;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "tea" ) :),
        (: $3->add_property("potion",1) :),
        (: $3->set_short( "red tea" ) :),
        (: $3->add_adjective(({"red"})) :),
        (: $3->set_long("This is a red tea.\n") :),
        (: $3->set_medium_alias("BunnyEyeballTea") :),
        (: $3->add_eat_effect("/std/effects/ingested/blind_protect") :),
    });
  data["GroundBunnyEyeball"]["Water"] = item;
  if (!mappingp(data["DriedSlug"])) data["DriedSlug"] = ([]);
  item = new( class reaction );
  item->ratio = ({0.1,0.2});
  item->result = "/d/sur/Scrogden/items/slugsoup";
  item->result_amt = 1.0;
  data["DriedSlug"]["Water"] = item;
  if (!mappingp(data["GroundFloppyMadman"])) data["GroundFloppyMadman"] = ([]);
  item = new( class reaction );
  item->ratio = 1.0/400;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name( "water" ) :),
        (: $3->set_short( "pale brownish green water" ) :),
        (: $3->set_long("This is some pale green water, with "
                        "a brownish tint to it, about $pile_size$.\n") :),
        (: $3->set_medium_alias("SpellcheckPotion") :),
        (: $3->add_eat_effect( "/std/effects/ingested/potion_spellcheck",
            this_player()->query_skill_bonus(
               "magic.methods.physical.brewing" ) / 10 ) :)
    });
  data["GroundFloppyMadman"]["Water"] = item;
  if (!mappingp(data["SecondSightPotion"])) data["SecondSightPotion"] = ([]);
  item = new( class reaction );
  item->ratio = 1.0/2500;
  item->result = "/obj/reagents/ss_test.ob";
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_duration(600) :),
        (: $3->set_final("morph") :),
        (: $3->set_result("/obj/reagents/water.ob") :),
        (: $3->set_result_amt(1.0) :),
        (: $3->add_picture_command() :),
        (: $3->start_reaction() :)
    });
  data["SecondSightPotion"]["Water"] = item;
  if (!mappingp(data["RamtopsScumble"])) data["RamtopsScumble"] = ([]);
  item = new( class reaction );
  item->ratio = 2400/400.0;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("scumble") :),
        (: $3->set_short("sparkly scumble") :),
        (: $3->set_long("This is scumble with some sparkly "
               "sherbet traces in it, $amount_size$.\n") :),
        (: $3->add_eat_effect("/std/effects/ingested/drunk", 1500) :),
        (: $3->set_medium_alias("SherbetScumble") :),
    });
  data["RamtopsScumble"]["Sherbet"] = item;
  if (!mappingp(data["GroundThyme"])) data["GroundThyme"] = ([]);
  item = new( class reaction );
  item->ratio = 200/2800.0;
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->func = ({
        (: $3->set_name("bomb") :),
        (: $3->add_adjective("thyme") :),
        (: $3->set_short("thyme bomb") :),
        (: $3->set_long("This is a thyme bomb, a drink that looks like "
                        "sparkly scumble, $pile_size$.") :),
        (: $3->add_eat_effect("/std/effects/ingested/drunk", 1500) :),
        (: $3->set_medium_alias("ThymeBomb") :),
        (: call_out( (: environment($1)->
           add_effect( "/w/dragonkin/effects/thyme_bomb", 
           ({ this_player()->
           query_skill_bonus("magic.methods.physical.brewing"), $1 }) ) :),
           1, $3 ) :),
        (: call_out( (: environment($1)->init() :), 2, $3 ) :)
    });
  data["GroundThyme"]["SherbetScumble"] = item;
  if (!mappingp(data["AncientWine"])) data["AncientWine"] = ([]);
  item = new( class reaction );
  item->ratio = ({1.0, 300});
  item->result = LIQUID_OBJ;
  item->result_amt = 1.0;
  item->message = "There is a bubbling and hissing within the bottle as "
        "the mould and wine react...\n";
  item->func = ({
        (: $3->set_name( "vinegar" ) :),
        (: $3->set_short( "strong vinegar" ) :),
        (: $3->add_adjective("strong") :),
        (: $3->set_long("It's a clear liquid with an exceptionally acidic "
                "odour.\n") :),
        (: $3->set_medium_alias("Vinegar") :),
        (: $3->add_eat_effect( "/std/effects/ingested/poison", 20) :)
    });
  data["AncientWine"]["VinegarCulture"] = item;
}

mixed data_return() {
  data_return_0();
  data_return_1();
  return data;
}
