#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>

using namespace std;

int random(int min, int max) {
    random_device rd;
    mt19937::result_type seed = rd() ^ (
        (mt19937::result_type)
        chrono::duration_cast<chrono::seconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count() +
        (mt19937::result_type)
        chrono::duration_cast<chrono::microseconds>(
            chrono::high_resolution_clock::now().time_since_epoch()
        ).count()
    );

    mt19937 gen(seed);
    uniform_int_distribution<unsigned> distrib(min, max);
    
    return distrib(gen);
}

map<int, int> cardValues;
map<int, map<int, int>> suits_chosen;
string suits[4] = {"Clubs", "Spades", "Diamonds", "Hearts"};
map<int, string> royalties = {
    {1, "Jack"},
    {2, "Queen"},
    {3, "King"},
    {4, "Ace"}
};

int getRandomCard(int *hand, int *royaltyType) {
    int cardIndex = 0;

    while (true) {
        cardIndex = random(1,14);
        int cardsLeft = cardValues[cardIndex];

        if (cardsLeft > 0) { break; }
    }
    cardValues[cardIndex]--;

    if (11 <= cardIndex && cardIndex < 14) { *royaltyType = cardIndex - 10; cardIndex = 10; }

    if (*royaltyType == 4 && *hand + 11 < 21) { cardIndex = 1; } // Ace

    *hand += cardIndex;

    return cardIndex;
}

void deal(int *hand, string *card, int *suit, int *royaltyType) {
    int cardIndex = getRandomCard(hand, royaltyType);

    while (true) {
        *suit = random(1,4);
        if (suits_chosen[cardIndex + *royaltyType][*suit] > 0) { break; }
    }

    suits_chosen[cardIndex + *royaltyType][*suit]--;

    if (*royaltyType != 0) {
        *card = royalties[*royaltyType];
    } else {
        *card = to_string(cardIndex);
    }
}

int dealerRound(bool *dealerStopped, bool *playerStopped, int *hand, int *dealer) {
    bool draw = (*hand > *dealer && *playerStopped) | (*dealer < 12) | (random(0, round((*dealer - 11)/3)) == 0);

    if (!*dealerStopped && draw) {
        string dealerCard;
        int dealerSuit;
        int dealerRoyaltyType = 0;

        deal(dealer, &dealerCard, &dealerSuit, &dealerRoyaltyType);

        cout << "Dealer picked up the " << dealerCard << " of "<< suits[dealerSuit-1] << ", putting their hand at " << *dealer << endl << endl;

        if (*dealer > 21) {
            cout << "Win!" << endl;
            return 1;
        }

        if (*hand == 21 && *dealer == 21) {
            cout << "Push!" << endl; // Bet is returned (tie)
            return 1;
        } else if (*dealer == 21) {
            cout << "Lose!" << endl;
            return 1;
        } else if (*hand == 21) {
            cout << "Win!" << endl;
            return 1;
        }
    } else {
        *dealerStopped = true;
        cout << "Dealer stopped at " << *dealer << endl;
        if (*hand > *dealer) {
            cout << "Win!" << endl;
            return 1;
        }
    }

    return 0;
}

int doRound(int *hand, int *dealer, bool *dealerStopped, bool *playerStopped) {
    string card;
    int suit;
    int royaltyType = 0;

    deal(hand, &card, &suit, &royaltyType);

    cout << "You picked up the " << card << " of "<< suits[suit-1] << ", putting your hand at " << *hand << endl;

    if (*hand > 21) {
        cout << "Bust!" << endl;
        return 1;
    }

    return dealerRound(dealerStopped, playerStopped, hand, dealer);
}

bool dealerContinues = false;

int main() {
    while (true) {
        // Reset card values
        for (int i = 1; i < 14; i++) {
            cardValues[i] = 4;
            map<int, int> _suits = {
                {1, 4},
                {2, 4},
                {3, 4},
                {4, 4}
            };
            suits_chosen[i] = _suits;
        };

        int hand = 0;
        int dealer = 0;
        bool dealerStopped = false;
        bool playerStopped = false;

        doRound(&hand, &dealer, &dealerStopped, &playerStopped);

        while (true) {
            cout << "Draw? (Y/n) ";
            string input;
            getline(cin, input);

            transform(input.begin(), input.end(), input.begin(), ::tolower);

            if (input != "y") { playerStopped = true; break; };

            int result = doRound(&hand, &dealer, &dealerStopped, &playerStopped);

            if (result == 1) { break; }
        }

        cout << "Final hand: " << hand << endl;

        if (playerStopped) {

            if (dealerContinues) {
                while (true) {
                    int dealerResult = dealerRound(&dealerStopped, &playerStopped, &hand, &dealer);

                    if (dealerResult == 0) {
                        if (playerStopped && dealerStopped) {
                            if (hand > dealer) {
                                cout << "Win!" << endl;
                                break;
                            } else {
                                cout << "Lose!" << endl;
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
            } else {
                int dealerResult = dealerRound(&dealerStopped, &playerStopped, &hand, &dealer);

                if (dealerResult == 0) {
                    if (hand > dealer) {
                        cout << "Win!" << endl;
                    } else {
                       cout << "Lose!" << endl;
                    }
                }
            }
        }


        cout << "\n\n\n";
    }

    return 1;
}