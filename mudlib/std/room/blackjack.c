/**
 * A Blackjack inherit for Divided Sky.
 * @author Avelan, 3rd February 2002.
 * @changed Did some cosmetics and fixed a few bugs.
 * - Sandoz, 2002-2003.
 */

nosave inherit ROOM_OBJ;

#define BUSTED     -1
#define BLACKJACK  -2
#define NAMES      ([ 11 : "Jack", 12 : "Queen", 13 : "King", 14 : "Ace" ])

#define TYPES      ([ "d" : "Diamonds", \
                      "c" : "Clubs",    \
                      "s" : "Spades",   \
                      "h" : "Hearts",   \
                      ])

#define CARDS      ({ "02", "03", "04", "05", "06", "07", \
                      "08", "09", "10", "11", "12", "13", "14", \
                      })

#define MSTRING(x) MONEY_H->money_value_string( x, query_place() )
#define MARRAY(x)  MONEY_H->create_money_array( x, query_place() )

private nosave object *pending, *players, *now_playing, *played;
private nosave object cur_plr, dealer;
private nosave int hand, doing_split, max_players, min_bet, max_bet, deck_amt;
private nosave int main_id, reset_id, check_id;
private nosave string *deck, savefile, dealer_file, place;
private nosave mapping data, used;

class player_data {
    string *hand1;
    string *hand2;
    int *bets;
}

int winnings_paid, raw_income;

private void reset_game();
varargs void hands( object player, int which );
varargs void check_state( int kick );
varargs mixed eval_cards( object player, int hand_b, int text );
void switch_player();
void deal_card( object plr );
void make_dealer();
int do_bet( object *monies );

/** @ignore yes */
string *query_deck() { return deck; }

/** @ignore yes */
mapping query_used() { return used; }

/** @ignore yes */
mapping query_data() { return data; }

/** @ignore yes */
object *query_players() { return players; }

/** @ignore yes */
private void get_play_data() {
    if( savefile && savefile != "" && file_exists( savefile ) )
        unguarded( (: restore_object, savefile :) );
} /* get_play_data() */

/** @ignore yes */
private void save_play_data() {
    unguarded( (: save_object,savefile :) );
} /* save_play_data() */

/** @ignore yes */
void create() {
    ::create();

    get_play_data();

    players = ({ });
    played = ({ });
    data = ([ ]);
    now_playing = ({ });
    deck = ({ });
    pending = ({ });

    add_help_file("blackjack_table");

    if( base_name(TO) != __FILE__[0..<3] ) {
        if( !savefile )
            write("Save file unset for "+base_name(TO)+".\n");
        make_dealer();
    }

    add_extra_look(TO);

} /* create() */

/** @ignore yes */
void reset() {
    if( !dealer && base_name(TO) != __FILE__[0..<3] )
        call_out("make_dealer", 4 );
} /* reset() */

/**
 * This method creates a generic dealer for the blackjack room.
 * If you want to have a custom dealer, please overwrite this
 * function in the actual room.  Alternatively you can specify
 * a dealer to be loaded from a separate file.
 * @see set_dealer()
 * @see query_dealer()
 */
void make_dealer() {
        if( base_name(TO) == __FILE__[0..<3] )
            return;

    if( !dealer ) {
        if( !dealer_file ) {
            dealer = clone_object(NPC_OBJ);
            dealer->set_name("dealer");
            dealer->set_short("Speedy the dealer");
            dealer->set_determinate("");
            dealer->add_alias( ({"speedy", "Speedy", "dealer"}) );
            dealer->add_adjective( ({"Speedy", "speedy", "the"}) );
            dealer->add_property("unique", 1 );

            dealer->set_long("Speedy is a short man with ragged clothes full "
                "of holes.  His composure reminds that of an old woman, and "
                "it is hard to see whether he is standing or sitting.\n");

            dealer->basic_setup("human", 200 + random( 50 ), 1 );

            dealer->load_chat( 20, ({
                1, ":lights a cigarette and blows a big cloud of smoke into "
                   "the air.",
                1, ":sighs.",
                1, ":gets a nasty look in his eyes, then coughs so loudly it "
                   "would scare off most people."
                }) );

            dealer->load_a_chat( 50, ({
                1, "'You will regret this!",
                1, "'Come on, leave me alone!",
                1, "'I've done nothing wrong, have I?!",
                }) );
        } else {
            dealer = clone_object(dealer_file);
        }
    }

    if( !ENV(dealer) || ENV(dealer) != TO )
        dealer->move( TO, "$N enter$s from elsewhere.",
                          "$N run$s away, screaming.");

} /* make_dealer() */

/**
 * This method specifies the optional external dealer file to clone
 * the dealer from.  It is usually better to overwrite make_dealer()
 * in the actual room though.
 * @param word the file name of the dealer to load
 */
void set_dealer( string word ) {
    if( word && word != "" ) {
        if( word[<2..] == ".c" )
            word = word[0..<3];
        if( file_exists( word+".c" ) ) {
            dealer_file = word;
            return;
        }
    }

    error("Trying to set an invalid dealer file - "+word+"\n");

} /* set_dealer() */

/**
 * This method returns the dealer file (if one is set).
 * @return the external dealer file used
 */
string query_dealer() { return dealer_file; }

/**
 * This method sets the save file for the blackjack table.
 * That file will be used to save the profit made etc.
 * @param word the save file to use
 */
void set_save_file( string word ) {
    savefile = word+".o";
} /* set_save_file() */

/**
 * This method returns the save file of the blackjack table.
 * @return the save file path and name
 */
string query_save_file() { return savefile; }

/**
 * This method sets the minimum possible bet in this table.
 * @param amount the minimum bet amount
 */
void set_min_bet( int amount ) {
    min_bet = amount;
} /* set_min_bet() */

