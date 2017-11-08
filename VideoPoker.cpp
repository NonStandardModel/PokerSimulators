///////////////////////////////////////////////////////////////////////////////////////////////////
//
// VideoPoker.cpp
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

#include "VideoPoker.h"
#include <stdlib.h>
#include <time.h>

// added by MZ
#include <iostream>
# include <vector>
#include <algorithm>
#include <random>
#include <chrono>
//using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Video_Poker
//

Video_Poker::Video_Poker(int min_pair_rank) {
    for (int cards = 0, suit = 0; suit < 4; suit++)             // initialize deck
        for (int rank = 2; rank <= 14; rank++, cards++)
            deck[cards] = (1 << (rank - 2)) | (1 << 13 << suit);
    srand(time(NULL));                                          // seed random number generator

    pair_rank_mask = (-1 << (min_pair_rank - 2))&0x1fff;        // save bit mask for ranking one
                                                                // pair hands

// Create a lookup table, to be indexed by the bitwise or of the card ranks, indicating (mostly)
// the number of distinct ranks in the hand.  Five distinct ranks are a special case, where
// straights (and ace-high straights in particular, for royal flushes) are quickly recognized.

    for (int rank_or = 0; rank_or < 8192; rank_or++) {
        int distinct = 0,                                       // count distinct ranks (set bits)
            straight_rank = 1;                                  // high card in a straight
        bool straight = false,
            done = false;
        for (int temp = rank_or; temp; temp >>= 1)
            if (temp&1) {
                distinct++;
                straight_rank++;
                straight = !done;
            }
            else
                if (straight)
                    done = true;
                else
                    straight_rank++;
        if (distinct == 5)                                      // for 5 distinct ranks, check for
            if (straight) {                                     // straights (also ace-high)
                distinct++;
                if (straight_rank == 14)
                    distinct++;
            }
        distinct_table[rank_or] = distinct;
    }
    distinct_table[4111] = 6;                                   // A-2-3-4-5 isn't a run of ones

// Get_Discard_Total() simply returns the appropriate binomial coefficient (47 choose d).

    int combinations[6] = {1, 47, 1081, 16215, 178365, 1533939};
    for (int discard_code = 0; discard_code < 32; discard_code++) {
        int num_discards = 0;
        for (int temp = discard_code; temp; temp >>= 1)
            if (temp&1)
                num_discards++;
        discard_totals[discard_code] = combinations[num_discards];
    }

// Get_Rank_Total() isn't just a convenience; rather than enumerating all possible hands from
// discarding all 5, we can simply subtract the hands from all other discards from this total.

    int poker_rank_totals[10] = {1302540, 1098240, 123552, 54912, 10200, 5108, 3744, 624, 36, 4},
        low_pairs = 84480*(min_pair_rank - 2);
    for (int rank = High_Card; rank <= Royal_Flush; rank++)
        rank_totals[rank] = poker_rank_totals[rank];
    rank_totals[High_Card] += low_pairs;
    rank_totals[One_Pair] -= low_pairs;
}

void Video_Poker::Shuffle() {
    for (int i = 52; i > 1; i--)
        swap(i - 1,rand()%i);
}

void Video_Poker::Get_Hand(int hand[]) {
    for (int i = 0; i < 5; i++)
        hand[i] = deck[i];
}

void Video_Poker::Discard(int discard_code) {
    for (int top_card = 5, i = 0; i < 5; i++)
        if (discard_code&(1 << i))
            swap(i, top_card++);
}

