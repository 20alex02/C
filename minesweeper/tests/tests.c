#include "cut.h"

#include "../minesweeper.h"
#include "utils_for_tests.h"

/* Our provided tests are testing:
 *
 * set_cell - subset of valid states
 * postprocess - on simple valid board
 * assisting functions - is_mine, is_revealed ...
 * show_cell - subset of valid cells/states
 *
 * These functions are necessary for us, to test your representation.
 *
 * NOTE: These tests are not complete at all, they just give you a rough idea,
 *       if you are working correctly. They should also motivate you to create
 *       some of your own.
 *
 * Additional tests:
 * - print_board (this test is not a part of the nanecisto tests)
 * - load_board (this test is not a part of the nanecisto tests)
 *
 * We have provided you with these additional tests to give you a simple example
 * how the Output and Input should look like.
 *
 * As it was said previously, you should extend these provided tests
 * with your own. */

TEST(set_cell_with_mine_uppercase)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, 'M'));
    // THEN
    ASSERT(is_mine(dummy_cell));
    ASSERT(!is_revealed(dummy_cell));
}

TEST(set_cell_with_mine_lovercase)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, 'm'));
    // THEN
    ASSERT(is_mine(dummy_cell));
    ASSERT(!is_revealed(dummy_cell));
}

TEST(set_cell_revealed)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, '.'));
    // THEN
    ASSERT(is_revealed(dummy_cell));
}

TEST(set_cell_0_8)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    for (int i = 0; i <= 8; i++){
        ASSERT(set_cell(&dummy_cell, i + '0'));
        // THEN
        ASSERT(is_revealed(dummy_cell));
        ASSERT(!is_mine(dummy_cell));
        ASSERT(is_revealed_num(dummy_cell));
        ASSERT(get_number(dummy_cell) == i);
    }

}

TEST(set_cell_9)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(!set_cell(&dummy_cell, '9'));
}

TEST(set_cell_invalid_char)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(!set_cell(&dummy_cell, 'P'));
    ASSERT(!set_cell(&dummy_cell, 'q'));
    ASSERT(!set_cell(&dummy_cell, 'V'));
    ASSERT(!set_cell(&dummy_cell, '@'));

}

TEST(set_cell_flag)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, 'F'));
    // THEN
    ASSERT(!is_revealed(dummy_cell));
    ASSERT(is_flag(dummy_cell));
    ASSERT(is_mine(dummy_cell));
}

TEST(set_cell_wrong_flag)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, 'W'));
    // THEN
    ASSERT(!is_revealed(dummy_cell));
    ASSERT(is_flag(dummy_cell));
    ASSERT(!is_mine(dummy_cell));
}

TEST(set_cell_covered)
{
    // GIVEN
    uint16_t dummy_cell = 0;
    // DO
    ASSERT(set_cell(&dummy_cell, 'X'));
    // THEN
    ASSERT(!is_revealed(dummy_cell));
}

TEST(postprocess_on_simple_board_3by3_board)
{
    // GIVEN
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    fill_board_value(size, size, board, '.'); // function from test utils
    set_cell(&(board[1][1]), 'M');
    // THEN
    ASSERT(postprocess(size, size, board) == 1); // it has one mine
}

TEST(postprocess_corner_mines)
{
    // GIVEN
    const size_t size = 15;
    uint16_t board[15][15] = { 0 };
    fill_board_value(size, size, board, '.');
    // THEN
    set_cell(&(board[0][0]), 'M');
    ASSERT(postprocess(size, size, board) == -1);
    set_cell(&(board[0][0]), '.');
    set_cell(&(board[0][14]), 'M');
    ASSERT(postprocess(size, size, board) == -1);
    set_cell(&(board[0][14]), '.');
    set_cell(&(board[14][0]), 'M');
    ASSERT(postprocess(size, size, board) == -1);
    set_cell(&(board[14][0]), '.');
    set_cell(&(board[14][14]), 'M');
    ASSERT(postprocess(size, size, board) == -1);
}

