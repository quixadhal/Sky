.DT
history.c
$MUDNAME$ autodoc help
history.c

.SH Description
.SP 5 5
Save the current state. 
.EP
.SP 10 5

.EP

.SH Public Functions
.SP 5 5
These are functions that everyone can access.
.EP

.SI 3
* %^BOLD%^add_chat_history%^RESET%^
.EI
.SI 5
void add_chat_history(string channel, string start, string mess)
.EI
.SP 7 5

Keep a mesage from a specified line on the chat stick.
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
channel - the channel being stored
.EP
.SP 9 5
start - the start string (name)
.EP
.SP 9 5
mess - the message being stored

.EP

.SI 3
* %^BOLD%^query_chat_history%^RESET%^
.EI
.SI 5
mixed * query_chat_history(string channel)
.EI
.SP 7 5

Returns the current chat channel history. The array contains arrays of
three elemnts each.,   In each array the first element is the
start, the second the mssage and thwe third the time.


({  .. ({ start, mess, time }), ({ start, mess, time }), .. })
.EP
.SI 7
%^BOLD%^Parameters:%^RESET%^
.EI
.SP 9 5
channel - the channel to get the history of

.EP
.SI 7
%^BOLD%^Returns:%^RESET%^
.EI
.SP 9 5
an array of the chat channel history.
.EP

.SI 3
* %^BOLD%^save_me%^RESET%^
.EI
.SI 5
void save_me()
.EI


