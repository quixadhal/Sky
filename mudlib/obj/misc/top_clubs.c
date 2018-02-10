/**
 * This keeps track of the top families in various areas.
 * @author Pinkfish
 * @started May 16th
 */
#include <top_ten_tables.h>
#include <clubs.h>
inherit "/std/object";

string long( string words, int number );

string long(string str, int dark);

void setup() {
   set_name( "table" );
   set_short( "top club table" );
   add_adjective( ({ "top", "club", "score" }) );
   reset_get();
   add_property( "there", "in one corner" );
   add_property( "survive fire", 1 );
   add_read_mess((: long("",0) :), 0, "common", 0);
} /* setup() */

string club_name(string str) {
   if (!str) {
      return "Unknown";
   }
   return CLUB_HANDLER->query_club_name(str);
} /* club_name() */

string long(string str, int dark) {
   mixed *stuff;
   string bing;
   string guild;
   string *g_sorted;
   mapping g_info;

   stuff = TOP_TEN_HANDLER->query_club_info();

   str = "The club with:\n"
         "The most members          : " +
         club_name(stuff[TOP_TEN_LARGEST_FAMILY])  + "\n"
         "The oldest average age    : " +
         club_name(stuff[TOP_TEN_OLDEST_FAMILY]) + "\n"
         "Most average quest points : " +
         club_name(stuff[TOP_TEN_MOST_QUEST_POINTS]) + "\n"
         "Most player killers       : " +
         club_name(stuff[TOP_TEN_MOST_PKS]) + "\n"
         "Largest single gender     : " +
         club_name(stuff[TOP_TEN_SINGLE_GENDER]) + "\n"
      "Oldest start Riftwar time      : " +
         club_name(stuff[TOP_TEN_OLDEST_LOGONS]) + "\n"
         ;

   g_info = stuff[TOP_TEN_MOST_GUILD];
   g_sorted = sort_array(filter(keys(g_info), (: stringp($1) &&
                                                 file_size($1 + ".c") > 0 :)),
                         (: strcmp($1->query_name(), $2->query_name()) :));
   foreach (guild in g_sorted) {
      if (stringp(guild)) {
         bing = g_info[guild];
         str += sprintf("%-26s: %s\n",
               "Most " + (guild->query_name()),
               club_name(bing));
      }
   }

   str += "\nTo get onto the average list the club must have more than "
          "3 members that are not creators.\n";
   return str;
} /* long() */