TEST(postprocess_min_max_size)
{
    // GIVEN
    size_t size = 2;
    uint16_t board[2][2] = { 0 };
    fill_board_value(size, size, board, '.'); // function from test utils
    // THEN
    ASSERT(postprocess(size, size, board) == -1);
    size = 100;
    uint16_t boardMax[100][100] = {0};
    fill_board_value(size, size, boardMax, '.');
    ASSERT(postprocess(size, size, boardMax) == -1);
}

TEST(postprocess_wrong_mine_count)
{
    const size_t size = 15;
    uint16_t board[15][15] = { 0 };
    fill_board_value(size, size, board, '.');

    set_cell(&(board[3][3]), '3');
    set_cell(&(board[3][2]), 'M');
    set_cell(&(board[3][4]), 'M');
    ASSERT(postprocess(size, size, board) == -1);
    set_cell(&(board[2][3]), 'M');
    set_cell(&(board[4][3]), 'M');
    ASSERT(postprocess(size, size, board) == -1);
}

TEST(postprocess_min_max_mines)
{
    // GIVEN
    const size_t size = 8;
    uint16_t board[8][8] = { 0 };
    fill_board_value(size, size, board, '.'); // function from test utils
    set_cell(&(board[3][6]), 'M');
    ASSERT(postprocess(size, size, board) == 1); // it has one mine

    fill_board_value(size, size, board, 'M'); // function from test utils
    set_cell(&(board[0][0]), 'x');
    set_cell(&(board[0][7]), 'x');
    set_cell(&(board[7][0]), 'x');
    set_cell(&(board[7][7]), 'x');
    ASSERT(postprocess(size, size, board) == 60);

}

TEST(postprocess_mine_count)
{
    const size_t size = 15;
    uint16_t board[15][15] = { 0 };
    fill_board_value(size, size, board, '.');
    set_cell(&(board[3][2]), 'M');
    set_cell(&(board[3][4]), 'M');
    set_cell(&(board[2][3]), 'M');
    set_cell(&(board[3][3]), '3');
    ASSERT(postprocess(size, size, board) == 3);
}

TEST(set_adj_mines)
{
    const size_t size = 15;
    uint16_t board[15][15] = { 0 };
    fill_board_value(size, size, board, '.');
    set_cell(&(board[3][2]), 'M');
    set_cell(&(board[3][4]), 'M');
    set_cell(&(board[2][3]), 'M');
    set_cell(&(board[3][3]), '3');
    ASSERT(get_number(board[3][3]) == set_adj_mines(size, size, board, 3, 3));
}

TEST(postprocess_no_mines)
{
    const size_t size = 15;
    uint16_t board[15][15] = { 0 };
    fill_board_value(size, size, board, '.');
    ASSERT(postprocess(size, size, board) == -1);
}