/**
 * This method returns the minimum possible bet in this table.
 * @return the minimum bet amount
 */
int query_min_bet() { return min_bet; }

/**
 * This method sets the maximum possible bet in this table.
 * @param amount the maximum bet amount
 */
void set_max_bet( int amount ){
    max_bet = amount;
} /* set_max_bet() */

/**
 * This method returns maximum possible bet in this table.
 * @return the maximum bet amount
 */
int query_max_bet() { return max_bet; }

/**
 * This method sets the maximum number of players possible in this table.
 * @param amount the maximum number of players
 */
void set_max_players( int amount ) {
    max_players = amount;
} /* set_max_players() */

/**
 * This method returns the maximum number of players possible in this table.
 * @return the maximum number of players
 */
int query_max_players() { return max_players; }

/**
 * This method sets the number of decks to use in the blackjack game.
 * Normally this should be set to 6.
 * @param amount the number of decks to use
 */
void set_deck_amount( int amount ) {
    deck_amt = amount;
} /* set_deck_amt() */

/**
 * This method returns the number of decks to use in the blackjack game.
 * @return the number of decks to use
 */
int query_deck_amount() { return deck_amt; }

/**
 * This method sets the place the blackjack table operates in.
 * It is needed when the table is outside of the "default" money area.
 * @param word the place to set
 */
void set_place( string word ) { place = word; }

/**
 * This method returns the place this blackjack table operates in.
 * @return the place we're operating in
 */
string query_place() { return place || "default"; }

/** @ignore yes */
void init() {
    ::init();
    add_command("join", "");
    add_command("join", "game");
    add_command("bet", "<indirect:object:me>'money'", (: do_bet($1) :) );
    add_command("hit", "");
    add_command("double", "");
    add_command("split", "");
    add_command("stand", "");
    add_command("cards", "");
    add_command("leave", "");
    add_command("status", "");
    add_command("count", "cards");
    if( creatorp(TP) )
        add_command("statistics", "");
} /* init() */

/**
 * This method returns 1 if the specified player is already playing the game.
 * @param player the player to check
 * @return 1 if the player is already playing the game
 */
int query_playing( object player ) {
        return member_array( player, players ) != -1;
} /* query_playing() */

/**
 * This method returns 1 if the specified player in the pending array,
 * waiting to start playing.
 * @param player the player to check
 * @return 1 if the player is pending
 */
int query_pending( object player ) {
        return member_array( player, pending ) != -1;
} /* query_playing() */

/** @ignore yes */
int do_join() {
    if( !dealer || ENV(dealer) != TO ) {
        add_failed_mess("The dealer isn't present at the moment.\n");
        return 0;
    }

    if( sizeof(pending) + sizeof(players) >= max_players ) {
        add_failed_mess("The table is already full.  Please wait until "
            "someone leaves the game.\n");
        return 0;
    }

    if( query_pending( TP ) ) {
        add_failed_mess("You are already waiting for the next round to "
            "start.\n");
        return 0;
    }

    if( query_playing( TP ) ) {
        add_failed_mess("You are already playing the game.\n");
        return 0;
    }

    pending += ({ TP });
    add_succeeded_mess("$N sit$s on one of the seats and join$s the game.\n");

    if( !hand ) {
        if( reset_id )
            remove_call_out( reset_id );
        reset_id = call_out( (: reset_game :), 10 );
    }

    return 1;

} /* do_join */

/** @ignore yes */
void end_game() {
    hand = 0;
    doing_split = 0;
    cur_plr = 0;
    data = ([ ]);
    played = ({ });
    now_playing = ({ });

    if( main_id ) {
        remove_call_out( main_id );
        main_id = 0;
    }

    if( check_id ) {
        remove_call_out( check_id );
        check_id = 0;
    }

    if( reset_id )
        remove_call_out( reset_id );

    reset_id = call_out( (: reset_game :), 10 );

} /* end_game() */

/** @ignore yes */
void remove_player( object plr ) {
    map_delete( data, plr );
    now_playing -= ({ plr });
    players -= ({ plr });
    pending -= ({ plr });
    played -= ({ plr });
} /* remove_player() */

/** @ignore yes */
int do_leave() {
    if( classp( data[TP] ) && data[TP]->bets ) {
        add_failed_mess("You have placed bets this round, please wait "
            "until the game is finished.\n");
        return 0;
    }

    if( query_pending( TP ) || query_playing( TP ) ) {
        remove_player( TP );
        if( !sizeof(pending) && !sizeof(players) ) {
            add_succeeded_mess("$N $V the blackjack table and $I clears the "
                "table as there is no-one left playing.\n", ({ dealer }) );
            end_game();
            return 1;
        }
        add_succeeded_mess("$N $V the blackjack table.\n");
        return 1;
    }

    add_failed_mess("You are not playing blackjack.\n");
    return 0;

} /* do_leave() */

