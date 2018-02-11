.DT
money_handler.c
$MUDNAME$ autodoc help
money_handler.c

.SH Description
.SP 5 5

This handles all the methods for determining values of coins and
the current valid set of coins.   It also handles change calculation.
This was written originaly by Pinkfish, reworked significantly by
Deutha to add in the multiple currency areas.
.EP
.SP 10 5


Written by Pinkfish
.EP



.SH See also
.SP 5 5
/std/living/money.c
.EP
.SH Change history
.SP 5 5
Fixed up the code that deals with different currency areas.
Also added money_array_from_string() and made value_from_string()
work properly with different types of currencies.
- Sandoz, April 2003.

.EP

.SH Includes
.SP 5 5
This class includes the following files /include/money.h.
.EP

.SH Public Functions
.SP 5 5
These are functions that everyone can access.
.EP

.SI 3
* %^BOLD%^calc_change%^RESET%^
.EI
.SI 5
mixed calc_change(int value, mixed * mon_array)
.EI
.SP 7 5

This method calculates the change of a certain value from a
given money array.   This makes sure that the change does not include
money that does not actually exist.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
value - the value of the change to calculate
.EP
.SP 9 5
mon_array - the money array to determine the change from
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the money array containing the change to use
.EP

.SI 3
* %^BOLD%^create_money_array%^RESET%^
.EI
.SI 5
varargs mixed create_money_array(int value, string where)
.EI
.SP 7 5

This method creates a money array from a certain value in a particular
money area. A money array consists of ({ type, number }) pairs in an
array.   ie: ({ "brass", 12, "copper", 24 }).
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
value - the value to get the money array for
.EP
.SP 9 5
where - the money area to get the value in
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
a money array for the value in the area
.EP

.SI 3
* %^BOLD%^filter_legal_tender%^RESET%^
.EI
.SI 5
varargs object * filter_legal_tender(object money, string where)
.EI
.SP 7 5

This method figures out the legal and illegal tender money from
the specified money object in the specified money area.   This method
returns a two element array which consists of the legal and illegal
tender for the given money area.   ({ legal, illegal })
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
money - the money object to get the legal tender from
.EP
.SP 9 5
where - the money area the tender is for
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
an two element array of objects ({ legal, illegal })
.EP

.SI 3
* %^BOLD%^make_money_array_payment%^RESET%^
.EI
.SI 5
mixed make_money_array_payment(string type, int value, mixed mon_array, string where, int use_default)
.EI
.SP 7 5

This method makes a payment from a money array.  It returns the
depleted money array, the amount taken out and the change
needed.  If the type is not set, then the best fit for the value
is found from the array.


The return array is formated as:
.EP
.SP 7 5
({ depleted_money_array, change, taken_from })
.EP
.SP 7 5
The change is an integer value.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the type of money to take out
.EP
.SP 9 5
value - the amount of the type to take out
.EP
.SP 9 5
mon_array - the money array to use
.EP
.SP 9 5
where - the money area
.EP
.SP 9 5
use_default - allow the use of the default money type
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the return array as formated above

.EP

.SI 3
* %^BOLD%^make_new_amount%^RESET%^
.EI
.SI 5
varargs object make_new_amount(int value, string where)
.EI
.SP 7 5

This method creates a money object of a certain value in a certain
money area.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
value - the value to create the new money object with
.EP
.SP 9 5
where - the area to create the new money object in
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the new money object

.EP

.SI 3
* %^BOLD%^make_payment%^RESET%^
.EI
.SI 5
varargs mixed make_payment(string type, int value, object thing, string where)
.EI
.SP 7 5

This method makes a payment of a particular amount in a particular
money area.   Please note that player or living objects can double
as money objects in this circumstance.  The first element of the
payment array is the values which should be used to take off
the player, the second element is the change needed to be payed
back.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the type of money to pay in
.EP
.SP 9 5
value - the number of the type to pay
.EP
.SP 9 5
thing - the thing which is doing the payment (money object)
.EP
.SP 9 5
where - the money area the payment will occur in
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the payment array
.EP

.SI 3
* %^BOLD%^money_array_from_string%^RESET%^
.EI
.SI 5
mixed money_array_from_string(string str, string where, int flag)
.EI
.SP 7 5

