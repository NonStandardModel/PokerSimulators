///////////////////////////////////////////////////////////////////////////////////////////////////
//
// MZ in Oct. 2017
//
// Video Poker RTP analasys
// TODO:-add set deck method to VideoPoker class
//	-add Joker cards and Joker related wins to VideoPoker class  (? all bits 1 -> 2**17 - 1 = 131071 ?)
//	-write custom hand eveluator function
//	-write custom get_discard_cards function for different strategies (perfect, standard, basic, idiot...)

#include "VideoPoker.h"
//#include <stdlib.h> //do I need this?
//#include <stdio.h> //do I need this?
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

vector<int> card_deck;
vector<int> remainig_card_deck;
vector<int> game_combination; //max 10 cards are needed (depends on num. of holds in first draw)

// No-Win, One_Pair, Two_Pair, Three_Of_A_Kind, Straight, Flush, Full_House, Four_Of_A_Kind, Straight_Flush, Royal_Flush
int PAY_TABLE[10] = {0, 1, 2, 3, 4, 6, 9, 25, 50, 500};

////Read pay_table from file
//ifstream paytable_file ("PayTable.txt");



void pretty_print(const vector<int>& v) {
  static int count = 0;
  cout << "combination no " << (++count) << ": [ ";
  for (int i = 0; i < v.size(); ++i) { cout << v[i] << " "; }
  cout << "] " << endl;
}

// this will construct all combinations for first 5 cards
void combination_constructer(int offset, int k) {
  if (k == 0) {
    pretty_print(game_combination);
    return;
  }
  for (int i = offset; i <= card_deck.size() - k; ++i) {
    game_combination.push_back(card_deck[i]);
    combination_constructer(i+1, k-1);
    game_combination.pop_back();
  }
}


// try to do it all here
//void my_combination_constructer(int offset, int k) {

//	int myDeck[k];
//	int HAND[5];
//	int totGames = 0;
//	int totPrize = 0;
//	int PRIZE = 0;
//	int discardCode = 0;
//	Video_Poker ViP(11);

// 	if (k == 0) {

//		//MAGIC GOES HERE
//		std::copy(game_combination.begin(), game_combination.end(), myDeck);
//		ViP.set_deck(myDeck, sizeof(myDeck)/sizeof(*myDeck));
////		discardCode = ViP.Evaluate_Hand(PAY_TABLE);
////		ViP.Discard(discardCode);
//		ViP.Get_Hand(HAND);
//		PRIZE = PAY_TABLE[ViP.Rank_Hand(HAND)];
//		totPrize += PRIZE;
//		totGames++;

//		return;
//	}
//	for (int i = offset; i <= card_deck.size() - k; ++i) {
//		game_combination.push_back(card_deck[i]);
//		my_combination_constructer(i+1, k-1);
//		game_combination.pop_back();
//	}
//}


// function name tels you all 
// Note: CARDS MUST BE ORDERED c1 < c2 < c3 < c4 < c5
void construct_remaining_deck(int c1, int c2, int c3, int c4, int c5) {
	int k = 0;
	int used_crds[5] = {c1, c2, c3, c4, c5};
	for (int idx = 1; idx <= card_deck.size(); idx++) {
		if (idx == used_crds[k]) {
			k++;		
		} else {
			remainig_card_deck.push_back(idx);
		}
	}
}

//void construct_remaining_deck(int *first_5_cards) {
//	for (int idx = 0; idx < card_deck.size(); idx++) {
//		if (idx == *(first_5_cards) or *(first_5_cards+1) or *(first_5_cards+2) or *(first_5_cards+3) or *(first_5_cards+4)) {
//			remainig_card_deck.push_back(card_deck[idx]);
//		}
//	}
//}

// this will construct all permutations for last 5 cards from remaining 47 
void permutation_constructer(int *first_5_cards) {
	//for card in deck:
	//	if card not in first_5_cards:
	//		remainig_card_deck.append(card)
//this should be moved outside ... no need to redo remainig_card_deck every time!
//same combo of 1st 5 cards has the same remainig_card_deck!
//	for (int j = 1; j <= card_deck.size(); j++) {
//		if () {
//			remainig_card_deck.push_back();
//		}
//	}
 	//cout << *(first_5_cards+1) << "fdfhdgh\n";
}

