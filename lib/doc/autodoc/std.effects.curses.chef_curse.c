.DT
chef_curse.c
$MUDNAME$ autodoc help
chef_curse.c

.SH Description
.SP 5 5

This curse makes the players talk as if they are the sweedish chef.
The classification of the effect is "misc.curse.chef".


The argument is an integer and specifies how long the curse will last. 

This effect has a shadow associated with it.
.EP
.SP 10 5

.EP

.SH Example
.SI 5
// give them the curse for 2 minutes
add_effect("/std/effects/curses/chef_curse", 120);

.EI
.SH Includes
.SP 5 5
This class includes the following files /include/effect.h.
.EP