int Video_Poker::Rank_Hand(const int hand[]) {
    int rank,
        card_or = hand[0] | hand[1] | hand[2] | hand[3] | hand[4],  // bitwise or of ranks/suits
        rank_or = card_or&0x1fff,                                   // just of ranks
        distinct = distinct_table[rank_or],                         // fast table lookup
        card_xor,
        num_cards;

    switch (distinct) {
    case 5  :   if (distinct_table[card_or >> 13] == 1)             // if there's only one suit,
                    rank = Flush;                                   // it's a flush
                else
                    rank = High_Card;                               // straights are elsewhere
                break;
    case 4  :   card_xor = hand[0] ^ hand[1] ^ hand[2] ^ hand[3] ^ hand[4];
                if ((rank_or^card_xor)&pair_rank_mask)              // check if pair is of
                    rank = One_Pair;                                // sufficient rank
                else
                    rank = High_Card;
                break;
    case 3  :   card_xor = hand[0] ^ hand[1] ^ hand[2] ^ hand[3] ^ hand[4];
                if (rank_or^(card_xor&0x1fff))
                    rank = Two_Pair;
                else
                    rank = Three_Of_A_Kind;
                break;
    case 6  :   if (distinct_table[card_or >> 13] == 1)             // non-ace-high straights;
                    rank = Straight_Flush;                          // check for flush again
                else
                    rank = Straight;
                break;

// Distinguishing between four of a kind and full house seems to be the most difficult.  Both have
// the same signature for both bitwise OR and XOR.

    case 2  :   card_xor = (hand[0] ^ hand[1] ^ hand[2] ^ hand[3] ^ hand[4])&0x1fff;
                num_cards = 0;
                if (hand[0]&card_xor) num_cards++;
                if (hand[1]&card_xor) num_cards++;
                if (hand[2]&card_xor) num_cards++;
                if (hand[3]&card_xor) num_cards++;
                if (hand[4]&card_xor) num_cards++;
                if (num_cards == 1)
                    rank = Four_Of_A_Kind;
                else
                    rank = Full_House;
                break;
    case 7  :   if (distinct_table[card_or >> 13] == 1)
                    rank = Royal_Flush;
                else
                    rank = Straight;
    }
    return rank;
}

int Video_Poker::Evaluate_Hand(const int pay_table[]) {
    int discard_code;
    for (discard_code = 0; discard_code < 31; discard_code++)   // clear count table
        for (int rank = High_Card; rank <= Royal_Flush; rank++)
            hand_counts[discard_code][rank] = 0;

    hand_counts[0][Rank_Hand(deck)]++;                          // holding all 5 is easy

// As described above, we enumerate hands for all discards except discarding all 5, which would
// take the most time.

    for (discard_code = 1; discard_code < 31; discard_code++) {
        int hand[5],
            discards[5],
            num_discards = 0;

        for (int i = 0, temp = discard_code; i < 5; i++, temp >>= 1) {
            hand[i] = deck[i];                                  // re-copy current hand
            if (temp&1)                                         // determine discard positions
                discards[num_discards++] = i;
        }

        int draw[5] = {5, 6, 7, 8, 9},                          // don't draw cards from current
            change = 0;                                         // hand
        do {
            while (change < num_discards) {                     // draw another subset
                hand[discards[change]] = deck[draw[change]];
                change++;
            }
            hand_counts[discard_code][Rank_Hand(hand)]++;       // and evaluate resulting hand
            
            int j, n = 52;                                      // compute next subset
            change = num_discards;
            while ((j = draw[--change]) == --n && change);
            if (j++ == n) j = 0;
            int i = change;
            while (i < 5) draw[i++] = j++;
        } while (change || draw[0]);
    }

    for (int rank = High_Card; rank <= Royal_Flush; rank++) {   // subtract results so far from
        int total = rank_totals[rank];                          // rank totals for discarding 5
        for (discard_code = 0; discard_code < 31; discard_code++)
            total -= hand_counts[discard_code][rank];
        hand_counts[31][rank] = total;
    }

    for (discard_code = 0; discard_code < 32; discard_code++) { // compute expected returns
        returns[discard_code] = 0;
        for (int rank = High_Card; rank <= Royal_Flush; rank++)
            returns[discard_code] += (double)pay_table[rank]*hand_counts[discard_code][rank];
        returns[discard_code] /= discard_totals[discard_code];
    }

    int best_discard = 0;                                       // find discard maximizing expected
    double max_return = returns[0];                             // return
    for (discard_code = 1; discard_code < 32; discard_code++)
        if (returns[discard_code] > max_return) {
            max_return = returns[discard_code];
            best_discard = discard_code;
        }
    return best_discard;
}

double Video_Poker::Get_Return(int discard_code) {
    return returns[discard_code];
}

int Video_Poker::Get_Hand_Count(int discard_code, int rank) {
    return hand_counts[discard_code][rank];
}

int Video_Poker::Get_Discard_Total(int discard_code) {
    return discard_totals[discard_code];
}

int Video_Poker::Get_Rank_Total(int rank) {
    return rank_totals[rank];
}

