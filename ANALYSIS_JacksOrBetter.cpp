// Video Poker analasys
// By MZ in Nov. 2017
//
//

#include "VideoPoker.h"
#include <iostream>
#include <vector>
//#include <fstream>

using namespace std;

vector<int> card_deck;
vector<int> remainigCards;
//vector<int> game_combination; //max 10 cards are needed (depends on num. of holds in first draw)

// All American RTP_high = 0.973864
// No-Win, One_Pair, Two_Pair, Three_Of_A_Kind, Straight, Flush, Full_House, Four_Of_A_Kind, Straight_Flush, Royal_Flush
int PAY_TABLE[10] = {0, 1, 1, 3, 7, 8, 9, 35, 90, 800};

void construct_remaining_deck(int c1, int c2, int c3, int c4, int c5) {
	int k = 0;
	int used_crds[5] = {c1, c2, c3, c4, c5};
	for (int i = 0; i < remainigCards.size(); i++) {remainigCards.erase(remainigCards.begin(), remainigCards.end());}
	for (int idx = 0; idx < card_deck.size(); idx++) {
		if (idx == used_crds[k]) {
			k++;		
		} else {
			remainigCards.push_back(idx + 1);
//			cout << "Pushing to remainigCards: " << idx << endl;
		}
	}
}