/** @ignore yes */
int do_bet( object *money ) {
    int value;
    object *tender, legal, illegal, cont;
    class player_data game_data;

    if( !query_playing( TP ) ) {
        if( query_pending( TP ) ) {
            add_failed_mess("You have to wait until a new round begins to "
                "place your bet.\n");
            return 0;
        }
        add_failed_mess("You are not playing blackjack.  Please join the "
            "game first.\n");
        return 0;
    }

    if( !hand ) {
        add_failed_mess("You have to wait until a new round begins to "
            "place your bet.\n");
        return 0;
    }

    game_data = data[TP] || new( class player_data );

    if( game_data->bets || member_array( TP, played ) != -1 ) {
        add_failed_mess("You have already placed your bet this round.\n");
        return 0;
    }

    if( sizeof( filter( money, (: !$1->query_property( "money" ) :) ) ) ) {
        add_failed_mess("You can only bet money, silly.\n");
        return 0;
    }

    cont = clone_object("/std/container");
    money->move( cont );
    money = INV( cont );

    if( !sizeof( money ) ) {
        cont->dest_me();
        add_failed_mess("Something is broken, please contact a creator.\n");
        return 0;
    }

    tender = MONEY_H->filter_legal_tender( money[ 0 ], query_place() );
    legal = tender[ 0 ];
    illegal = tender[ 1 ];
    cont->dest_me();

    if( illegal ) {
        tell_object( TP, "Sorry, "+(string)MONEY_H->money_string(
            illegal->query_money_array() )+" is not legal money here.\n"
            "Betting "+(string)MONEY_H->money_string(
            legal->query_money_array() )+".\n");
        illegal->move( TP );
    }

    if( legal ) {
        value = legal->query_value_in(query_place());
        if( query_place() != "default" )
            value += legal->query_value_in("default");

        if( !value || value > max_bet || value < min_bet ) {
            legal->move(TP);
            add_failed_mess("The bet has to be between "+
                MSTRING(min_bet)+" and "+MSTRING(max_bet)+".\n");
            return 0;
        }

        game_data->bets = ({ value });
        data[TP] = game_data;
        raw_income += value;

        tell_object( TP, "You bet "+(string)MONEY_H->money_string(
            legal->query_money_array() )+".\n");
        tell_room( TO, TP->the_short()+" "+verbalize("bet")+" "+
            (string)MONEY_H->money_string( legal->query_money_array() )+
            ".\n", TP );

        legal->move("/room/rubbish");
        add_succeeded_mess("");
        return 1;
    }

    add_failed_mess("Unable to find any legal money to bet, sorry.\n");
    return 0;

} /* do_bet() */

/** @ignore yes */
void shuffle_deck() {
    int i, j;
    string card, tmp, *all;

    all = ({ });
    deck = ({ });
    used = ([ ]);

    // Assemble a single deck.
    foreach( card in keys(TYPES) )
        foreach( tmp in CARDS )
            all += ({ card + tmp });

    // This is a deck of many decks.
    for( i = 0; i < deck_amt; i++ )
            all += all;

    for( i = 0; i < deck_amt * 52; i++ ) {
        j = random(sizeof(all));
        deck += all[j..j];
        all = all[0..j-1] + all[j+1..];
    }

    tell_room( TO, dealer->the_short()+" produces "+
        query_num(deck_amt)+" new deck"+( deck_amt > 1 ? "s" : "")+" and "
        "shuffles the cards.\n");

} /* shuffle_deck() */

/** @ignore yes */
private void reset_game() {
    players = filter( players,(: $1 && interactive($1) :) );
    players += filter( pending, (: $1 && interactive($1) :) );

    if( !sizeof(players) ) {
        hand = 0;
        return;
    }

    hand = 1;
    pending = ({ });

    if( sizeof(deck) < 52 )
        shuffle_deck();

    dealer->do_command("\"Players, place your bets please.");

    if( !check_id )
        check_id = call_out( (: check_state :), 20 );

} /* reset_game() */

/** @ignore yes */
string get_name( int val ) {
    switch( val ) {
      case 2..10 :
        return CAP( query_num( val ) );
      default:
        return NAMES[val];
    }
} /* get_name() */

/** @ignore yes */
string card_name( string card ) {
    return add_a( get_name( to_int(card[1..2]) ) )+" of "+TYPES[card[0..0]];
} /* card_name() */

/** @ignore yes */
string *query_hand( object player, int i ) {
    if( !classp( data[player] ) )
        return ({ });

    if( i == 1 && data[player]->hand1 )
        return map( data[player]->hand1, (: card_name($1) :) );

    if( i == 2 && data[player]->hand2 )
        return map( data[player]->hand2, (: card_name($1) :) );

    return ({ });

} /* query_hand() */

/** @ignore yes */
string count_helper( int num ) {
    switch( num ) {
      case 0..2  : return "a couple";
      case 3..5  : return "a few";
      case 6..10 : return "several";
      default    : return "a bunch";
    }
} /* count_helper() */

/** @ignore yes */
int do_count() {
    string ret, name;

    if( !sizeof( used ) ) {
        add_failed_mess("No cards from this deck have been used yet.\n");
        return 0;
    }

    ret = "The following cards have been used from the currently used "
          "deck:\n";

    foreach( name in sort_array( keys(used), -1 ) )
        ret +=  sprintf("    %*'.'-s ", 18, get_name( to_int(name) ) )+
                count_helper( used[name] )+"\n";

    add_succeeded_mess("");
    tell_object( TP, ret );
    return 1;

} /* do_count() */

/** @ignore yes */
int do_status() {
    string ret;
    object player, *obs;
    int width;

    add_succeeded_mess("");

    if( !sizeof(players) && !sizeof(pending) ) {
        tell_object( TP, "No-one is playing blackjack right now.\n");
        return 1;
    }

    if( !hand ) {
        tell_object( TP, "There isn't a round going on at the moment.\n");
        return 1;
    }

    width = (int)TP->query_cols();

    ret = sprintf("%|*'-'s\n", width-1, "=========] Game Status [==========");

    if( sizeof(now_playing) ) {
        obs = ( member_array( TP, now_playing ) != -1 ? ({ TP }) : ({ }) );
        obs += now_playing - ({ TP });
        foreach( player in obs ) {
            ret += "$C$"+player->poss_short()+" first hand cards: "+
                   query_multiple_short( query_hand( player, 1 ) )+"\n";
            if( classp( data[player] ) && sizeof( data[player]->hand2 ) )
                ret += "$C$"+player->poss_short()+" second hand cards: "+
                   query_multiple_short( query_hand( player, 2 ) )+"\n";
            ret += "\n";
        }
        ret += "$C$"+dealer->poss_short()+" cards: "+
               query_multiple_short( query_hand( dealer, 1 ) )+"\n";
        if( objectp(cur_plr) )
            ret += sprintf("%*'-'|s\n", width-1, "> It is currently "+
                TP->convert_message( cur_plr->poss_short() )+" turn. <");
        tell_object( TP, ret );
        return 1;
    }

    tell_object( TP, "None of the players have any cards yet.\n");
    return 1;

} /* do_status() */

