#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define PLAYER_CARDS 2
#define BOARD_CARDS 5
#define MIN_PLAYERS 2
#define MAX_PLAYERS 8
#define COMBINATION_CARDS 5

enum input_states {INVALID_INPUT, VALID_INPUT_CONTINUE, VALID_INPUT_EOF};
/**
 * Keep the order and default values assigned in all enums below.
 * When adding new value add it to last but one position,
 * as the last value tracks the size of the enum.
 */
enum symbols {VALUE, SUIT, CARD_SYMBOLS};
enum suits {HEARTHS, DIAMONDS, SPADES, CLUBS, SUITS};
enum values {
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE,
    VALUES
};
enum combinations {
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    COMBINATIONS
};

/********************************************************* 
 *                   INPUT VALIDATION                    *
 *********************************************************/

/**
 * Parses card value
 * @param value card value
 * @return parsed card value, -1 if invalid input
 */
int parse_card_value(int value) {
    switch (value) {
        case '2':
            return TWO;
        case '3':
            return THREE;
        case '4':
            return FOUR;
        case '5':
            return FIVE;
        case '6':
            return SIX;
        case '7':
            return SEVEN;
        case '8':
            return EIGHT;
        case '9':
            return NINE;
        case 'T':
            return TEN;
        case 'J':
            return JACK;
        case 'Q':
            return QUEEN;
        case 'K':
            return KING;
        case 'A':
            return ACE;
        default:
            return -1;
    }
}

/**
 * Parses card suit
 * @param suit card suit
 * @return parsed card suit, -1 if invalid input
 */
int parse_card_suit(int suit) {
    switch (suit) {
        case 'h':
            return HEARTHS;
        case 'd':
            return DIAMONDS;
        case 's':
            return SPADES;
        case 'c':
            return CLUBS;
        default:
            return -1;
    }
}

/**
 * Checks if two cards are equal, by comparing individual card symbols
 * @param card1 first card
 * @param card2 second card
 * @return true if cards are equal, else false
 */
bool equal_cards(const int card1[CARD_SYMBOLS], const int card2[CARD_SYMBOLS]) {
    for (int symbol = 0; symbol < CARD_SYMBOLS; ++symbol) {
        if (card1[symbol] != card2[symbol]) {
            return false;
        }
    }
    return true;
}

/**
 * Checks if cards are unique
 * @param players number of players
 * @param cards cards to be checked
 * @return true if unique, else false
 */
bool unique_cards(int players, int new_card, int cards[players * PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS]) {
    for (int card = 0; card < new_card; ++card) {
            if (equal_cards(cards[card], cards[new_card])) {
                fprintf(stderr, "Duplicate cards found\n");
                return false;
            }
    }
    return true;
}

/**
 * Loads whitespaces from stdin, reports when invalid characters are loaded
 * @param player player index
 * @param card card index
 * @return status of loading input.
 * VALID_INPUT_CONTINUE when valid characters were received, but loading game is not finished yet
 * VALID_INPUT_EOF when EOF is received after successfully loaded entire game (player == 0, card == 0)
 * INVALID_INPUT otherwise
 */
int read_whitespaces(int player, int card) {
    int whitespace_count = 0;
    int input;
    while ((input = getchar()) != EOF && isspace(input)) {
        whitespace_count++;
    }
    if (input == EOF) {
        if (player == 0 && card == 0) {
            return VALID_INPUT_EOF;
        }
        fprintf(stderr, "Unexpected end of file\n");
        return INVALID_INPUT;
    }
    if (whitespace_count == 0 && card != 0) {
        fprintf(stderr, "Invalid input. Expected whitespace\n");
        return INVALID_INPUT;
    }
    ungetc(input, stdin);
    return VALID_INPUT_CONTINUE;
}

/*********************************************************
 *                        INPUT                          *
 *********************************************************/

static int parse_players (int argc, char **argv) {
    switch (argc) {
        case 1:
            return 2;
        case 2:
            return atoi(argv[1]);
        default:
            return 0;
    }
}

/**
 * Loads cards for given player (player == players when loading board cards)
 * @param players number of players
 * @param player index of current player
 * @param cards
 * @return status of loading input.
 * VALID_INPUT_CONTINUE when valid characters were received, but loading game is not finished yet
 * VALID_INPUT_EOF when EOF is received after successfully loaded entire game (player == 0, card == 0)
 * INVALID_INPUT otherwise
 */
