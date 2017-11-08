///////////////////////////////////////////////////////////////////////////////////////////////////
//
// MZ in Nov. 2017
//
// Video Poker RTP simulation
// TODO:
//

#include "VideoPoker.h"
#include <iostream>
#include <vector>
//#include <fstream>

using namespace std;

vector<int> card_deck;
vector<int> remainig_card_deck;
vector<int> game_combination; //max 10 cards are needed (depends on num. of holds in first draw)

// No-Win, One_Pair, Two_Pair, Three_Of_A_Kind, Straight, Flush, Full_House, Four_Of_A_Kind, Straight_Flush, Royal_Flush
int PAY_TABLE[10] = {0, 1, 2, 3, 5, 6, 8, 20, 40, 800};

int main() {

	int SIMULATIONS = 1000000; // set number of games to simulate
	Video_Poker JoB(11); // 11 = J .. this sets min 1-pair to Jacks
	int HAND[5];
	int RANK;
	int PRIZE;
	int TotPrize = 0;
	int Wins = 0;
	long long SquaredPrizeSum = 0;
	double Sigma;

	for (int game = 0; game < SIMULATIONS; game++) {
		JoB.Shuffle(); // Shuffle card deck.
		JoB.Discard(JoB.Evaluate_Hand(PAY_TABLE)); // Discard cards.
		JoB.Get_Hand(HAND); // Get hand after discard.
		RANK = JoB.Rank_Hand(HAND); // Rank the hand.
		PRIZE = PAY_TABLE[RANK]; // Get the prize for given rank.

		TotPrize += PRIZE;
		SquaredPrizeSum += PRIZE * PRIZE;
		if (PRIZE) {Wins++;}

		// Print results every x games to see the progress.
		if (game % 100 == 0) {
			Sigma = (SquaredPrizeSum - (double)(TotPrize*TotPrize)/game) / (game -1);
			cout << "Total prize: " << TotPrize << endl;
			cout << "Total games: " << game << endl;
			cout << "RTP: " << (double)TotPrize/game << endl;
			cout << "Sigma: " << Sigma << endl;
			cout << "Hit rate: " << (double)Wins/game << endl;
			cout << "------------------------------------------\n";
		}
	}

	cout << "Finished............................\n";


	return 0;
}