This method attempts to create a money array from a string.  It will
attempt to do fuzzy matching of the type.  This means it will match on
partial matches, this could lead to somewhat weird behaviour.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
str - the string to find the value of
.EP
.SP 9 5
where - the currency area to get the money array for
.EP
.SP 9 5
flag - if the flag is set, then we will return 0 on a partial match
.EP

.SI 3
* %^BOLD%^money_string%^RESET%^
.EI
.SI 5
string money_string(mixed mon_array)
.EI
.SP 7 5

This method converts a money array into a string so it can be displayed.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
mon_array - the money array to convert into a string
.EP

.SI 3
* %^BOLD%^money_value_string%^RESET%^
.EI
.SI 5
varargs string money_value_string(int value, string where)
.EI
.SP 7 5

This method returns a string which is based on the value of
the money in a certain money area.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
value - the value to get the string for
.EP
.SP 9 5
where - the place to get the string for
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
a string of the money value in the certain money area
.EP

.SI 3
* %^BOLD%^parse_money%^RESET%^
.EI
.SI 5
varargs mixed parse_money(string words, object player, string place)
.EI
.SP 7 5

This method determines all the money from the player object and moves
it into a container.  It then figured out the legal tender for
specified money area and tells the player if the given money is
legal tender for the current area.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
words - the string to match the money on
.EP
.SP 9 5
player - the player who is attempting the transaction
.EP
.SP 9 5
place - the money area the transaction is taking place
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
a money object consisting of the legal tender
.EP

.SI 3
* %^BOLD%^pay_amount_from%^RESET%^
.EI
.SI 5
varargs object pay_amount_from(int value, object money, string where)
.EI
.SP 7 5

This method makes a payment from a specified money object.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
value - the amount to pay
.EP
.SP 9 5
money - the money object to pay from
.EP
.SP 9 5
where - the money area the payment occurs in
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the change object
.EP

.SI 3
* %^BOLD%^query_aliases_for%^RESET%^
.EI
.SI 5
string * query_aliases_for(string type)
.EI
.SP 7 5

This method returns all the current aliases for the given type
of money.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the type of money to get the aliases for
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the aliases for the money type

.EP

.SI 3
* %^BOLD%^query_aliases_of%^RESET%^
.EI
.SI 5
string * query_aliases_of(string word)
.EI
.SP 7 5