int load_cards(int players, int player, int cards[players * PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS]) {
    int input;
    int state;
    // player == players means loading board cards
    int card_count = player == players ? BOARD_CARDS : PLAYER_CARDS;
    for (int card = 0; card < card_count; ++card) {
        if ((state = read_whitespaces(player, card)) != VALID_INPUT_CONTINUE) {
            return state;
        }
        for (int symbol = 0; symbol < CARD_SYMBOLS; ++symbol) {
            input = getchar();
            switch (symbol) {
                case VALUE:
                    input = parse_card_value(input);
                    break;
                case SUIT:
                    input = parse_card_suit(input);
                    break;
                default:
                    fprintf(stderr, "Symbol error. This should never execute\n");
                    return INVALID_INPUT;
            }
            if (input == -1) {
                fprintf(stderr, "Invalid input. Expected card format: [2-9TJQKA][hdsc]\n");
                return INVALID_INPUT;
            }
            cards[player * PLAYER_CARDS + card][symbol] = input;
        }
        if (!unique_cards(players, player * PLAYER_CARDS + card, cards)) {
            return INVALID_INPUT;
        }
    }
    if (getchar() != '\n') {
        fprintf(stderr, "Invalid input. Expected newline\n");
        return INVALID_INPUT;
    }
    return VALID_INPUT_CONTINUE;
}

/**
 * Loads game
 * @param players number of players
 * @param cards all cards in the game
 * @return status of loading input.
 * VALID_INPUT_CONTINUE when valid characters were received, but loading game is not finished yet
 * VALID_INPUT_EOF when EOF is received after successfully loaded entire game (player == 0, card == 0)
 * INVALID_INPUT otherwise
 */
int load_game(int players, int cards[players * PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS]) {
    int state;
    // loading board when player == players
    for (int player = 0; player < players + 1; ++player) {
        if ((state = load_cards(players, player, cards)) != VALID_INPUT_CONTINUE) {
            return state;
        }
    }
    return VALID_INPUT_CONTINUE;
}

/*********************************************************
 *                        OUTPUT                         *
 *********************************************************/

/**
 * Prints result of the game
 * @param result index of player who won, -1 if draw
 */
void print_result_of_game(int result) {
    if (result == -1) {
        printf("Draw\n");
    } else {
        printf("Player %d\n", result + 1);
    }
}

/*********************************************************
 *                   PLAYER EVALUATION                   *
 *********************************************************/

/**
 * Checks if there is a flush in given cards
 * @param cards player and board cards, sorted in descending order
 * @return suit of the flush found, -1 if no flush found
 */
int check_flush(int cards[PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS]) {
    int suits[SUITS] = {0};
    int suit;
    for (int card = 0; card < PLAYER_CARDS + BOARD_CARDS; ++card) {
        suit = cards[card][SUIT];
        suits[suit]++;
        if (suits[suit] == 5) {
            return suit;
        }
    }
    return -1;
}

/**
 * Fills combination with card values of flush given by suit
 * @param suit suit of flush
 * @param cards player and board cards, sorted in descending order
 * @param combination card values of the flush
 */
void fill_flush_combination(int suit, int cards[PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS], int combination[COMBINATION_CARDS]) {
    int combination_index = 0;
    for (int card = 0; card < PLAYER_CARDS + BOARD_CARDS; ++card) {
        if (cards[card][SUIT] == suit) {
            combination[combination_index] = cards[card][VALUE];
            combination_index++;
            if (combination_index == 5) {
                break;
            }
        }
    }
}

/**
 * Checks for straight in given cards and fills combination with corresponding cards of the straight
 * @param cards player and board cards, sorted in descending order
 * @param combination card values of the straight
 * @return STRAIGHT, -1 if not found
 */
int check_straight(int cards[PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS], int combination[COMBINATION_CARDS]) {
    int counter = 1;
    int last_card = cards[0][VALUE];
    for (int card = 1; card < PLAYER_CARDS + BOARD_CARDS; ++card) {
        if (last_card == cards[card][VALUE] + 1) {
            counter++;
            if (counter == 5) {
                combination[0] = last_card + 4;
                return STRAIGHT;
            }
        } else if (last_card == cards[card][VALUE]) {
            continue;
        } else {
            counter = 1;
        }
        last_card = cards[card][VALUE];
    }

    if (counter == 4 && last_card == TWO && cards[0][VALUE] == ACE) {
        combination[0] = FIVE;
        return STRAIGHT;
    }
    return -1;
}

/**
 * Checks for straight flush and flush in given cards and fills combination with corresponding cards
 * @param cards player and board cards, sorted in descending order
 * @param combination card values of the combination found
 * @return STRAIGHT_FLUSH, FLUSH, -1 if no combination found
 */