/** @ignore yes */
void do_say( mixed who, string message ) {
    dealer->do_command("sayto "+( pointerp(who) ?
        implode( map( who, (: $1->query_name() :) ), "," ) :
        who->query_name() )+" "+message );
} /* do_say() */

/** @ignore yes */
void pay_out( object player, int hnd ) {
    int tmp_bet, bet, eval;

    switch( hnd ) {
      case 0:
        eval = eval_cards( player, 1 );
        if( eval == BLACKJACK ) {
            bet = data[player]->bets[0];
            tmp_bet = bet * 3 / 2;
        } else {
            foreach( bet in data[player]->bets )
                tmp_bet += bet;
            bet = tmp_bet;
        }
        player->adjust_money( MARRAY( tmp_bet + bet ) );
        winnings_paid += tmp_bet;
        tell_object( player, "You receive "+MSTRING( tmp_bet )+" in "
            "winnings"+( eval == BLACKJACK ? " for the blackjack, plus your "
            "original bet" : ", plus the original bet for your cards")+".\n");
        tell_room( TO, player->the_short()+" "+verbalize(
            "receive winnings for their", "receives winnings for "+
            player->HIS )+" cards.\n", player );
        return save_play_data();
      case 1:
        bet = data[player]->bets[0];
        eval = eval_cards( player, 1 );
        tmp_bet = ( eval == BLACKJACK ? bet * 3 / 2 : bet );
        player->adjust_money( MARRAY( tmp_bet + bet ) );
        winnings_paid += tmp_bet;
        tell_object( player, "You receive "+MSTRING( tmp_bet )+" in "
            "winnings"+( eval == BLACKJACK ? " for the blackjack, plus your "
            "original bet" : ", plus the original bet for your first hand")+
            ".\n");
        tell_room( TO, player->the_short()+" "+verbalize("receive winnings "
            "for their", "receives winnings for "+player->HIS )+" first hand "
            "cards.\n", player );
        return save_play_data();
      case 2:
        bet = data[player]->bets[0];
        player->adjust_money( MARRAY( bet * 2 ) );
        winnings_paid += bet;
        tell_object( player, "You receive "+MSTRING( bet )+" in "
            "winnings, plus the original bet for your second hand.\n");
        tell_room( TO, player->the_short()+" "+verbalize("receive")+" "+
            "winnings for "+verbalize("their", player->HIS )+" second "+
            "hand cards.\n", player );
        return save_play_data();
    default:
        tell_object( player, "Something went wrong in paying out the winnings, "
            "please contact a creator immediately or send in a bug "
            "report.\n");
    }

} /* pay_out() */

/** @ignore yes */
void black_jack( object player ) {
    tell_room( TO, player->the_short()+" got a blackjack!\n");
    pay_out( player, 0 );
    map_delete( data, player );
    now_playing -= ({ player });
    cur_plr = 0;
} /* black_jack() */

/** @ignore yes */
void start_game() {
    object player;
    int eval;

    now_playing = filter( keys(data), (: $1 && interactive($1) :) );

    foreach( player in now_playing )
        deal_card(player);

    data[dealer] = new( class player_data );
    deal_card(dealer);

    foreach( player in now_playing )
        deal_card(player);

    eval = eval_cards( dealer, 1 );

    if( eval > 9 && eval < 12 ) {
        tell_room( TO, dealer->the_short()+" has a possibility for a "
             "blackjack so no blackjacks are paid out at this point.\n");
        return switch_player();
    }

    foreach( player in now_playing )
        if( eval_cards( player, 1 ) == BLACKJACK )
            black_jack(player);

    switch_player();

} /* start_game() */

/** @ignore yes */
varargs void check_state( int kick ) {
    object *obs;

    if( check_id )
        remove_call_out( check_id );

    check_id = 0;

    if( !sizeof(pending) && !sizeof(players) )
        return end_game();

    obs = filter( players, (: $1 && undefinedp( data[$1] ) :) );

    if( sizeof(obs) ) {
        if( kick > 5 && ( sizeof(pending) || sizeof( data ) ) ) {
            do_say( obs, "I'm afraid I am going to have to remove you "
                "from the game because we need to continue.");
            players -= obs;
        } else {
            if( kick > 10 ) {
                tell_room( TO, dealer->the_short()+" calls off the round "
                    "because none of the players are responding.\n");
                players = ({ });
                return end_game();
            }
            do_say( obs, "Please place your bet"+( sizeof(obs) > 1 ?
                "s" : "")+" so that we can continue with the game.");
            check_id = call_out( (: check_state :), 20, ++kick );
            return;
        }
    }

    if( sizeof(players) )
        return start_game();

    return end_game();

} /* check_state() */

/** @ignore yes */
private void end_it_all() {
    dealer = 0;
    players = ({ });
    pending = ({ });
    end_game();
} /* end_it_all() */

/** @ignore yes */
void event_death( object thing, object *people, object killer,
                  string room_mess, string killer_mess ) {
    if( !objectp(thing) )
        return;

    if( thing == dealer ) {
        if( sizeof(pending) || sizeof(players) ) {
            if( objectp(killer) ) {
                tell_object( killer, "You have killed the dealer and the "
                    "game is interrupted.\n");
                tell_room( TO, "Oh no!  "+killer->the_short()+" has killed "+
                    dealer->the_short()+" and the game is interrupted.\n",
                    killer );
            } else {
                tell_room( TO, "Oh no!  Someone has killed "+
                    dealer->the_short()+" and the game is interrupted.\n");
            }
        }
        return end_it_all();
    }

} /* event_death() */