TEST(postprocess_adj_mines_check)
{
    const size_t size = 3;
    uint16_t board[3][3] = {0};
    INPUT_STRING("x.x"
                 "wfm"
                 "12x");
    load_board(size, size, board);
    ASSERT(!is_revealed(board[0][0]));
    ASSERT(!is_mine(board[0][0]));
    ASSERT(!is_flag(board[0][0]));
    ASSERT(get_number(board[0][0]) == 1);

    ASSERT(is_revealed(board[0][1]));
    ASSERT(!is_mine(board[0][1]));
    ASSERT(!is_flag(board[0][1]));
    ASSERT(get_number(board[0][1]) == 2);

    ASSERT(!is_revealed(board[0][2]));
    ASSERT(!is_mine(board[0][2]));
    ASSERT(!is_flag(board[0][2]));
    ASSERT(get_number(board[0][2]) == 2);

    ASSERT(!is_revealed(board[1][0]));
    ASSERT(!is_mine(board[1][0]));
    ASSERT(is_flag(board[1][0]));
    ASSERT(get_number(board[1][0]) == 1);

    ASSERT(!is_revealed(board[1][1]));
    ASSERT(is_mine(board[1][1]));
    ASSERT(is_flag(board[1][1]));
    ASSERT(get_number(board[1][1]) == 0);

    ASSERT(!is_revealed(board[1][2]));
    ASSERT(is_mine(board[1][2]));
    ASSERT(!is_flag(board[1][2]));
    ASSERT(get_number(board[1][2]) == 0);

    ASSERT(is_revealed(board[2][0]));
    ASSERT(!is_mine(board[2][0]));
    ASSERT(!is_flag(board[2][0]));
    ASSERT(get_number(board[2][0]) == 1);

    ASSERT(is_revealed(board[2][1]));
    ASSERT(!is_mine(board[2][1]));
    ASSERT(!is_flag(board[2][1]));
    ASSERT(get_number(board[2][1]) == 2);

    ASSERT(!is_revealed(board[2][2]));
    ASSERT(!is_mine(board[2][2]));
    ASSERT(!is_flag(board[2][2]));
    ASSERT(get_number(board[2][2]) == 2);


}

TEST(show_cell_on_3by3_board)
{
    // GIVEN
    const size_t size = 4;
    uint16_t board[4][4] = { 0 };
    const char *input = "XXX2"
                        "XWFf"
                        "XMX2";
    // THEN
    fill_board(size, size, board, input); // function from test utils

    ASSERT(show_cell(board[0][3]) == '2');
    ASSERT(show_cell(board[0][0]) == 'X');
    ASSERT(show_cell(board[1][1]) == 'F'); // this is a wrong flag, but in print it should be shown as flag
    ASSERT(show_cell(board[1][2]) == 'F'); // this is a correct flag
    ASSERT(show_cell(board[2][1]) == 'X'); // this is mine, but it should not be revealed
    ASSERT(show_cell(board[2][2]) == 'X');
}

TEST(print_revealed_all_but_mine_board)
{
    // GIVEN
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    const char *input = "..."
                        ".M."
                        "...";
    fill_board(size, size, board, input);

    // THEN
    ASSERT(print_board(size, size, board) == 0);

    ASSERT_FILE(stdout, "     0   1   2 \n"
                        "   +---+---+---+\n"
                        " 0 | 1 | 1 | 1 |\n"
                        "   +---+---+---+\n"
                        " 1 | 1 |XXX| 1 |\n"
                        "   +---+---+---+\n"
                        " 2 | 1 | 1 | 1 |\n"
                        "   +---+---+---+\n");
}

TEST(load_rev_undef_1_mine)
{
    // GIVEN
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    const char *input = "..."
                        ".M."
                        "...";
    fill_board(size, size, board, input);

    // THEN
    for (size_t r = 0; r < size; r++){
        for (size_t c = 0; c < size; c++){
            uint16_t cell = board[r][c];
            if (r == 1 && c == 1){
                ASSERT(!is_revealed(cell));
                ASSERT(get_number(cell) == 0);
                ASSERT(is_mine(cell));
                ASSERT(!is_flag(cell));
            }
            else {
                ASSERT(is_revealed(cell));
                ASSERT(get_number(cell) == 1);
                ASSERT(!is_mine(cell));
                ASSERT(!is_flag(cell));
            }
        }
    }
}

TEST(load_unrevealed_3by3_board)
{
    // GIVEN
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    INPUT_STRING("XXX"
                 "XMX"
                 "XXX"); // the content of STDIN for LOAD

    // THEN
    ASSERT(load_board(size, size, board) == 1); // it has one mine

    ASSERT(!is_revealed(board[0][0]));
    ASSERT(get_number(board[0][0]) == 1);
    ASSERT(is_mine(board[1][1]));
    ASSERT(!is_mine(board[1][0]));
}

