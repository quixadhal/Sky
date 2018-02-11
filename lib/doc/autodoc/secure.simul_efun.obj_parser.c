.DT
obj_parser.c
$MUDNAME$ autodoc help
obj_parser.c

.SH Description
.SP 5 5

This is the object matching code.
It is based on some code by Scatter at Dawn Whispers, although this is
both cut down and extended from what scatter had written.
.EP
.SP 10 5

.EP

.SH Includes
.SP 5 5
This class includes the following files /include/obj_parser.h.
.EP

.SH Public Functions
.SP 5 5
These are functions that everyone can access.
.EP

.SI 3
* %^BOLD%^filter_in_me_or_environment%^RESET%^
.EI
.SI 5
object * filter_in_me_or_environment(object * obs, object player)
.EI
.SP 7 5

This method returns the array of objects that are only inside the
player in question, or in their environment.  It excludes any items
in other peoples inventorys.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
obs - the objects to filer
.EP
.SP 9 5
player - the player to check
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the filtered objects

.EP

.SI 3
* %^BOLD%^is_in_me_or_environment%^RESET%^
.EI
.SI 5
int is_in_me_or_environment(object ob, object player)
.EI
.SP 7 5

This method checks to see if the specified object is in the player
or the environment.  If it has no environment it is considered to
be here.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
ob - the object to test
.EP
.SP 9 5
player - the player to check against
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
1 if it is, 0 if not

.EP

.SI 3
* %^BOLD%^match_object_in_array%^RESET%^
.EI
.SI 5
class obj_match match_object_in_array(string input, object * ob_list, int singular, object player)
.EI
.SP 7 5

Find an object in the given array that matches the given words.
This will only return a correct match if the entire string
matches.  If it gets a partial match then nothing will be
returned.  The return array is of the format:
.EP
.SP 7 5
({ flag, class obj_match info })
.EP
.SP 7 5
The flag can be one of: OBJ_PARSER_SUCCESS Successfuly matched the objects.  the objects part of the class will contain the matched objects. OBJ_PARSER_NO_MATCH No successful match.  The text bit of the omatch class will contain the text that didn't match OBJ_PARSER_AMBIGUOUS An ambigous match is returned, this means 'frog' was referenced when there was more than one frog.  The objects part of the class has all the objects there were matched OBJ_PARSER_BAD_FRACTION The specified fracition was bad, the text bit of the class contains the bad fraction. OBJ_PARSER_FRACTION Means that a fraction was attempted to be applied to multiple objects. OBJ_PARSER_TOO_DARK Unable to match the specified object because it is too dark. 
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
input - the input string to match
.EP
.SP 9 5
ob_list - the object to list to match in
.EP
.SP 9 5
singular - force a singular match
.EP
.SP 9 5
player - the person doing the lookup
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
an array of the format ({ flag, class obj_match info })

.EP

.SI 3
* %^BOLD%^match_objects_failed_mess%^RESET%^
.EI
.SI 5
string match_objects_failed_mess(class obj_match failed_match)
.EI
.SP 7 5

This method returns the failed message for the specified
failed match string.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
failed_match - the return result from match_objects_*
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the message to print when it gets an error

.EP

.SI 3
* %^BOLD%^match_objects_for_existence%^RESET%^
.EI
.SI 5
object * match_objects_for_existence(string input, object * env_list, object player)
.EI
.SP 7 5

This method checks for existance of the specified objects.  It will
return any ambiguous matches as well as real matches.  This should only
be used in cases in the code where it is not nessessary to distinguish
between different objects of the same type.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
input - the input string to check
.EP
.SP 9 5
env_list - the environments to check in
.EP
.SP 9 5
player - the player to check with
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the array of objects

.EP

.SI 3
* %^BOLD%^match_objects_in_environments%^RESET%^
.EI
.SI 5
class obj_match match_objects_in_environments(string input, mixed env_list, int singular, object player)
.EI
.SP 7 5

Find an object in the given environments that match the given words.
This will only return a correct match if the entire string
matches.  If it gets a partial match then nothing will be
returned.  The return array is of the format:
.EP
.SP 7 5
({ flag, class obj_match info })
.EP
.SP 7 5
The flag can be one of: OBJ_PARSER_SUCCESS Successfuly matched the objects.  the objects part of the class will contain the matched objects. OBJ_PARSER_NO_MATCH No successful match.  The text bit of the omatch class will contain the text that didn't match OBJ_PARSER_AMBIGUOUS An ambigous match is returned, this means 'frog' was referenced when there was more than one frog.  The objects part of the class has all the objects there were matched OBJ_PARSER_BAD_FRACTION The specified fracition was bad, the text bit of the class contains the bad fraction. OBJ_PARSER_FRACTION Means that a fraction was attempted to be applied to multiple objects. OBJ_PARSER_TOO_DARK Unable to match the specified object because it is too dark. 
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
input - the input string to match
.EP
.SP 9 5
env_list - the environments to get the objects from
.EP
.SP 9 5
singular - force a singular match
.EP
.SP 9 5
player - the person doing the lookup
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
an array of the format ({ flag, class obj_match info })

.EP

.SI 3
* %^BOLD%^query_multiple_short%^RESET%^
.EI
.SI 5
varargs string query_multiple_short(mixed args, string type, int no_dollars, int flag)
.EI
.SP 7 5

Returns a nice string which looks like "Frog, fish and chicken".  This
takes in an array of objects and or strings and returns a message
like that shown above.  It also does things like adding the number of
each type at the start of the function.


This function has two main uses: to produce the control codes for a     set of objects and to produce a nice list from a set of strings.     The "type" argument is optional and may be "a", "the", "one" or     "poss", defaulting to "a": it determines which of a_short,     the_short, one_short or poss_short will be used to make the control     code sequence for the objects in "args".  If all of the elements of     "args" are objects, it simply returns the block control code for     the relevant shorts of the objects.  Otherwise, it looks through     the elements one at a time, replacing objects by the result of     calling the relevant short function on them; finally it     concatenates the elements with commas between each pair, except     between the last pair which have " and " between them.  Since the     output will contain processing codes if there are any objects in     "args", it should be passed through convert_message before being     stored. 
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
args - the array to do the processing on
.EP
.SP 9 5
type - the type of short to use "a", "the", "one", "poss"
.EP
.SP 9 5
no_dollars - this forces the output to be a real string
.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
the processed string

.EP