This converts a currency type's alias (i.e. "royal" ) and returns
its 'real' names (i.e. "Ankh-Morpork royal"
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
word - the alias to find the real name of
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
an array of real names, or 0 if it's not a real alias

.EP

.SI 3
* %^BOLD%^query_all_places%^RESET%^
.EI
.SI 5
string * query_all_places()
.EI
.SP 7 5

This method returns the current set of areas in which types can
be found.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the set of places

.EP

.SI 3
* %^BOLD%^query_all_values%^RESET%^
.EI
.SI 5
mapping query_all_values()
.EI
.SP 7 5

This method returns the mapping containing all the values of the
currently valid money types.   The mapping has keys of the domain
of the money and has a value of an array.   The array contains
alternating name, value pairs.

.EP
.SP 7 5
([ "default": ({ "brass", 1, "copper", 10, "silver", 100,
.EP
.SP 7 5
                 "gold", 2000, "platinum", 6000 }) ])
.EP
.SP 7 5

.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the mapping of values
.EP

.SI 3
* %^BOLD%^query_details%^RESET%^
.EI
.SI 5
mapping query_details()
.EI
.SP 7 5

This method returns all the details for the current set of
coins.   The details are information which is shown when the coin
is looked at.   Stuff about heads and tails and things.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the mapping of details for all coin types
.EP

.SI 3
* %^BOLD%^query_details_for%^RESET%^
.EI
.SI 5
class money_data query_details_for(string type)
.EI
.SP 7 5

This method returns the details for a specified type of money.
It will return a class of the form:

.EP
.SP 7 5
({ forward short, reverse short,
.EP
.SP 7 5
   forward long, reverse long, composition,
.EP
.SP 7 5
   plural, adjectives, weight })
.EP
.SP 7 5

.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the money type to get the details for
.EP

.SI 3
* %^BOLD%^query_main_plural_for%^RESET%^
.EI
.SI 5
string query_main_plural_for(string type)
.EI
.SP 7 5

This method returns the main short plural description of the money type.
This returns the expanded plural version like 'Ankh-Morpork pennies'.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the money type to get the short plural description for
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the short plural description for the money object
.EP

.SI 3
* %^BOLD%^query_origin_of%^RESET%^
.EI
.SI 5
string query_origin_of(string type)
.EI
.SP 7 5

This takes a coin type and returns the place it is associated with.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the coin type i.e. "Ankh-Morpork dollar"
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the place i.e. "Ankh-Morpork"

.EP

.SI 3
* %^BOLD%^query_person_payments%^RESET%^
.EI
.SI 5
mixed query_person_payments(int value, string place, object payer, object payee)
.EI
.SP 7 5

This method makes a payment from one person to another.
This method figures out what money should be given to the player
and what should be taken from the other to make a payment of the
correct value in the correct place.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
value - the value to pay
.EP
.SP 9 5
place - the place to make the payment in
.EP
.SP 9 5
payer - the person the money is payed from
.EP
.SP 9 5
payee - the person the money is payed to
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
two element array, or 0 if it cannot be done

.EP

.SI 3
* %^BOLD%^query_plural_for%^RESET%^
.EI
.SI 5
string query_plural_for(string type)
.EI
.SP 7 5

This method returns the short plural description of the money type.
This returns just the one word, like 'coins' or 'talons'.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the money type to get the short plural description for
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the short plural description for the money object
.EP

.SI 3
* %^BOLD%^query_short_for%^RESET%^
.EI
.SI 5
string query_short_for(string type)
.EI
.SP 7 5

This method returns the short description of the money type.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the money type to get the short description for
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the short description for the money object
.EP

.SI 3
* %^BOLD%^query_total_value%^RESET%^
.EI
.SI 5
varargs int query_total_value(mixed mon_array, string where)
.EI
.SP 7 5

This method determines the total value of a specified money array.
A money array consists of pairs of values ({ type, number })
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
mon_array - the array to find the value of
.EP
.SP 9 5
where - the money area to get the value in
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the total value as an integer
.EP

.SI 3
* %^BOLD%^query_valid_types%^RESET%^
.EI
.SI 5
string * query_valid_types()
.EI
.SP 7 5

This returns a list of valid coin types
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
an array of valid coin types

.EP

.SI 3
* %^BOLD%^query_value%^RESET%^
.EI
.SI 5
varargs int query_value(string type, string where)
.EI
.SP 7 5

This method returns the value of a specified type of money in a certain
money area.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the type of money to get the value for
.EP
.SP 9 5
where - the money area the money is in
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the integer value of the money
.EP

.SI 3
* %^BOLD%^query_value_of%^RESET%^
.EI
.SI 5
int query_value_of(string type)
.EI
.SP 7 5

This returns the value of a currency type.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - currency type
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
an int of the currency type's value

.EP

.SI 3
* %^BOLD%^query_values%^RESET%^
.EI
.SI 5
mixed * query_values()
.EI
.SP 7 5

This method returns the values in the default area.
This method returns the array as given in the value above.
It contains name, value pairs and is for the "default"
area.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the array of values
.EP

.SI 3
* %^BOLD%^query_values_in%^RESET%^
.EI
.SI 5
mixed query_values_in(string where)
.EI
.SP 7 5

This method returns the values in the specified area.
It contains name, value pairs and is for the "default"
area.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
where - the area in which to return the values for
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the array of values
.EP

.SI 3
* %^BOLD%^query_weight_for%^RESET%^
.EI
.SI 5
int query_weight_for(string type)
.EI
.SP 7 5

This method returns the weight of 100 coins of the money type.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
type - the money type to get the weight of 100 coins for
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the weight of 100 coins of the specified type

.EP

.SI 3
* %^BOLD%^smallest_in%^RESET%^
.EI
.SI 5
string smallest_in(string place)
.EI
.SP 7 5

This returns the smallest unit of currency in this place.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
place - The place to query.
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
a string of the smallest unit of currency

.EP

.SI 3
* %^BOLD%^value_from_string%^RESET%^
.EI
.SI 5
int value_from_string(string str, string where)
.EI
.SP 7 5

This method attempts to find a money value from a string.  It will
attempt to do fuzzy matching of the type.  This means it will match on
partial matches, this could lead to somewhat weird behaviour...  So it
goes.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
str - the string to find the value of
.EP