/** @ignore yes */
void event_exit( object ob, string mess, object to ) {
    ::event_exit();

    if( ob == dealer ) {
        if( sizeof(pending) || sizeof(players) )
            tell_room( TO, dealer->the_short()+" leaves the room and the "
                "game is interrupted.\n");
        return end_it_all();
    }

    if( query_pending( ob ) || query_playing( ob ) ) {
        tell_object( ob, "You leave the blackjack table.\n");
        tell_room( TO, ob->one_short()+" "+verbalize("leave")+" the "
            "blackjack table.\n", ob );

        remove_player(ob);

        if( ob == cur_plr )
            return switch_player();

        if( !sizeof(players) )
            return end_game();
    }

} /* event_exit() */

/** @ignore yes */
int card_val( string card ) {
    int val;
    
    val = to_int( card[1..] );

    switch( val ) {
      case 2..9:
        return val;
      case 10..13:
        return 10;
      case 14:
        return 11;
      default:
        return 0;
    }

} /* card_val() */

/** @ignore yes */
varargs int split_ok( object player, int flag ) {
    if( !data[player]->hand1 ) {
        if( flag )
            add_failed_mess("You can't split your hand because you don't "
                "appear to have any cards.\n");
        return 0;
    }

    if( sizeof( data[player]->hand1 ) > 2 ) {
        if( flag )
            add_failed_mess("You can only split a hand of two equal "
                "cards.\n");
        return 0;
    }

    if( data[player]->hand2 ) {
        if( flag )
            add_failed_mess("You have already split or doubled your hand "
                "this round.\n");
        return 0;
    }

    if( card_val( data[player]->hand1[0] ) ==
        card_val( data[player]->hand1[1] ) )
        return 1;

    if( flag )
        add_failed_mess("You can only split a hand of two equal cards.\n");

    return 0;

} /* split_ok() */

/** @ignore yes */
varargs int double_ok( object player, int flag ) {
    int eval;

    if( !data[player]->hand1 ) {
        if( flag )
            add_failed_mess("You can't double your hand because you don't "
                "appear to have any cards.\n");
        return 0;
    }

    if( sizeof( data[player]->hand1 ) > 2 ) {
        if( flag )
            add_failed_mess("You cannot double your hand unless you have "
                "two cards only, and the sum of their values is ten or "
                "eleven.\n");
        return 0;
    }

    if( data[player]->hand2 ) {
        if( flag )
            add_failed_mess("You have already split or doubled your hand "
                "this round.\n");
        return 0;
    }

    eval = eval_cards( player, 1 );

    if( eval > 9 && eval < 12 )
        return 1;

    if( flag )
        add_failed_mess("You cannot double your hand unless you have "
            "two cards only, and the sum of their values is ten or "
            "eleven.\n");

    return 0;

} /* double_ok() */

/** @ignore yes */
void do_announce( object player, int flag ) {
    string ret;

    if( flag ) {
        do_say( player, "You can either hit or stand.  Please choose one.");
        return;
    }

    if( split_ok(player) && double_ok(player) )
        ret = ", stand, split or double";

    if( split_ok(player) && !double_ok(player) )
        ret = ", stand or split";

    if( !split_ok(player) && !double_ok(player) )
        ret = " or stand";

    if( !split_ok(player) && double_ok(player) )
        ret = ", stand or double";

    do_say( player, "You can either hit"+ret+".  Please choose one.");

} /* do_announce() */

/** @ignore yes */
varargs void main_game( object player, int kick ) {
    if( kick > 1 && ( sizeof(players) + sizeof(pending) ) > 1 ) {
        do_say( player, "If you're here to idle, then be my guest, but I am "
            "going to have to remove you from the game.");
        remove_player(player);
        return switch_player();
    }

    if( kick > 6 ) {
        do_say( player, "I don't have the patience to wait after you "
            "forever, so I am going to remove you from the game.");
        remove_player(player);
        return switch_player();
    }

    do_announce( player, 0 );
    main_id = call_out( (: main_game :), 30, player, ++kick );

} /* main_game() */

/** @ignore yes */
int check_player() {
    if( query_pending(TP) ) {
        add_failed_mess("You are waiting for a new round to begin.\n");
        return 0;
    }

    if( !query_playing(TP) ) {
        add_failed_mess("You are not playing blackjack.  Please join the "
            "game first.\n");
        return 0;
    }

    if( !hand ) {
        add_failed_mess("You have to wait for a new round to begin.\n");
        return 0;
    }

    if( !sizeof(data) || !data[TP] ) {
        add_failed_mess("You have not placed a bet yet.\n");
        return 0;
    }

    if( member_array( TP, now_playing ) == -1 ) {
        add_failed_mess("You don't have any cards yet.\n");
        return 0;
    }

    if( TP != cur_plr ) {
        add_failed_mess("It is not your turn right now.\n");
        return 0;
    }

    return 1;

} /* check_player() */

/** @ignore yes */
int do_split() {
    if( !check_player() )
        return 0;

    if( split_ok( TP, 1 ) ) {
        if( (int)TP->query_value_in( query_place() ) >= data[TP]->bets[0] ) {
            data[TP]->hand2 = ({ data[TP]->hand1[1] });
            data[TP]->hand1 = ({ data[TP]->hand1[0] });

            hand = 1;
            doing_split = 1;

            data[TP]->bets += ({ data[TP]->bets[0] });
            TP->pay_money( MARRAY( data[TP]->bets[0] ) );

            add_succeeded_mess("");

            tell_object( TP, "You decide to split your hand and double the "
                "bet, tossing the money onto the table with a tinkle.\n");
            tell_room( TO, TP->the_short()+" decides to split "+
                TP->HIS+" hand and double the bet, tossing the money onto "
                "the table with a tinkle.\n", TP );

            do_say( TP, "Let's start with your first hand.");
            hands( TP, 1 );
            if( main_id ) {
                remove_call_out( main_id );
                main_id = 0;
            }
            main_game( TP );
            return 1;
        }

        add_failed_mess("You do not have enough money to split your hand.\n");
        return 0;
    }

    return 0;

} /* do_split() */