int check_straight_flush(int cards[PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS], int combination[COMBINATION_CARDS]) {
    int suit = check_flush(cards);
    if (suit == -1) {
        return -1;
    }
    int counter = 0;
    int last_card = -1;
    int current_card;
    for (int card = 0; card < PLAYER_CARDS + BOARD_CARDS; ++card) {
        if (counter == 0) {
            if (cards[card][SUIT] == suit) {
                last_card = cards[card][VALUE];
                counter++;
            }
        } else {
            current_card = cards[card][VALUE];
            while (card < PLAYER_CARDS + BOARD_CARDS && current_card == cards[card][VALUE]) {
                if (cards[card][SUIT] == suit) {
                    if (cards[card][VALUE] + 1 == last_card) {
                        last_card = cards[card][VALUE];
                        counter++;
                        if (counter == 5) {
                            combination[0] = last_card + 4;
                            return STRAIGHT_FLUSH;
                        }
                    } else {
                        last_card = cards[card][VALUE];
                        counter = 1;
                    }
                }
                card++;
            }
            card--;
            counter = current_card != last_card ? 0 : counter;
        }
    }

    if (counter == 4 && last_card == TWO) {
        int card = 0;
        while (cards[card][VALUE] == ACE) {
            if (cards[card][SUIT] == suit) {
                combination[0] = FIVE;
                return STRAIGHT_FLUSH;
            }
            card++;
        }
    }
    fill_flush_combination(suit, cards, combination);
    return FLUSH;
}

/**
 * Find the best combination for given player and board cards
 * @param cards player and board cards, sorted in descending order
 * @return best combination found
 */
int check_remaining_combination(int cards[PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS]) {
    int values[VALUES] = {0};
    int combinations[COMBINATIONS] = {0};
    combinations[HIGH_CARD]++;
    int value;
    for (int card = 0; card < PLAYER_CARDS + BOARD_CARDS; ++card) {
        value = cards[card][VALUE];
        values[value]++;
        if (values[value] == 4) {
            combinations[FOUR_OF_A_KIND]++;
            combinations[THREE_OF_A_KIND]--;
        }
        if (values[value] == 3) {
            combinations[THREE_OF_A_KIND]++;
            combinations[PAIR]--;
        }
        if (values[value] == 2) {
            combinations[PAIR]++;
        }
    }
    if ((combinations[THREE_OF_A_KIND] > 0 && combinations[PAIR] > 0) || combinations[THREE_OF_A_KIND] > 1) {
        combinations[FULL_HOUSE]++;
    }
    if (combinations[PAIR] > 1) {
        combinations[TWO_PAIR]++;
    }
    for (int combination = COMBINATIONS - 1; combination >= HIGH_CARD; --combination) {
        if (combinations[combination] > 0) {
            return combination;
        }
    }
    return -1;
}

/**
 * Fills combination with corresponding cards of the multiple
 * multiple1 == 0 && multiple2 == 0 for the HIGH_CARD
 * multiple1 != 0 && multiple2 == 0 for the multiple of a kind
 * multiple1 != 0 && multiple2 != 0 for the TWO_PAIRS or FULL_HOUSE
 * @param multiple1 multiple of a kind
 * @param multiple2 multiple of a kind
 * @param cards player and board cards, sorted in descending order
 * @param combination card values of the combination found
 */
void fill_remaining_combination(int multiple1, int multiple2, int cards[PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS],
                                int combination[COMBINATION_CARDS]) {
    int high_card_count = 0;
    int high_card_index = 2;
    if (multiple2 == 0) {
        high_card_index = multiple1 == 0 ? 0 : 1;
    }
    for (int card = 0; card < PLAYER_CARDS + BOARD_CARDS; ++card) {
        if (multiple1 != 0 && combination[0] == -1 && cards[card][VALUE] == cards[card + multiple1 - 1][VALUE]) {
            combination[0] = cards[card][VALUE];
            card += multiple1 - 1;
        } else if (multiple2 != 0 && combination[1] == -1 && cards[card][VALUE] == cards[card + multiple2 - 1][VALUE]) {
            combination[1] = cards[card][VALUE];
            card += multiple2 - 1;
        } else if (high_card_count < COMBINATION_CARDS - multiple1 - multiple2) {
            combination[high_card_index] = cards[card][VALUE];
            high_card_index++;
            high_card_count++;
        }
    }
}

/**
 * Evaluates player's and board cards, fills combination with cards to choose
 * winner in case of matching combination
 * @param cards player's and board cards, sorted in descending order
 * @param combination cards to choose winner in case of matching combination, initialized with -1
 * @return best combination for given player
 */
