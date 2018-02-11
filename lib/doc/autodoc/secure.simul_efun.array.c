.DT
array.c
$MUDNAME$ autodoc help
array.c

.SH Description
.SP 5 5

This function finds all the occurrences of the target in an array.
It returns an array of the positions.
.EP
.SP 10 5


Written by Terano

.EP

.SH Public Functions
.SP 5 5
These are functions that everyone can access.
.EP

.SI 3
* %^BOLD%^delete%^RESET%^
.EI
.SI 5
mixed delete(mixed arr, int start, int len)
.EI
.SP 7 5

This method deletes and element from an array.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
arr - the array to delete an element from
.EP
.SP 9 5
start - the start position to delete from
.EP
.SP 9 5
len - the number of elements to delete
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the new array

.EP

.SI 3
* %^BOLD%^find_member%^RESET%^
.EI
.SI 5
int * find_member(mixed target, mixed arr)
.EI

.SI 3
* %^BOLD%^insert%^RESET%^
.EI
.SI 5
mixed insert(mixed arr, mixed el, int pos)
.EI
.SP 7 5

This method inserts an element into the array.
It inserts an element into the array at the specified point.
This function accepts strings as well as arrays.


The code to do this is: "arr[0..pos-1] + ({ el }) + arr[pos..]".
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
arr - the array to insert the element into.
.EP
.SP 9 5
el - the element to insert
.EP
.SP 9 5
pos - the position at which to insert it
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the array with the element inserted into it

.EP