void Video_Poker::swap(int i, int j) {
    int temp = deck[i];
    deck[i] = deck[j];
    deck[j] = temp;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// added part (by MZ)
//void Video_Poker::set_deck(int* my_deck) { // THIS IS NOT WORKING
//	for (int idx = 0; idx < sizeof(my_deck)/sizeof(*my_deck); idx++) {
//		deck[idx] = my_deck[idx];
//		cout << "\nFrom set_deck...-> " << sizeof(my_deck);//sizeof(*my_deck) << endl;

//	}
//}

void Video_Poker::set_deck(int* my_deck, int num_cards_to_set) {
	for (int idx = 0; idx < num_cards_to_set; idx++) {
		deck[idx] = my_deck[idx];
	}
}

int Video_Poker::count_high_bits(int x) {
	int counter = 0;
	for (int i = 0; i < 8*sizeof(x); i++) {
		if ((x >> i) & 0x01) {
			counter++;
		}
	}
	return counter;
}

bool Video_Poker::detect_togle(int x, int y, int position) {
	if (((x >> position) & 0x01) == ((y >> position) & 0x01)) {
		return false;
	}
	return true;
}

void Video_Poker::print_bits(int x){
	std::cout << "Bit representation for number " << x << " is: ";
	for (int i = 0; i < 8*sizeof(x); i++) {
		if ((x >> i) & 0x01 == 0x01) {
			std::cout << "1";
		} else {
			std::cout << "0";
		}
	}
	std::cout << std::endl;
}

long long Video_Poker::hand_to_64bit(const int hand[]) {
	int value; // value of card: 2,3,4,...,10,11 = J,12 = Q,13 = K, 14 = A
	int suite; // suite: 1,2,3,4
	long long result = 0;

	int mask_value = ((1 << 13) - 1); // MOVE OUT
	int mask_suite = ((1 << 17) - 1) ^ ((1 << 13) - 1); // MOVE OUT

	for (int i = 0; i < 5; i++) {
		switch (hand[i] & mask_value) {
			case 1:	value = 2;
				break;
			case 2:	value = 3;
				break;
			case 4:	value = 4;
				break;
			case 8:	value = 5;
				break;
			case 16:	value = 6;
				break;
			case 32:	value = 7;
				break;
			case 64:	value = 8;
				break;
			case 128:	value = 9;
				break;
			case 256:	value = 10;
				break;
			case 512:	value = 11;
				break;
			case 1024:	value = 12;
				break;
			case 2048:	value = 13;
				break;
			case 4096:	value = 14;
				break;
//			default:	value = 15; // JOKER
//				break;
		}
		switch (hand[i] & mask_suite) {
			case 8192: suite = 1;  // suite 1
				break;
			case 16384: suite = 2; // suite 2
				break;
			case 32768: suite = 3; // suite 3
				break;
			case 65536: suite = 4; // suite 4
				break;
		}
		result += (1ULL << (4*(value - 2) + (suite - 1))); // NOT OK...WILL NOT SHIFT BITS FOR > 32 bits .. FIXED
	}

//	//DEBUG
//	for (int i = 0; i < 64; i++) {
//		if (i % 4 == 0) {std::cout << "|";}
//		if ((result >> i) & 0x01 == 0x01) {std::cout << 1;}
//		else {std::cout << 0;}
//	}
//	//std::cout << " <-- hand_to_64bit result\n";

	return result;
}


int Video_Poker::MZ_Rank_hand(const int hand[]) {

	int NO_WIN = 0,
	    ONE_PAIR = 1,
	    TWO_PAIR =  2,
	    TRIS = 3,
	    STRAIGHT = 4,
	    FLUSH = 5,
	    FULL = 6, 
	    POKER = 7,
	    STRAIGHT_FLUSH = 8,
	    ROYAL_FLUSH = 9; // MOVE ALL OF THIS OUT

	int mask_MIN_PAIR = ((1 << 13) - 1) ^ ((1 << 9) - 1); // MOVE THIS OUT

	//int rank = 0; // for future use...if I need to save all posible ranks and select the best one to return...

	int mask_value = ((1 << 13) - 1); // MOVE THIS OUT so it is not done each time
	int mask_suite = ((1 << 17) - 1) ^ ((1 << 13) - 1); // MOVE THIS OUT
	int OR = hand[0] | hand[1] | hand[2] | hand[3] | hand[4];
	int XOR = hand[0] ^ hand[1] ^ hand[2] ^ hand[3] ^ hand[4];
	
	int hand_suits = count_high_bits(OR & mask_suite);
	int hand_values = count_high_bits(OR & mask_value);

	/// make sure that the correct rank is returned (best rank)... for now it isn't important, but leter this can cause strange "bugs"
	switch (OR & mask_value) {
		case 31:	if (hand_suits == 1) {return STRAIGHT_FLUSH;} // 2-3-4-5-6
				else {return STRAIGHT;}
				break;
		case 62:	if (hand_suits == 1) {return STRAIGHT_FLUSH;} // 3-4-5-6-7
				else {return STRAIGHT;}
				break;
		case 124:	if (hand_suits == 1) {return STRAIGHT_FLUSH;} // 4-5-6-7-8
				else {return STRAIGHT;}
				break;
		case 248:	if (hand_suits == 1) {return STRAIGHT_FLUSH;} // 5-6-7-8-9
				else {return STRAIGHT;}
				break;
		case 496:	if (hand_suits == 1) {return STRAIGHT_FLUSH;} // 6-7-8-9-10
				else {return STRAIGHT;}
				break;
		case 992:	if (hand_suits == 1) {return STRAIGHT_FLUSH;} // 7-8-9-10-J
				else {return STRAIGHT;}
				break;
		case 1984:	if (hand_suits == 1) {return STRAIGHT_FLUSH;} // 8-9-10-J-Q
				else {return STRAIGHT;}
				break;
		case 3968:	if (hand_suits == 1) {return STRAIGHT_FLUSH;} // 9-10-J-Q-K
				else {return STRAIGHT;}
				break;
		case 7936:	if (hand_suits == 1) {return ROYAL_FLUSH;} // 10-J-Q-K-A
				else {return STRAIGHT;}
				break;
		case 4111:	if (hand_suits == 1) {return STRAIGHT_FLUSH;} // A-2-3-4-5
				else {return STRAIGHT;}
				break;
		default:	if (hand_suits == 1) {return FLUSH;} // if we have only one suite and is none of the above -> it must be FLUSH
				break;
	}

	switch (hand_values) {
		case 5:	return NO_WIN; // NO-WIN or STRAIGHT or FLUSH ... flush and straight are covered above
			break;
		case 4: if (((OR ^ XOR) & mask_MIN_PAIR) > 0) {return ONE_PAIR;} // 1-PAIR ... BUG ... how to check if 1-PAIR has sufficient rank (Jacks or Better)? ... FIXED!
			//print_bits((OR ^ XOR) & mask_MIN_PAIR);
			break;
		case 3: for (int c1 = 0; c1 < 3; c1++) { // 2-PAIR or TRIS
				for (int c2 = c1 + 1; c2 < 4; c2++) {
					for (int c3 = c2 + 1; c3 < 5; c3++) {
						if (count_high_bits((hand[c1] | hand[c2] | hand[c3]) & mask_value) == 1) {return TRIS;}
					}
				}
			}
			return TWO_PAIR;
		case 2: // FULL-HOUSE or POKER
		// if there is any hand subset of 4 cards that have the same bit high in value part
		// we have a poker ... otherwise it is full-house
			int count = 0;
			for (int i = 0; i < 5; i++) {
				if ((XOR & mask_value) & hand[i]) {count++;} // BUG ... FULL-HOUSE are recognised as POKER (ex. 3-4-3-4-3) .. FIXED!
			}
			if (count == 1) {return POKER;}
			return FULL;
		//case 1: // JOKER COMBOS?
	}
		
}

void Video_Poker::MZ_Card_Mixxer(const int deck[], int* start, int* end) {
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle (start, end, std::default_random_engine(seed));
}

//vector<int> Video_Poker::detect_bit_toggle(int hand[]) {
//	vector<int> position;
//	int OR = hand[0] | hand[1] | hand[2] | hand[3] | hand[4];
//	int XOR = hand[0] ^ hand[1] ^ hand[2] ^ hand[3] ^ hand[4];
//	int toggle_spy = OR ^ XOR;
//	for (int i = 0; i < 8*sizeof(*hand); i++) {
//		if ((toggle_spy >> i) & 0x01 == 0x01) {
//			position.push_back(i);
//		}
//	}
//	return position;
//	
//}
	

///////////////////////////////////////////////////////////////////////////////////////////////////













