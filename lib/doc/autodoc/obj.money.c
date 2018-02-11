.DT
money.c
$MUDNAME$ autodoc help
money.c

.SH Inherits
.SP 5 5
This class inherits the following classes /std/object.
.EP

.SH Includes
.SP 5 5
This class includes the following files /include/obj_parser.h, /include/move_failures.h and /include/money.h.
.EP

.SH Public Functions
.SP 5 5
These are functions that everyone can access.
.EP

.SI 3
* %^BOLD%^check_adjectives%^RESET%^
.EI
.SI 5
void check_adjectives()
.EI
.SP 7 5

This method goes through the coins and sets up all the adjectives
and plurals it needs to.

.EP

.SI 3
* %^BOLD%^fixup_money_weight%^RESET%^
.EI
.SI 5
void fixup_money_weight()
.EI
.SP 7 5

This method fixes up the weight of the money object to be what it should
be.

.EP

.SI 3
* %^BOLD%^merge_coins%^RESET%^
.EI
.SI 5
object merge_coins()
.EI
.SP 7 5

This method merges two coin object together.  Or attempts to anyway.
This will occur whenever a coin object moves.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the merged coin object

.EP

.SI 3
* %^BOLD%^query_merger%^RESET%^
.EI
.SI 5
int query_merger()
.EI

.SI 3
* %^BOLD%^query_money%^RESET%^
.EI
.SI 5
int query_money(string type)
.EI
.SP 7 5

This method returns the amount of type of money there is in the array.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the type of money to check
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the number of coins of that type

.EP

.SI 3
* %^BOLD%^query_number_coins%^RESET%^
.EI
.SI 5
int query_number_coins()
.EI
.SP 7 5

This method returns the number of coins in the object.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the number of coins

.EP


