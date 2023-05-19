#define CUT

#include "libs/cut.h"
#include "libs/mainwrap.h"

#include <stdlib.h>

TEST(duplicate_one_player) {
    INPUT_STRING(
            "3d 3d\n"
            "Ac Ts\n"
            "Qd 8s 2c 4c Kh\n"
    );
    ASSERT(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Duplicate cards found\n");
}

TEST(duplicate_player_player) {
    INPUT_STRING(
            "3d Ac\n"
            "Ac Ts\n"
            "Qd 8s 2c 4c Kh\n"
    );
    ASSERT(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Duplicate cards found\n");
}

TEST(duplicate_player_board) {
    INPUT_STRING(
            "4d 3h\n"
            "Ac Ts\n"
            "Qd Ts 2c 4c Kh\n"
    );
    ASSERT(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Duplicate cards found\n");
}

TEST(duplicate_board_board) {
    INPUT_STRING(
            "4c 3d\n"
            "Ac Ts\n"
            "Qd 8s 2c 4c 2c\n"
    );
    ASSERT(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Duplicate cards found\n");
}

TEST(multiple_whitespaces) {
    INPUT_STRING(
            "   3d 3h\n"
            "Ac     Ts\n"
            " Qd  8s 2c   4c             Kh\n"
    );
    ASSERT(app_main() == EXIT_SUCCESS);
    ASSERT_FILE(stdout, "Player 1\n");
}

TEST(missing_whitespace) {
    INPUT_STRING(
            "3d 3h\n"
            "AcTs\n"
            "Qd 8s 2c 4c Kh\n"
    );

    ASSERT(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Invalid input. Expected whitespace\n");
}

TEST(invalid_card_value) {
    INPUT_STRING(
            "3d 3h\n"
            "Ac Ts\n"
            "Qd 8s gc 4c Kh\n"
    );

    ASSERT(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Invalid input. Expected card format: [2-9TJQKA][hdsc]\n");
}

TEST(invalid_card_suit) {
    INPUT_STRING(
            "3d 3f\n"
            "Ac Ts\n"
            "Qd 8s gc 4c Kh\n"
    );

    ASSERT(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Invalid input. Expected card format: [2-9TJQKA][hdsc]\n");
}

TEST(missing_newline) {
    INPUT_STRING(
            "3d 3h\n"
            "Ac Ts"
            "Qd 8s gc 4c Kh\n"
    );

    ASSERT(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Invalid input. Expected newline\n");
}

TEST(unexpected_eof) {
    INPUT_STRING(
            "3d 3h\n"
            "Ac "
    );

    ASSERT(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Unexpected end of file\n");
}

TEST(one_player) {
    INPUT_STRING(
            "3d 3h\n"
            "Ac Ts\n"
            "Qd 8s 2c 4c Kh\n"
    );

    ASSERT(app_main_args("1") == EXIT_FAILURE);
    ASSERT_FILE(stderr, "main: Invalid program arguments. Expected number of players [2-8]\n");
}

TEST(nine_players) {
    INPUT_STRING(
            "3d 3h\n"
            "Ac Ts\n"
            "Qd 8s 2c 4c Kh\n"
    );

    ASSERT(app_main_args("9") == EXIT_FAILURE);
    ASSERT_FILE(stderr, "main: Invalid program arguments. Expected number of players [2-8]\n");
}

TEST(four_players) {
    INPUT_STRING(
        "Kc Jh\n"
        "7d Jd\n"
        "4c 7h\n"
        "5h 2s\n"
        "9s 5c Ah 5s Qs\n"
    );

    ASSERT(app_main_args("4") == EXIT_SUCCESS);
    ASSERT_FILE(stdout, "Player 4\n");
}