// converts integer number (1 - 52) to interger  for bit representation of that card 
int int_to_card(int card_idx) {
	int card_rank = (1 << ((card_idx - 1) % 13));//2**((card - 1) % 13)
	int card_suite = (1 << (13 + ((card_idx - 1) / 13)));//2**(13 + math.ceil((card - 1) / 13))
	return (card_rank + card_suite); // CARD SUITE IS NOT RAISING ON DEUCES -> FIXED
}

// converts from card bit representation to int 1 -> 52
int card_to_int(int crd_rep) {
	int card;
	int s = (int)(crd_rep / 8192);
	int v = crd_rep - 8192*s;
	switch (s) {
		case 1: card = 0;  // suite 1
			break;
		case 2: card = 13; // suite 2
			break;
		case 4: card = 26; // suite 3
			break;
		case 8: card = 39; // suite 4
			break;
	}
	switch (v) {
		case 1: card += 1;
			break;
		case 2: card += 2;
			break;
		case 4: card += 3;
			break;
		case 8: card += 4;
			break;
		case 16: card += 5;
			break;
		case 32: card += 6;
			break;
		case 64: card += 7;
			break;
		case 128: card += 8;
			break;
		case 256: card += 9;
			break;
		case 512: card += 10;
			break;
		case 1024: card += 11;
			break;
		case 2048: card += 12;
			break;
		case 4096: card += 13;
			break;
	}
	return card;
}

// print card int (from 1 -> 52) in semi-human format
void print_card(int card) {
	int card_value = ((card - 1) % 13) + 2; //lowest card value is 2
	int suite = (int)((card - 1) / 13) + 1; //suits are just nums from 1 to 4
	if (card_value < 11) {
		cout << "Value: " << card_value << " Suite: " << suite << endl;
	} else {
		char c;
		if (card_value == 11) {c = 'J';}
		if (card_value == 12) {c = 'Q';}
		if (card_value == 13) {c = 'K';}
		if (card_value == 14) {c = 'A';}
		cout << "Value: " << c << " Suite: " << suite << endl;
	}
}

// print function for debug
void print(vector<int> crds, int show) {
	if (show > crds.size()) {
		std::cout << "\nCan't print more than \"size\" elements!\n";
		show = crds.size();
	}
	for (int idx = 0; idx < show; idx++) {
		//std::cout << crds[idx] << std::endl;
		std::cout << "Print num. " << (idx + 1) << " -> " << crds[idx] << std::endl;
	}
}
void print(int crds[], int show) {
//	if (show > sizeof(crds)/sizeof(*crds)) {
//		std::cout << "\nCan't print more than \"size\" elements!\n";
//		show = sizeof(crds)/sizeof(*crds);
//	}
	for (int idx = 0; idx < show; idx++) {
		//std::cout << crds[idx] << std::endl;
		std::cout << "Print num. " << (idx + 1) << " -> " << crds[idx] << std::endl;
	}
}

vector<int> detect_bit_toggle(int hand[]) {
	vector<int> position;
	int OR = hand[0] | hand[1] | hand[2] | hand[3] | hand[4];
	int XOR = hand[0] ^ hand[1] ^ hand[2] ^ hand[3] ^ hand[4];
	int toggle_spy = OR ^ XOR;
	for (int i = 0; i < 8*sizeof(*hand); i++) {
		if ((toggle_spy >> i) & 0x01 == 0x01) {
			position.push_back(i);
		}
	}
	return position; /*CAUSE OF THE BUG  .. there was no return -> Segmentation fault*/
	
}
	
void print_bits(int num) {
	for (int i = 0; i < 8*sizeof(num); i++) {
		if ((num >> i) & 0x01 == 0x01) {
			cout << "1";
		} else {
			cout << "0";
		}
	}
	cout << endl;
}

