.DT
guild-race.c
$MUDNAME$ autodoc help
guild-race.c

.SH Description
.SP 5 5

This file contains all the information relating to races
that is stored on the player.

.EP
.SP 10 5


Written by Pinkfish

.EP

.SH Includes
.SP 5 5
This class includes the following files /include/command.h, /include/player.h and /include/playtesters.h.
.EP

.SH Public Functions
.SP 5 5
These are functions that everyone can access.
.EP

.SI 3
* %^BOLD%^add_known_command%^RESET%^
.EI
.SI 5
int add_known_command(string str)
.EI
.SP 7 5

This method adds a command to the player.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
str - the command to add
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
1 if was successfuly added, 0 if not
.EP

.SI 3
* %^BOLD%^help_command%^RESET%^
.EI
.SI 5
string help_command(string str)
.EI
.SP 7 5

This method handles help calls associated with commands on the
player.  This will either return a string or a function pointer.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
str - the command to get help on
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the help for the command
.EP

.SI 3
* %^BOLD%^query_gtitle%^RESET%^
.EI
.SI 5
string query_gtitle()
.EI
.SP 7 5

This method returns the title associated with this player.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the title

.EP

.SI 3
* %^BOLD%^query_known_command%^RESET%^
.EI
.SI 5
int query_known_command(string word)
.EI
.SP 7 5

This method checks to see if the command is known by the player.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
word - the command to check
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
1 if known, 0 if not
.EP

.SI 3
* %^BOLD%^query_known_commands%^RESET%^
.EI
.SI 5
string * query_known_commands()
.EI
.SP 7 5

This method returns the array of commands the player knows.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the list of commands the player knows
.EP

.SI 3
* %^BOLD%^query_race%^RESET%^
.EI
.SI 5
string query_race()
.EI
.SP 7 5

This method returns the name of the race associated with the
player.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the race name
.EP

.SI 3
* %^BOLD%^query_race_ob%^RESET%^
.EI
.SI 5
string query_race_ob()
.EI
.SP 7 5

This method returns ther current race object associated with the
player.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the race object
.EP

.SI 3
* %^BOLD%^race_guild_commands%^RESET%^
.EI
.SI 5
void race_guild_commands()
.EI
.SP 7 5

This method adds all the guild-race commands to the player
when they start up.
.EP

.SI 3
* %^BOLD%^remove_known_command%^RESET%^
.EI
.SI 5
int remove_known_command(string str)
.EI
.SP 7 5

This method removes a command from the player.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
str - the command to remove
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
1 if it was successful, 0 if it was unsuccessful
.EP

.SI 3
* %^BOLD%^set_race_ob%^RESET%^
.EI
.SI 5
void set_race_ob(string str)
.EI
.SP 7 5

This method sets the players current race.  This should never be
used, all players should be set by the login object.
the set functions should not be called indisciminately...
becareful when using them that you dont screw up a character ok?
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
str - the players new race
.EP