TEST(load_no_mines)
{
    const size_t size = 4;
    uint16_t board[4][4] = {0};
    INPUT_STRING("XX.."
                 "W..X"
                 "WWXX"
                 "X...");
    ASSERT(load_board(size, size, board) == -1);
}

TEST(load_corner_mine)
{
    const size_t size = 3;
    uint16_t board[3][3] = {0};
    INPUT_STRING("XXX"
                 "xxX"
                 "XxM");
    int load = load_board(size, size, board);
    ASSERT(!is_revealed(board[1][1]));
    ASSERT(is_corner_cell(2, 2, 3, 3));
    ASSERT(is_mine(board[2][2]));
    ASSERT(load == -1);
}

TEST(load_invalid_chars)
{
    // GIVEN
    const size_t size = 3;
    uint16_t board[3][3] = { 0 };
    INPUT_STRING("q..q."
                 ".M.q"
                 ".qq..");
    ASSERT(load_board(size, size, board) == 1);

    // THEN
    for (size_t r = 0; r < size; r++){
        for (size_t c = 0; c < size; c++){
            uint16_t cell = board[r][c];
            if (r == 1 && c == 1){
                ASSERT(!is_revealed(cell));
                ASSERT(get_number(cell) == 0);
                ASSERT(is_mine(cell));
                ASSERT(!is_flag(cell));
            }
            else {
                ASSERT(is_revealed(cell));
                ASSERT(get_number(cell) == 1);
                ASSERT(!is_mine(cell));
                ASSERT(!is_flag(cell));
            }
        }
    }
}

TEST(reveal_cell_invalid)
{
    const size_t size = 3;
    uint16_t board[3][3] = {0};
    INPUT_STRING("x.x"
                 "wfm"
                 "xxx");
    load_board(size, size, board);
    ASSERT(reveal_cell(size, size, board, 0 , 1) == -1);
    ASSERT(reveal_cell(size, size, board, 1 , 0) == -1);
    ASSERT(reveal_cell(size, size, board, 1 , 1) == -1);
    ASSERT(reveal_cell(size, size, board, 3 , 1) == -1);
}

TEST(reveal_single_invalid)
{
    const size_t size = 3;
    uint16_t board[3][3] = {0};
    INPUT_STRING("x.x"
                 "wfm"
                 "xxx");
    load_board(size, size, board);
    ASSERT(reveal_single(&board[0][1]) == -1);
    ASSERT(reveal_single(&board[1][0]) == -1);
    ASSERT(reveal_single(&board[1][1]) == -1);
    ASSERT(reveal_single(NULL) == -1);
}

TEST(is_revealed_num)
{
    ASSERT(is_revealed_num(64));
}

TEST(is_corner_cell)
{
    ASSERT(is_corner_cell(0, 0, 5, 5));
    ASSERT(is_corner_cell(0, 4, 5, 5));
    ASSERT(is_corner_cell(4, 0, 5, 5));
    ASSERT(is_corner_cell(4, 4, 5, 5));
    ASSERT(!is_corner_cell(1, 3, 5, 5));
}

TEST(random_board)
{
    const size_t size = 5;
    uint16_t board[5][5] = {0};
//    generate_random_board(size, size, board, 1);
    ASSERT(generate_random_board(size, size, board, 1) == -1);
    int mine_count = 0;
    for (size_t row = 0; row < size; row++){
        for (size_t col = 0; col < size; col++) {
            if (is_mine(board[row][col])){
                mine_count++;
            }
            else {
                ASSERT(!is_revealed(board[row][col]));
                ASSERT(!is_mine(board[row][col]));
                ASSERT(!is_flag(board[row][col]));
            }
        }
    }
    ASSERT(mine_count == 0);
}

TEST(find_mines)
{
    const size_t size = 5;
    uint16_t board[5][5] = {0};
    INPUT_STRING("xx11x"
                 "xx111"
                 "xx111"
                 "xxxxx"
                 "xxxxx");
    ASSERT(find_mines(size, size, board) == 1);
}