int evaluate_player(int cards[PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS], int combination[COMBINATION_CARDS]) {
    int best_combination;
    if ((best_combination = check_straight_flush(cards, combination)) != -1) {
        return best_combination;
    }
    if ((best_combination = check_straight(cards, combination)) != -1) {
        return best_combination;
    }
    best_combination = check_remaining_combination(cards);
    switch (best_combination) {
        case FOUR_OF_A_KIND:
            fill_remaining_combination(4, 0, cards, combination);
            break;
        case FULL_HOUSE:
            fill_remaining_combination(3, 2, cards, combination);
            break;
        case THREE_OF_A_KIND:
            fill_remaining_combination(3, 0, cards, combination);
            break;
        case TWO_PAIR:
            fill_remaining_combination(2, 2, cards, combination);
            break;
        case PAIR:
            fill_remaining_combination(2, 0, cards, combination);
            break;
        case HIGH_CARD:
            fill_remaining_combination(0, 0, cards, combination);
            break;
        default:
            fprintf(stderr,"evaluate_player: Invalid combination found. Expected [HIGH_CARD-STRAIGHT_FLUSH]\n");
            return -1;
    }
    return best_combination;
}

/*********************************************************
 *                   GAME EVALUATION                     *
 *********************************************************/

/**
 * Compares card values, used for qsort in descending order
 * @param card1 pointer to the first card
 * @param card2 pointer to the second card
 * @return negative int if card2 has lower value, 0 if equal, positive int if card2 has higher value
 */
int compare_card_values_descending(const void *card1, const void *card2) {
    const int value1 = ((const int *) card1)[VALUE];
    const int value2 = ((const int *) card2)[VALUE];
    return value2 - value1;
}

/**
 * Chooses the winning player based on the combination and cards
 * @param players number of players
 * @param combination each player's best combination
 * @param cards cards to determine who won in case of matching combination
 * @return index of player who won, -1 if draw
 */
int choose_winner(int players, const int combination[players], int cards[players][COMBINATION_CARDS]) {
    int winner = 0;
    bool draw = false;
    for (int player = 1; player < players; ++player) {
        if (combination[player] > combination[winner]) {
            winner = player;
            draw = false;
            continue;
        }
        if (combination[player] == combination[winner]) {
            draw = true;
            for (int card = 0; card < COMBINATION_CARDS; ++card) {
                if (cards[player][card] != cards[winner][card]) {
                    winner = cards[player][card] > cards[winner][card] ? player : winner;
                    draw = false;
                    break;
                }
            }
        }
    }
    if (draw) {
        return -1;
    }
    return winner;
}

/**
 * Fills player_board_cards with appropriate cards
 * @param players number of players
 * @param player current player index
 * @param cards all cards in the game, player's cards first, board cards are at the end
 * @param player_board_cards cards for given player and board cards
 */
void get_player_board_cards(int players, int player, int cards[players * PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS],
                      int player_board_cards[PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS]) {
    for (int card = 0; card < PLAYER_CARDS; ++card) {
        for (int symbol = 0; symbol < CARD_SYMBOLS; ++symbol) {
            player_board_cards[card][symbol] = cards[player * PLAYER_CARDS + card][symbol];
        }
    }
    for (int card = 0; card < BOARD_CARDS; ++card) {
        for (int symbol = 0; symbol < CARD_SYMBOLS; ++symbol) {
            player_board_cards[PLAYER_CARDS + card][symbol] = cards[players * PLAYER_CARDS + card][symbol];
        }
    }
}

/**
 * Evaluates the game for given cards
 * @param players number of players
 * @param cards all cards in the game, player's cards first, board cards are at the end
 * @return index of player who won, -1 if draw
 */
int evaluate_game(int players, int cards[players * PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS]) {
    int combination[players];
    int combination_cards[players][COMBINATION_CARDS];
    memset(combination_cards, -1, players * COMBINATION_CARDS * sizeof(int));
    int player_board_cards[PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS];
    for (int player = 0; player < players; ++player) {
        get_player_board_cards(players, player, cards, player_board_cards);
        qsort(player_board_cards, PLAYER_CARDS + BOARD_CARDS, sizeof(*player_board_cards), compare_card_values_descending);
        combination[player] = evaluate_player(player_board_cards, combination_cards[player]);
    }
    return choose_winner(players, combination, combination_cards);
}

/*********************************************************
 *                        MAIN                           *
 *********************************************************/

int main(int argc, char **argv)
{
    int players = parse_players(argc, argv);
    if (players < MIN_PLAYERS || players > MAX_PLAYERS) {
        fprintf(stderr, "main: Invalid program arguments. Expected number of players [2-8]\n");
        return EXIT_FAILURE;
    }
    int state;
    int cards[players * PLAYER_CARDS + BOARD_CARDS][CARD_SYMBOLS];
    while ((state = load_game(players, cards)) == VALID_INPUT_CONTINUE) {
        print_result_of_game(evaluate_game(players, cards));
    }
    if (state == INVALID_INPUT) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
