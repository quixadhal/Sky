inherit "/obj/food";

void setup() {

    set_name( "face" );
    set_short( "braised pig face" );
    add_property( "determinate", "a " );
    add_adjective( ({ "braised", "pig" }) );
    set_main_plural( "braised pig faces" );
    set_long( "Folklore has it that eating the face of a pig will "
              "enhance one's beauty and make one more intelligent "
              "than the common folks.  A local chef perfected the art "
              "of cooking pig faces.  The pig face is double boiled and "
              "braised in a medley of herbs and spices till it is soft "
              "tender, yet slightly crunchy around the ears.  Worth a shot, "
              "if you believe in old wives' tales, besides, you have "
              "nothing to lose...\n" );
    add_property( "no recycling", 1 );
    add_alias( "pig face" );
    set_value( 6000 );
    set_weight( 8 );
    set_weight_per_bite( 2 );
    add_eat_effect( "/std/effects/ingested/nausea.c", 100 );

    //add_eat_effect( "/std/effects/ingested/int_bonus.c", 60 );
} 