/* @ignore yes */
int do_double() {
    if( !check_player() )
        return 0;

    if( doing_split ) {
        add_failed_mess("You cannot double a splitted hand.\n");
        return 0;
    }

    if( double_ok( TP, 1 ) ) {
        if( (int)TP->query_value_in( query_place() ) >= data[TP]->bets[0] ) {
            TP->pay_money( MARRAY( data[TP]->bets[0] ) );

            add_succeeded_mess("");
            tell_object( TP, "You double your bet, tossing the money onto "
                "the table with a tinkle, and receive one more card.\n");
            tell_room( TO, TP->the_short()+" doubles "+TP->HIS+" bet, "
                "tossing the money onto the table with a tinkle, and "
                "receives one more card.\n", TP );

            deal_card(TP);
            data[TP]->bets += ({ data[TP]->bets[0] });

            if( eval_cards( TP, 1 ) == BUSTED ) {
                do_say( TP, "You're busted, better luck next round.");
                data[TP]->bets = ({ });
            }

            switch_player();
            return 1;
        }

        add_failed_mess("You do not have enough money to double your hand.\n");
        return 0;
    }

    return 0;

} /* do_double() */

/** @ignore yes */
int do_stand() {
    if( !check_player() )
        return 0;

    add_succeeded_mess("");

    if( doing_split ) {
        if( hand == 1 ) {
            tell_object( TP, "You are satisfied with your first hand.\n");
            tell_room( TO, TP->the_short()+" is satisfied with "+
                TP->HIS+" first hand.\n", TP );
            do_say( TP, "Let's have a look at your second hand now.");
            hand = 2;
            hands( TP, 2 );
            do_announce( TP, 0 );
            if( main_id )
                remove_call_out( main_id );
            main_id = call_out( (: main_game :), 30, TP );
            return 1;
        } else if( hand == 2 ) {
            tell_object( TP, "You are satisfied with your second hand.  "
                "Please wait until the round is finished.\n");
            tell_room( TO, TP->the_short()+" is satisfied with "+
                TP->HIS+" second hand.\n", TP );
            hand = 1;
            doing_split = 0;
            switch_player();
            return 1;
        }
        add_failed_mess("Something has gone wrong, please contact a creator "
            "immediately or file a bug report.\n");
        return 0;
    }

    tell_object( TP, "You are satisfied with your cards.  Please wait until "
        "the round is finished.\n");
    tell_room( TO, TP->the_short()+" is satisfied with "+
        TP->HIS+" cards.\n", TP );

    switch_player();
    return 1;

} /* do_stand */

/** @ignore yes */
varargs void hands( object player, int which ) {
    string ret, *handt, tmp;
    int aces, i, val;

    if( !sizeof(data) || !data[player] ||
        ( !data[player]->hand1 && !data[player]->hand2 ) ) {
        tell_object( player, "You don't have any cards at the moment.\n");
        return;
    }

    ret = "";

    if( ( handt = data[player]->hand1 ) && ( !which || which == 1 ) ) {
        if( !which )
            ret += "You have the following cards:\n";
        ret += "Hand one: "+query_multiple_short( map( handt,
               (: card_name($1) :) ) )+".\n";
        ret += "Value of the hand: %^WHITE%^";

        aces = 0;
        val = 0;

        foreach( tmp in handt ) {
            val += ( i = card_val( tmp ) );
            if( i == 11 )
                aces++;
        }

        for( i = 0; i < aces; i++ ) {
            if( val > 21 )
                val -= 10;
            else if( i < aces ) {
                i = eval_cards( player, 1 );
                if( i != BLACKJACK && i != BUSTED )
                    ret += CAP( query_num(i-10) ) + " / ";
                break;
            }
        }
        ret += eval_cards( player, 1, 1 )+"%^RESET%^";
        if( which == 1 ) {
            tell_object( player, ret+"\n");
            return;
        }
    }

    if( ( handt = data[player]->hand2 ) && ( !which || which == 2 ) ) {
        if( !which )
            ret += "\n";
        ret += "Hand two: "+
               query_multiple_short( map( handt, (: card_name($1) :) ) )+".\n";
        ret += "Value of the hand: %^WHITE%^";

        aces = 0;
        val = 0;

        foreach( tmp in handt ) {
            val += ( i = card_val( tmp ) );
            if( i == 11 )
                aces++;
        }

        for( i = 0; i < aces; i++ ) {
            if( val > 21 )
                val -= 10;
            else if( i < aces ) {
                i = eval_cards( player, 2 );
                if( i != BLACKJACK && i != BUSTED )
                    ret += CAP( query_num(i-10) ) + " / ";
                break;
            }
        }
        ret += eval_cards( player, 2, 1 )+"%^RESET%^";
        if( which == 2 ) {
            tell_object( player, ret+"\n");
            return;
        }
    }

    if( ret == "" )
        ret += "You don't have any cards at the moment.\n";

    tell_object( player, ret+"\n");

} /* hands() */

/** @ignore yes */
int do_cards() {
    if( !query_playing(TP) && !query_pending(TP) ) {
        add_failed_mess("You are not playing blackjack and don't have "
            "any cards.\n");
        return 0;
    }

    hands(TP);
    add_succeeded_mess("");
    return 1;

} /* do_cards() */