int main() {
	//we have to construct all possible hands from 1 deck of 52 cards
	//in reality only the firts 10 cards can have any effect on the game exit (5 1st hand + max 5 in 2nd hand)
	//therefore we need all combinations of 10 cards from 52
	//order is not important for 1st 5 cards, but important for last 5 cards
	//so we need all (combinations of 5 from 52 + permutations 5 from 47 remaining cards)
	int tot_cards = 52;
	int needed_cards = 5;

	//fill vector<int> card_deck with nums 1 -> 52
	for (int i = 0; i < tot_cards; ++i) { card_deck.push_back(i+1); }

	////Read pay_table from file
//	string line;
//	ifstream paytable_file ("PayTable.txt");
//	if (paytable_file.is_open()) {
//		while (getline(paytable_file, line)) {
//			cout << line << '\n';
//		}
//		paytable_file.close();
//	} else {cout << "Unable to open file";}

	ofstream log_file;
	log_file.open("error_log.txt");
	



	//
//	combination_constructer(0, needed_cards);
//	my_combination_constructer(0, needed_cards);

	/*pseudo-code*/
	//int TOT_GAMES = 0
	//int TOT_PRIZE = 0
	//for shuffled_deck in all_distinct_decks:
	//	VideoPoker.Get_Hand(HAND) ... this line may not be needed!!!!!
	//	DISCARD_CODE = VideoPoker.Evaluate_Hand(PAY_TABLE)
	//	VideoPoker.Discard(DISCARD_CODE)
	//	VideoPoker.Get_Hand(HAND)
	//	int HAND_PRIZE = PAY_TABLE[VideoPoker.Rank_Hand(HAND)]
	//	TOT_PRIZE += HAND_PRIZE
	//	TOT_GAMES++
	//int RTP = TOT_PRIZE / TOT_GAMES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int myDeck[needed_cards];
	int HAND[5];
	int totGames = 0;
	int totPrize = 0;
	int PRIZE = 0;
	int RANK, RANK_MZ;
	int discardCode = 0;
	int num_dis_cards = 0;
	int wins = 0;
	Video_Poker ViP(11);

	// this are here for analysis purposes
	int rank_count[10]; //from HighCard = 0 to RoyalFlush = 9
	int num_discard_rank[10][5]; // how many cards are discarded by rank

	for(int i1 = 0; i1 <= card_deck.size()-needed_cards; i1++) {
		for (int i2 = i1+1; i2 <= card_deck.size()-needed_cards+1; i2++) {
			for (int i3 = i2+1; i3 <= card_deck.size()-needed_cards+2; i3++) {
				for (int i4 = i3+1; i4 <= card_deck.size()-needed_cards+3; i4++) {
					for (int i5 = i4+1; i5 <= card_deck.size()-needed_cards+4; i5++) {
						cout << "[" << i1 << " " << i2 << " " << i3 << " " << i4 << " " << i5 << "]\n";
						myDeck[0] = int_to_card(i1 + 1);
						myDeck[1] = int_to_card(i2 + 1);
						myDeck[2] = int_to_card(i3 + 1);
						myDeck[3] = int_to_card(i4 + 1);
						myDeck[4] = int_to_card(i5 + 1);
						ViP.set_deck(myDeck, sizeof(myDeck)/sizeof(*myDeck));
						construct_remaining_deck(i1, i2, i3, i4, i5);
						//discardCode = ViP.Evaluate_Hand(PAY_TABLE);
						//DO THE PERMUTATIONS HERE
						//ViP.Discard(discardCode);
						ViP.Get_Hand(HAND);
						RANK = ViP.Rank_Hand(HAND);
						RANK_MZ = ViP.MZ_Rank_hand(HAND);
						if (RANK != RANK_MZ) {
							for (int i = 0; i < 5; i++) {
								cout << HAND[i] << " ";
								log_file << card_to_int(HAND[i]) << " ";
							}
							cout << RANK << " ... " << RANK_MZ << endl;
							log_file << RANK << " ... " << RANK_MZ << "\n";
						}
						PRIZE = PAY_TABLE[RANK];
						totPrize += PRIZE;
						totGames++;
						if (PRIZE > 0) {wins++;}
						rank_count[RANK]++;
//						num_dis_cards = 0;
//						cout << discardCode << endl;
//						for (int i = 0; i < 5; i++) {
//							if (discardCode & (1 << i) != 0) {num_dis_cards++;}
//							
//							cout << (discardCode&(1 << i)) << endl;
//							cout << (1 << i) << endl;
//						}
//						num_discard_rank[RANK][num_dis_cards]++;
					}
				//break;
				}
			//break;
			}
		//break;
		}
	break;
	}

	cout << "Total prizes given: " << totPrize << endl;
	cout << "Total games played: " << totGames << endl;
	cout << "RTP: " << (double)totPrize/totGames << endl;
	cout << "Hit rate: " << (double)wins/totGames << endl;
	
	cout << "------------------------------------------\n";
	for (int i = 0; i < 10; i++) {
		cout << "Rank " << i << " occured " << rank_count[i] << " times.\n";
	}
	cout << "------------------------------------------\n";
	cout << "Num discarded cards vs hand rank analysis:\n";
	for (int i = 0; i < 10; i++) {
		cout << "Rank " << i << " -> ";
		for (int j = 0; j < 5; j++) {
			cout << num_discard_rank[i][j] << " ";
		}
		cout << "\n-------------------------------------\n";
	}

	log_file.close();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	int fake_deck[5];
//	//royal-flush
////	fake_deck[0] = int_to_card(9);//10,1
////	fake_deck[1] = int_to_card(10);//J,1
////	fake_deck[2] = int_to_card(11);//Q,1
////	fake_deck[3] = int_to_card(12);//K,1
////	fake_deck[4] = int_to_card(13);//A,1
//	//straight-flush
////	fake_deck[0] = int_to_card(1);//2,1
////	fake_deck[1] = int_to_card(2);//3,1
////	fake_deck[2] = int_to_card(3);//4,1
////	fake_deck[3] = int_to_card(4);//5,1
////	fake_deck[4] = int_to_card(5);//6,1
//	//4-of-a-kind
//	fake_deck[0] = int_to_card(13);//A,1
//	fake_deck[1] = int_to_card(26);//A,2
//	fake_deck[2] = int_to_card(39);//A,3
//	fake_deck[3] = int_to_card(52);//A,4
//	fake_deck[4] = int_to_card(1);//2,1
//	//full-house
////	fake_deck[0] = int_to_card(13);//A,1
////	fake_deck[1] = int_to_card(26);//A,2
////	fake_deck[2] = int_to_card(39);//A,3
////	fake_deck[3] = int_to_card(9);//10,1
////	fake_deck[4] = int_to_card(22);//10,2
//	//flush
////	fake_deck[0] = int_to_card(1);//2,1
////	fake_deck[1] = int_to_card(2);//3,1
////	fake_deck[2] = int_to_card(5);//6,1
////	fake_deck[3] = int_to_card(7);//8,1
////	fake_deck[4] = int_to_card(9);//10,1
//	//straight
////	fake_deck[0] = int_to_card(11);//Q,1
////	fake_deck[1] = int_to_card(23);//J,2
////	fake_deck[2] = int_to_card(35);//10,3
////	fake_deck[3] = int_to_card(8);//9,1
////	fake_deck[4] = int_to_card(20);//8,2
//	//3 of a kind
////	fake_deck[0] = int_to_card(10);//J,1
////	fake_deck[1] = int_to_card(23);//J,2
////	fake_deck[2] = int_to_card(36);//J,3
////	fake_deck[3] = int_to_card(7);//8,1
////	fake_deck[4] = int_to_card(21);//9,2
//	//2 pairs
////	fake_deck[0] = int_to_card(10);//J,1
////	fake_deck[1] = int_to_card(23);//J,2
////	fake_deck[2] = int_to_card(39);//A,3
////	fake_deck[3] = int_to_card(7);//8,1
////	fake_deck[4] = int_to_card(20);//8,2
//	//pair of jacks
////	fake_deck[0] = int_to_card(10);//J,1
////	fake_deck[1] = int_to_card(23);//J,2
////	fake_deck[2] = int_to_card(27);//2,3
////	fake_deck[3] = int_to_card(43);//5,4
////	fake_deck[4] = int_to_card(5);//6,1
//	//pair of 8
////	fake_deck[0] = int_to_card(7);//8,1
////	fake_deck[1] = int_to_card(20);//8,2
////	fake_deck[2] = int_to_card(27);//2,3
////	fake_deck[3] = int_to_card(43);//5,4
////	fake_deck[4] = int_to_card(5);//6,1
//	//no-win
////	fake_deck[0] = int_to_card(2);//3,1
////	fake_deck[1] = int_to_card(19);//7,2
////	fake_deck[2] = int_to_card(35);//10,3
////	fake_deck[3] = int_to_card(43);//5,4
////	fake_deck[4] = int_to_card(5);//6,1
//	//JOKER TEST
////	//fake_deck[0] = 131071; // all bits 1 ... NOT WORKING!
////	fake_deck[0] = 0; // all bits 0
////	fake_deck[1] = int_to_card(23);//J,2
////	fake_deck[2] = int_to_card(36);//J,3
////	fake_deck[3] = int_to_card(7);//8,1
////	fake_deck[4] = int_to_card(21);//9,2
//	

//	int hand[5];

//	Video_Poker test(11);
//	//test.Shuffle();


//	// set VideoPoker::deck to my custom deck
//	//test.set_deck(fake_deck);
//	cout << "\nSize of fake_deck: " << sizeof(fake_deck)/sizeof(*fake_deck) << endl;
//	test.set_deck(fake_deck, sizeof(fake_deck)/sizeof(*fake_deck));

//	//get hand from VideoPoker::deck
//	test.Get_Hand(hand);


//	cout << "Hand before discard............\n";
//	cout << "Hand[0] -> " << hand[0] << "\n";
//	cout << "Hand[1] -> " << hand[1] << "\n";
//	cout << "Hand[2] -> " << hand[2] << "\n";
//	cout << "Hand[3] -> " << hand[3] << "\n";
//	cout << "Hand[4] -> " << hand[4] << "\n";
//	cout << "...............................\n";

////	cout << "CARD_TO_INT testing-------------\n";
////	cout << card_to_int(hand[0]) << endl;
////	cout << card_to_int(hand[1]) << endl;
////	cout << card_to_int(hand[2]) << endl;
////	cout << card_to_int(hand[3]) << endl;
////	cout << card_to_int(hand[4]) << endl;
////	cout <<"---------------------------------\n";

////	cout << "CONSTRUCT_REMAINING_DECK test---\n";
////	construct_remaining_deck(1,3,4,7,9);
////	cout << remainig_card_deck[0] << endl;
////	cout << remainig_card_deck[1] << endl;
////	cout << remainig_card_deck[2] << endl;
////	cout << remainig_card_deck[3] << endl;
////	cout << remainig_card_deck[4] << endl;
////	cout << "--------------------------------\n";

//	cout << "TOGGLE BIT DETECT test-----------\n";
//	vector<int> positions = detect_bit_toggle(hand);
//	cout << "DONE WITH DETECT...try to print\n";
//	cout << "positions.size() = " << positions.size() << endl; /*BUG*/
//	for (int i = 0; i < positions.size(); i++) {
//		cout << "Pos " << positions[i] << endl;
//	}
//	print_bits(hand[0]);
//	print_bits(hand[1]);
//	print_bits(hand[2]);
//	print_bits(hand[3]);
//	print_bits(hand[4]);

//	int dscrd_code = test.Evaluate_Hand(PAY_TABLE);
//	int hand_rank = test.Rank_Hand(hand);

//	cout << "----------------------------------------------------------" << endl;
//	cout << "Evaluate hand gives (discard code) -> " << dscrd_code << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "Rank hand gives (hand \"value\") -> " << hand_rank << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "Prize given for this hand -> " << PAY_TABLE[hand_rank] << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << endl;

//	//TEST MZ_Rank_hand
//	cout << "MZ_Rank_hand: " << test.MZ_Rank_hand(hand) << endl << endl;

//	// discard cards ... bit set iff card to be discarded
//	test.Discard(dscrd_code);

//	// get new hand (after discard has been performed)
//	test.Get_Hand(hand);


//	cout << "Hand after discard.............\n";
//	cout << "Hand[0] -> " << hand[0] << "\n";
//	cout << "Hand[1] -> " << hand[1] << "\n";
//	cout << "Hand[2] -> " << hand[2] << "\n";
//	cout << "Hand[3] -> " << hand[3] << "\n";
//	cout << "Hand[4] -> " << hand[4] << "\n";
//	cout << "...............................\n";


//	cout << "Tot num of new hands resulting from given discard:\n";
//	cout << "Get_Discard_Total: " << test.Get_Discard_Total(dscrd_code) << endl;
//	cout << ".................................................................\n";
//	cout << "----------------------------------------------------------" << endl;
//	cout << "Evaluate hand gives (discard code) -> " << dscrd_code << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "Rank hand gives (hand \"value\") -> " << hand_rank << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "Prize given for this hand -> " << PAY_TABLE[hand_rank] << endl;
//	cout << "----------------------------------------------------------" << endl;
//	int PRIZE = PAY_TABLE[hand_rank];
//	//if (PRIZE > 0) {cout << "drhghlshdgiahiadghiahdghahghw";}
//	cout << endl;


////	//TEST COUNT HIGH BIT FUNCTION
////	cout << "High bits: " << test.count_high_bits((2 << 31) - 1) << endl; 

////	//DETECT TOGLE BIT TEST
////	cout << "Detect toggle bit: " << test.detect_togle(3, 15, 31) << endl;

////	//PRINT BIT REPRESENTATION TEST
////	for (int i = 1; i < 53; i++) {
////		test.print_bits(card_to_int(i));
////	}
////	test.print_bits(4111); // 4111 is A-2-3-4-5 (STRAIGHT), but is not countinus run of 1 (high bits)
////	test.print_bits(((2 << 13) - 1)); //mask for value test
////	test.print_bits(((2 << 17) - 1) ^ ((2 << 13) - 1)); // mask for suite test

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// DEBUG FROM HERE ON...............................................
//	//print(card_deck, card_deck.size());
//	construct_remaining_deck(5);
//	//print(remainig_card_deck, remainig_card_deck.size());

//	int test_array[52];
//	//int* test_array = &card_deck[0];

//	// convert vector<int> to int array (will need this to pass my deck to VideoPoker::deck)
//	std::copy(card_deck.begin(), card_deck.end(), test_array);

////	cout << "test array size: " << sizeof(test_array)/sizeof(*test_array) << endl;
////	cout << "card deck size: " << card_deck.size() << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "Evaluate hand gives (discard code) -> " << test.Evaluate_Hand(PAY_TABLE) << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "Rank hand gives (hand \"value\") -> " << test.Rank_Hand(hand) << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "----------------------------------------------------------" << endl;
//	cout << "Get_Discard_Total gives -> " << test.Get_Discard_Total(test.Rank_Hand(hand)) << endl;
//	cout << "----------------------------------------------------------" << endl;


	//print(test_array, sizeof(test_array)/sizeof(*test_array));
//	cout << "Hand[0] -> " << hand[0] << "\n";
//	cout << "Hand[1] -> " << hand[1] << "\n";
//	cout << "Hand[2] -> " << hand[2] << "\n";
//	cout << "Hand[3] -> " << hand[3] << "\n";
//	cout << "Hand[4] -> " << hand[4] << "\n";

//	cout << "Fake deck: " << endl;
//	print(fake_deck, sizeof(fake_deck)/sizeof(*fake_deck));

//	print_card(1);
//	print_card(15);
//	print_card(26);
//	print_card(39);
//	print_card(45);

//	cout << int_to_card(1) <<  "\n";
//	cout << int_to_card(2) <<  "\n";
//	cout << int_to_card(3) <<  "\n";
//	cout << int_to_card(4) <<  "\n";
//	cout << int_to_card(5) <<  "\n";

//	int min_pair_rank = 11;
//	int pair_rank_mask = (-1 << (min_pair_rank - 2))&0x1fff;
//	cout << pair_rank_mask << endl;

	return 0;
}



