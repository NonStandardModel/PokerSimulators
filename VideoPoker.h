///////////////////////////////////////////////////////////////////////////////////////////////////
//
// VideoPoker.h
// Copyright (C) 2001 Eric Farmer
//
// Video Poker hand evaluator.  Contains a class for shuffling and dealing video poker hands,
// ranking a given video poker hand, as well as computing expected returns for all possible
// discards.
//
// This program is free software; you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, version 2 or later.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY,
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
// the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program; if
// not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
// Boston, MA  02111-1307 USA.
//

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Hand ranks range from High_Card (0) to Royal_Flush (9).  Note that One_Pair actually indicates a
// pair of sufficient rank (specified by min_pair_rank in the Video_Poker constructor), and
// High_Card indicates either a high card hand or a "low" pair.
//

enum Hand_Rank {High_Card, One_Pair, Two_Pair, Three_Of_A_Kind, Straight, Flush, Full_House,
    Four_Of_A_Kind, Straight_Flush, Royal_Flush};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// A Video_Poker object contains a 52-card deck, which may be shuffled for dealing video poker
// hands, ranking a given video poker hand, as well as computing expected returns for all possible
// discards.
//

class Video_Poker {
public:

// Video_Poker(min_pair_rank) prepares for evaluating video poker hands, where min_pair_rank
// indicates the lowest paying rank of a one pair hand, and ranges from deuce (2) through ace (14).

    Video_Poker(int min_pair_rank);

// Shuffle() shuffles the deck; the resulting hand consists of the "top" 5 cards of the shuffled
// deck.

    void Shuffle();

// Get_Hand(hand) copies the current hand into the given array.  Each card is represented by an
// integer with exactly 2 bits set, one each for rank and suit.  Rank is indicates by bits 0
// (deuce) through 12 (ace), and suit is indicated by bits 13 through 16.

    void Get_Hand(int hand[]);

// Discard(discard_code) discards from the current hand and deals the resulting new hand.  The
// discard_code is an integer, where the bit in position i (0 through 4) is set iff the card in
// position i is to be discarded.

    void Discard(int discard_code);

// Rank_Hand(hand) returns the rank of the given hand.

    int Rank_Hand(const int hand[]);

// Evaluate_Hand(pay_table) computes the expected returns for all possible discards of the current
// hand, returning the discard code with the greatest expected return.  Returns are computing using
// the given pay_table, where pay_table[rank] is the payoff for a hand of the given rank.  (Note
// that usually pay_table[High_Card] == 0.)

    int Evaluate_Hand(const int pay_table[]);

// Get_Return(discard_code) returns the expected return for the given discard, for the current
// hand.  The return value is valid only after calling Evaluate_Hand().

    double Get_Return(int discard_code);

// Get_Hand_Count(discard_code, rank) returns the number of ways a hand of the given rank may be
// drawn from the given discard, for the current hand.  The return value is valid only after
// calling Evaluate_Hand().

    int Get_Hand_Count(int discard_code, int rank);

// Get_Discard_Total(discard_code) returns the total number of new hands resulting from the given
// discard.

    int Get_Discard_Total(int discard_code);

// Get_Rank_Total(rank) returns the total number of hands of the given rank.

    int Get_Rank_Total(int rank);

///////////////////////////////////////////////////////////////////////////////////////////////////
// added part (by MZ)
//	void set_deck(int* my_deck);
	void set_deck(int* my_deck, int num_cards_to_set);
	int count_high_bits(int x);
	bool detect_togle(int x, int y, int position);
	void print_bits(int x);
	int MZ_Rank_hand(const int hand[]);
	long long hand_to_64bit(const int hand[]);
	void MZ_Card_Mixxer(const int deck[], int* start, int* end);

//	vector<int> detect_bit_toggle(int hand[]);
///////////////////////////////////////////////////////////////////////////////////////////////////

private:
    int deck[52],
        pair_rank_mask,
        distinct_table[8192],
        hand_counts[32][10],
        discard_totals[32],
        rank_totals[10];
    double returns[32];

    void swap(int i, int j);
};