/** @ignore yes */
int do_hit() {
    int eval;

    if( !check_player() )
        return 0;

    tell_object( TP, "You request another card from "+dealer->the_short()+".\n");
    tell_room( TO, TP->one_short()+" requests another card from "+
        dealer->the_short()+".\n", TP );

    add_succeeded_mess("");
    deal_card(TP);

    eval = eval_cards( TP, hand );

    if( doing_split ) {
        if( eval == BUSTED && hand == 1 ) {
            hand = 2;
            do_say( TP, "You have busted your first hand.  Let's have a look "
                "at your second hand now.");
            hands( TP, 2 );
            data[TP]->bets = ({ data[TP]->bets[0] });
            do_announce( TP, 1 );
            if( main_id )
                remove_call_out( main_id );
            main_id = call_out( (: main_game :), 30, TP );
            return 1;
        }

        if( eval == BUSTED && hand == 2 ) {
            do_say( TP, "Your second hand is busted, better luck next "
                "round.");
            if( sizeof( data[TP]->bets ) > 1 )
                data[TP]->bets = ({ data[TP]->bets[0] });
            else
                map_delete( data, TP );
            hand = 1;
            doing_split = 0;
            switch_player();
            return 1;
        }

        if( eval > 0 && eval < 22 ) {
            hands( TP, hand );
            do_announce( TP, 1 );
            if( main_id )
                remove_call_out( main_id );
            main_id = call_out( (: main_game :), 30, TP );
            return 1;
        }

        add_failed_mess("Something has gone wrong, please contact a creator "
            "immediately, or file a bug report.\n");
        return 0;
    }

    if( eval == BUSTED ) {
        do_say( TP, "You're busted, better luck next round.");

        if( sizeof(now_playing) == 1 ) {
            tell_room( TO, "All players are busted and "+
                dealer->the_short()+" clears the table.\n");
            end_game();
            return 1;
        }

        map_delete( data, TP );
        switch_player();
        return 1;
    }

    hands(TP);
    do_announce( TP, 1 );
    if( main_id )
        remove_call_out( main_id );
    main_id = call_out( (: main_game :), 30, TP );
    return 1;

} /* do_hit() */

/** @ignore yes */
void deal_card( object player ) {
    string *cards;

    if( !sizeof(deck) )
        shuffle_deck();

    if( hand == 1 ) {
        cards = data[player]->hand1 || ({ });
        cards += ({ deck[0] });
        data[player]->hand1 = cards;
    }

    if( hand == 2 ) {
        cards = data[player]->hand2 || ({ });
        cards += ({ deck[0] });
        data[player]->hand2 = cards;
    }

    tell_room( TO, dealer->the_short()+" deals "+( player == dealer ?
        dealer->HIM+"self" : player->one_short() )+" "+
        card_name( deck[0] )+".\n");

    used[ deck[0][1..] ]++;
    deck -= ({ deck[0] });

} /* deal_card() */

/** @ignore yes */
varargs mixed eval_cards( object player, int hand_b, int text ) {
    string *cards, card;
    int i, aces, val;

    if( hand_b == 1 )
        cards = data[player]->hand1;
    if( hand_b == 2 )
        cards = data[player]->hand2;

    foreach( card in cards ) {
        val += ( i = card_val( card ) );
        if( i == 11 )
            aces++;
    }

    if( aces ) {
        for( i = 0; i < aces; i++ ) {
            if( val > 21 )
                val -= 10;
        }
    }

    if( text ) {
        if( val > 21 )
            return "Busted";
        if( val == 21 && sizeof( data[player]->hand1 ) < 3 &&
            !data[player]->hand2 )
            return "Blackjack";
        return CAP( query_num( val ) );
    }

    if( val > 21 )
        return BUSTED;

    if( val == 21 && sizeof( data[player]->hand1 ) < 3 &&
        !data[player]->hand2 )
        return BLACKJACK;

    return val;

} /* eval_cards() */

/** @ignore yes */
void settle_game() {
    int pval, dval;
    string money;
    object player;
    class player_data game_data;

    if( main_id ) {
        remove_call_out( main_id );
        main_id = 0;
    }

    dval = eval_cards( dealer, 1 );

    if( dval == BLACKJACK ) {
        tell_room( TO, dealer->the_short()+" got a blackjack and all bets "
            "are lost.\n");
        return end_game();
    }

    foreach( player, game_data in data ) {
        if( player == dealer )
            continue;

        pval = game_data->bets[0];
        if( sizeof(game_data->bets) == 2 )
            pval += game_data->bets[1];

        money = MSTRING( pval );

        if( game_data->hand1 ) {
            pval = eval_cards( player, 1 );
            switch( pval ) {
              case BUSTED:
                tell_object( player, "Your"+( game_data->hand2 ? " first" :
                    "")+" hand was busted.\n");
                break;
              case BLACKJACK:
                black_jack(player);
                break;
              default:
                if( pval > dval ) {
                    tell_object( player, "Your"+( game_data->hand2 ? " first" :
                        "")+" hand with a value of "+query_num(pval)+" beats "+
                        dealer->poss_short()+" first hand, which has a value "
                        "of "+query_num(dval)+".\n");
                    pay_out( player, !game_data->hand2 ? 0 : 1 );
                    break;
                }
                tell_object( player, "Your"+( game_data->hand2 ? " first" :
                    "")+" hand "+( pval == dval ? "with a value of "+
                    query_num(pval)+" is tied with "+
                    dealer->poss_short()+" cards" : "is valued at "+
                    query_num(pval)+" and "+dealer->poss_short()+" first "
                    "hand is valued at "+query_num(dval) )+".  You lose your "
                    "bet of "+money+".\n");
                tell_room( TO, player->the_short()+" "+verbalize("lose their "
                    "bets for their", "loses "+player->HIS+" bet for "+
                    player->HIS )+" "+( game_data->hand2 ? "first hand" : "")+
                    "cards.\n", player );
            }
        }

        if( game_data->hand2 ) {
            pval = eval_cards( player, 2 );
            switch( pval ) {
              case BUSTED:
                tell_object( player, "Your second hand was busted.\n");
              break;
              default:
                if( pval > dval ) {
                    tell_object( player, "Your second hand with a value of "+
                        query_num(pval)+" beats "+dealer->poss_short()+" second "
                        "hand, which has a value of "+query_num(dval)+".\n");
                    pay_out( player, 2 );
                    break;
                }
                tell_object( player, "Your second hand "+( pval == dval ?
                    "with a value of "+query_num(pval)+" is tied with "+
                    dealer->poss_short()+" cards" : "is valued at "+
                    query_num(pval)+" and "+dealer->poss_short()+" first "
                    "hand is valued at "+query_num(dval) )+".  You lose your "
                    "bet of "+money+".\n");
                tell_room( TO, player->the_short()+" "+verbalize("lose their "
                    "bets for their", "loses "+player->HIS+" bet for "+
                    player->HIS )+" second hand cards.\n", player );
            }
        }
    }

    tell_room( TO, dealer->the_short()+" settles the game collecting all "
        "cards.\n");
    end_game();

} /* settle_game() */