// converts integer number (1 - 52) to interger  for bit representation of that card 
int int_to_card(int card_idx) {
	int card_rank = (1 << ((card_idx - 1) % 13));//2**((card - 1) % 13)
	int card_suite = (1 << (13 + ((card_idx - 1) / 13)));//2**(13 + math.ceil((card - 1) / 13))
	return (card_rank + card_suite); 
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


// NOTE...index 1 is card 2 when using int_to_card() function
int main() {

	int tot_cards = 52;
	int needed_cards = 5; // we need 5 cards for the first hand
	int myDeck[needed_cards];
	for (int i = 0; i < tot_cards; ++i) { card_deck.push_back(i+1); } //fill vector<int> card_deck with nums 1 -> 52

	Video_Poker JoB(11); // 11 = J .. this sets min 1-pair to Jacks
	int HAND[5];
	int RANK;
	int PRIZE;
	int discardCode;
	int discards;

	int TotGames = 0;
	int TotPrize = 0;
	int Wins = 0;
	long long SquaredPrizeSum = 0;
	double Sigma;

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
						JoB.set_deck(myDeck, sizeof(myDeck)/sizeof(*myDeck)); //TODO fix bug..must set all cards not just the first 5, so it does not affect the Evaluate_Hand() function
						construct_remaining_deck(i1, i2, i3, i4, i5); // set remainigCards

						cout << "Remaining deck: ";
						for (int i=0; i < remainigCards.size(); i++) {cout << remainigCards[i] << " ";}
						cout << "\n";

						discardCode = JoB.Evaluate_Hand(PAY_TABLE); //
						discards = JoB.count_high_bits(discardCode); // number of discarded cards

						switch (discards) {
							case 5:
								cout << "5 cards to discard.....\n";
								for (int i6 = 0; i6 < remainigCards.size()-4; i6++) {
									for (int i7 = i6+1; i7<remainigCards.size()-3; i7++) {
										for (int i8 = i7+1; i8<remainigCards.size()-2; i8++) {
											for (int i9=i8+1; i9<remainigCards.size()-1; i9++) {
												for (int i10=i9+1; i10<remainigCards.size(); i10++) {
													JoB.SetOneCard(int_to_card(remainigCards[i6]), 5);
													JoB.SetOneCard(int_to_card(remainigCards[i7]), 6);
													JoB.SetOneCard(int_to_card(remainigCards[i8]), 7);
													JoB.SetOneCard(int_to_card(remainigCards[i9]), 8);
													JoB.SetOneCard(int_to_card(remainigCards[i10]), 9);
													JoB.Discard(discardCode);
													JoB.Get_Hand(HAND);
													RANK = JoB.Rank_Hand(HAND);
													PRIZE = PAY_TABLE[RANK];
													TotPrize += PRIZE;
													TotGames++;
													if (PRIZE > 0) {Wins++;}
												}
											}
										}
									}
								}
							case 4:
								cout << "4 cards to discard.....\n";
								for (int i6 = 0; i6 < remainigCards.size()-3; i6++) {
									for (int i7 = i6+1; i7<remainigCards.size()-2; i7++) {
										for (int i8 = i7+1; i8<remainigCards.size()-1; i8++) {
											for (int i9=i8+1; i9<remainigCards.size(); i9++) {
												JoB.SetOneCard(int_to_card(remainigCards[i6]), 5);
												JoB.SetOneCard(int_to_card(remainigCards[i7]), 6);
												JoB.SetOneCard(int_to_card(remainigCards[i8]), 7);
												JoB.SetOneCard(int_to_card(remainigCards[i9]), 8);
												JoB.Discard(discardCode);
												JoB.Get_Hand(HAND);
												RANK = JoB.Rank_Hand(HAND);
												PRIZE = PAY_TABLE[RANK];
												TotPrize += PRIZE;
												TotGames++;
												if (PRIZE > 0) {Wins++;}
											}
										}
									}
								}
							case 3:
								cout << "3 cards to discard.....\n";
								for (int i6 = 0; i6 < remainigCards.size()-2; i6++) {
									for (int i7 = i6+1;i7<remainigCards.size()-1;i7++) {
										for (int i8 = i7+1;i8<remainigCards.size();i8++) {
											JoB.SetOneCard(int_to_card(remainigCards[i6]), 5);
											JoB.SetOneCard(int_to_card(remainigCards[i7]), 6);
											JoB.SetOneCard(int_to_card(remainigCards[i8]), 7);
											JoB.Discard(discardCode);
											JoB.Get_Hand(HAND);
											RANK = JoB.Rank_Hand(HAND);
											PRIZE = PAY_TABLE[RANK];
											TotPrize += PRIZE;
											TotGames++;
											if (PRIZE > 0) {Wins++;}
										}
									}
								}
							case 2:
								cout << "2 cards to discard.....\n";
								for (int i6 = 0; i6 < remainigCards.size()-1; i6++) {
									for (int i7 = i6+1; i7 < remainigCards.size(); i7++) {
										JoB.SetOneCard(int_to_card(remainigCards[i6]), 5);
										JoB.SetOneCard(int_to_card(remainigCards[i7]), 6);
										JoB.Discard(discardCode);
										JoB.Get_Hand(HAND);
										RANK = JoB.Rank_Hand(HAND);
										PRIZE = PAY_TABLE[RANK];
										TotPrize += PRIZE;
										TotGames++;
										if (PRIZE > 0) {Wins++;}
									}
								}
							case 1:
								cout << "1 card to discard......\n";
								for (int i6 = 0; i6 < remainigCards.size(); i6++) {
									JoB.SetOneCard(int_to_card(remainigCards[i6]), 5);
									JoB.Discard(discardCode);
									JoB.Get_Hand(HAND);
									cout << "Hand after doscard: ";
									cout << card_to_int(HAND[0]) << " ";
									cout << card_to_int(HAND[1]) << " ";
									cout << card_to_int(HAND[2]) << " ";
									cout << card_to_int(HAND[3]) << " ";
									cout << card_to_int(HAND[4]) << " ";
									cout << "\n";
									RANK = JoB.Rank_Hand(HAND);
									PRIZE = PAY_TABLE[RANK];
									TotPrize += PRIZE;
									TotGames++;
									if (PRIZE > 0) {Wins++;}
								}
//								cout << "Hand after doscard: ";
//								cout << card_to_int(HAND[0]);
//								cout << card_to_int(HAND[1]);
//								cout << card_to_int(HAND[2]);
//								cout << card_to_int(HAND[3]);
//								cout << card_to_int(HAND[4]);
//								cout << "\n";
								break;
							case 0:	
								cout << "0 cards to discard......\n";
								JoB.Get_Hand(HAND);
								RANK = JoB.Rank_Hand(HAND);
								PRIZE = PAY_TABLE[RANK];
								TotPrize += PRIZE;
								TotGames++;
								if (PRIZE > 0) {Wins++;}
								break;
							default:
								cout << "WARNING..you should not be here!\n";
								break;
						}

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
//				break;
				}
//			break;
			}
//		break;
		}
//	break;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
	cout << "Finished............................\n\n";
	Sigma = (SquaredPrizeSum - (double)(TotPrize*TotPrize)/TotGames) / (TotGames -1);
	cout << "Total prize: " << TotPrize << endl;
	cout << "Total games: " << TotGames << endl;
	cout << "RTP: " << (double)TotPrize/TotGames << endl;
	cout << "Variance: " << Sigma << endl;
	cout << "Hit rate: " << (double)Wins/TotGames << endl;
	cout << "------------------------------------------\n";


	return 0;
}