/** @ignore yes */
private void dealer_game() {
    int tmp_val, aces, i;
    string card;
    object player;

    tmp_val = eval_cards( dealer, 1 );

    if( tmp_val == BUSTED ) {
        tell_room( TO, dealer->the_short()+" is busted and settles "
            "the game, paying winnings to all winning players.\n");

        foreach( player in keys(data) ) {
            if( player != dealer )
                pay_out( player, 0 );
        }
        return end_game();
    }

    if( tmp_val == BLACKJACK )
        return settle_game();

    if( tmp_val < 17 ) {
        deal_card(dealer);
        return dealer_game();
    }

    foreach( card in data[dealer]->hand1 ) {
        if( ( i = card_val( card ) ) == 11 ) {
            aces++;
            tmp_val--;
        } else {
            tmp_val -= i;
        }
    }

    if( aces && !tmp_val )
        return settle_game();

    if( aces && tmp_val > 0 ) {
        deal_card( dealer );
        return dealer_game();
    }

    return settle_game();

} /* dealer_game() */

/** @ignore yes */
void switch_player() {
    object player;

    if( main_id ) {
        remove_call_out( main_id );
        main_id = 0;
    }

    if( !sizeof(now_playing) ) {
        tell_room( TO, dealer->the_short()+" clears the table as there is "
            "no-one left playing"+( sizeof(players) ? " this round" : "")+
            ".\n");
        return end_game();
    }

    foreach( player in now_playing ) {
        if( member_array( player, played ) == -1 ) {
            cur_plr = player;
            break;
        }
    }

    if( sizeof(played) == sizeof(now_playing) && !sizeof(data) ) {
        tell_room( TO, "All players are busted and "+
            dealer->the_short()+" clears the table.\n");
        return end_game();
    }

    if( sizeof(played) == sizeof(now_playing) ) {
        cur_plr = dealer;
        tell_room( TO, "It is now "+dealer->poss_short()+" turn.\n");
        return dealer_game();
    }

    tell_room( TO, "It is now "+cur_plr->poss_short()+" turn.\n");
    hands( cur_plr );
    played += ({ cur_plr });

    if( eval_cards( cur_plr, 1 ) == BLACKJACK ) {
        do_say( cur_plr, "You already have a blackjack, so we can skip you.");
        dealer->do_command("beam");
        return switch_player();
    }

    do_announce( cur_plr, 0 );
    main_id = call_out( (: main_game :), 30, cur_plr );

} /* switch_player() */

/** @ignore yes */
int do_statistics() {
    string ret;

    if( !creatorp(TP) ) {
        add_failed_mess("Sorry, only creators can check the game "
            "statistics.\n");
        return 0;
    }

    ret =  "Statistics:\n";
    ret += "Raw income      : "+MSTRING( raw_income )+" ("+raw_income+").\n";
    ret += "Total Winnings  : "+MSTRING( winnings_paid )+" ("+
           winnings_paid+").\n";
    ret += "Profit made     : "+MSTRING( raw_income - winnings_paid )+" ("+
           ( raw_income - winnings_paid )+").\n";
    ret += sprintf("Approximately %i percent of the cash used to play "
           "blackjack has been returned as winnings to the players.\n",
           ( raw_income ? 100 * winnings_paid / raw_income : 0 ) );

    tell_object( TP, ret );
    add_succeeded_mess("");
    return 1;

} /* do_statistics() */

/** @ignore yes */
string extra_look( object ob ) {
    string ret;

    if( ob != TO )
        return "";

    ret = "";

    if( sizeof(players) )
        ret += "$C$"+query_multiple_short( players )+" "+
             ( sizeof(players) == 1 && players[0] != TP ? "is" : "are")+
             " playing cards at the blackjack table";

    if( sizeof(pending) )
        ret += ( ret != "" ? " and " : "$C$")+
            query_multiple_short( pending )+" "+( sizeof(pending) == 1 &&
            pending[0] != TP ? "is" : "are")+" anxious to get into the "
            "game next round";

    return ( ret != "" ? ret+".\n" : "" );

} /* extra_look() */

/** @ignore yes */
mixed stats() {
    return ::stats() + ({
        ({ "raw income", raw_income }),
        ({ "total winnings", winnings_paid }),
        ({ "profit made", raw_income - winnings_paid }),
        ({ "profit percentage", raw_income ?
            100 * winnings_paid / raw_income : 0 }),
        });
} /* stats() */
