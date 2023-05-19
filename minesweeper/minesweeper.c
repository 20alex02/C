#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "minesweeper.h"

// CELL REPRESENTATION
#define ADJ_MINES_MASK 15 // 0000 0000 0000 1111
#define MINE 16           // 0000 0000 0001 0000
#define FLAG 32           // 0000 0000 0010 0000
#define REVEALED 64       // 0000 0000 0100 0000

/* ************************************************************** *
 *                         HELP FUNCTIONS                         *
 * ************************************************************** */

bool is_flag(uint16_t cell)
{
    return cell & FLAG;
}

bool is_wrong_flag(uint16_t cell)
{
    return is_flag(cell) && !is_mine(cell);
}

bool is_mine(uint16_t cell)
{
    return cell & MINE;
}

bool is_revealed(uint16_t cell)
{
    return cell & REVEALED;
}

bool is_revealed_undef(uint16_t cell)
{
    return (cell & (REVEALED + ADJ_MINES_MASK)) == REVEALED + ADJ_MINES_MASK;
}

bool is_revealed_num(uint16_t cell)
{
    return is_revealed(cell) && !is_revealed_undef(cell);
}

int get_number(uint16_t cell)
{
    if (is_mine(cell)){
        return 0;
    }
    return cell & ADJ_MINES_MASK;
}

/* ************************************************************** *
 *                         INPUT FUNCTIONS                        *
 * ************************************************************** */

bool is_valid_cell(int row, int col, int rows, int cols)
{
    return (row >= 0 && row < rows && col >= 0 && col < cols);
}

bool is_corner_cell(size_t row, size_t col, size_t rows, size_t cols)
{
    return ((row == 0 && col == 0) ||
            (row == 0 && col == cols - 1) ||
            (row == rows - 1 && col == 0) ||
            (row == rows - 1 && col == cols - 1));
}

int set_adj_mines(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    int mines = 0;
    for (int r = -1; r <= 1; r++){
        for (int c = -1; c <= 1; c++){
            if (r == 0 && c == 0){
                continue;
            }
            if (is_valid_cell((int) row + r, (int) col + c, (int) rows, (int) cols) &&
                is_mine(board[row + r][col + c])){
                mines++;
            }
        }
    }
    board[row][col] = (board[row][col] & ~ADJ_MINES_MASK) | mines;
    return mines;
}

bool set_cell(uint16_t *cell, char val)
{
    if (cell == NULL){
        return false;
    }
    if (isdigit(val) && val != '9'){
        *cell = REVEALED + (val - '0');
        return true;
    }
    val = (char) toupper(val);
    switch (val){
        case 'X':
            *cell = 0;
            break;
        case 'M':
            *cell = MINE;
            break;
        case 'F':
            *cell = MINE + FLAG;
            break;
        case 'W':
            *cell = FLAG;
            break;
        case '.':
            *cell = REVEALED + ADJ_MINES_MASK;
            break;
        default:
            return false;
    }
    return true;
}

int load_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    bool valid_input;
    int input;
    for (size_t row = 0; row < rows; row++){
        for (size_t col = 0; col < cols; col++){
            valid_input = false;
            while (!valid_input){
                if ((input = getchar()) == EOF){
                    return - 1;
                }
                valid_input = set_cell(&board[row][col], (char) input);
            }
        }
    }
    return postprocess(rows, cols, board);
}

int postprocess(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int mines = 0;
    if (rows < MIN_SIZE || cols < MIN_SIZE || rows > MAX_SIZE || cols > MAX_SIZE){
        return -1;
    }
    for (size_t row = 0; row < rows; row++){
        for (size_t col = 0; col < cols; col++){
            if (is_mine(board[row][col])){
                mines++;
                if (is_corner_cell(row, col, rows, cols)){
                    return -1;
                }
            }
            if (is_revealed_num(board[row][col])){
                int adj_mines = get_number(board[row][col]);
                if (adj_mines != set_adj_mines(rows, cols, board, row, col)){
                    return -1;
                }
            }
            else {
                if (!is_mine(board[row][col])){
                    set_adj_mines(rows, cols, board, row, col);
                }
            }
        }
    }
    if (mines == 0){
        return -1;
    }
    return mines;
}

/* ************************************************************** *
 *                        OUTPUT FUNCTIONS                        *
 * ************************************************************** */

void print_nums(size_t num)
{
    if (num < 10){
        printf(" %lu ", num);
    }
    else {
        printf("%lu ", num);
    }
}

int print_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    for (size_t row = 0; row <= 2 * rows + 1; row++){
        for (size_t col = 0; col <= cols; col++){
            uint16_t cell = board[row / 2 - 1][col - 1];
            // [0][0] cell
            if (row == 0 && col == 0){
                printf("    ");
            }
            // first row
            else if (row == 0){
                print_nums(col - 1);
                if (col != cols){
                    printf(" ");
                }
            }
            // first col
            else if (col == 0){
                if (row % 2 == 0){
                    print_nums(row / 2 - 1);
                    printf("|");
                }
                else {
                    printf("   +");
                }
            }
            else if (row % 2 == 1){
                printf("---+");
            }
            else {
                if (is_revealed(cell)){
                    if (is_mine(cell)){
                        printf(" M |");
                    }
                    else {
                        int adj_mines = get_number(cell);
                        if (adj_mines == 0){
                            printf("   |");
                        }
                        else{
                            printf(" %d |", adj_mines);
                        }
                    }
                }
                else {
                    if (is_flag(cell)){
                        printf("_F_|");
                    }
                    else {
                        printf("XXX|");
                    }
                }
            }
            if (col == cols){
                printf("\n");
            }
        }
    }
    return 0;
}

char show_cell(uint16_t cell)
{
    if (is_revealed(cell)){
        if (is_mine(cell)){
            return 'M';
        }
        int adj_mines = cell & ADJ_MINES_MASK;
        if (adj_mines == 0){
            return ' ';
        }
        return (char) ('0' + adj_mines);
    }
    if (is_flag(cell)){
        return 'F';
    }
    return 'X';
}

/* ************************************************************** *
 *                    GAME MECHANIC FUNCTIONS                     *
 * ************************************************************** */

int reveal_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (!is_valid_cell((int) row, (int) col, (int) rows, (int) cols)){
        return -1;
    }
    int ret_value = reveal_single(&board[row][col]);
    if (ret_value == 0 && get_number(board[row][col]) == 0){
        reveal_floodfill(rows, cols, board, row, col);
    }
    return ret_value;
}

int reveal_single(uint16_t *cell)
{
    if (cell == NULL || is_revealed(*cell) || is_flag(*cell)){
        return -1;
    }
    *cell |= REVEALED;
    if (is_mine(*cell)){
        return 1;
    }
    return 0;
}

void reveal_floodfill(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    int adj_mines;
    int reveal;
    for (int r = -1; r <= 1; r++){
        for (int c = -1; c <= 1; c++){
            if (r == 0 && c == 0){
                continue;
            }
            if (is_valid_cell((int) row + r, (int) col + c, (int) rows, (int) cols)){
                adj_mines = get_number(board[row + r][col + c]);
                reveal = reveal_single(&board[row + r][col + c]);
                if (reveal == 0 && adj_mines == 0){
                    reveal_floodfill(rows, cols, board, row + r, col + c);
                }
                if (is_wrong_flag(board[row + r][col + c])){
                    board[row + r][col + c] |= REVEALED;
                    board[row + r][col + c] ^= FLAG;
                    if (adj_mines == 0){
                        reveal_floodfill(rows, cols, board, row + r, col + c);
                    }
                }
            }
        }
    }
}

int flag_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    int mines = 0;
    int flags = 0;
    board[row][col] ^= FLAG;
    for (size_t r = 0; r < rows; r++){
        for (size_t c = 0; c < cols; c++){
            if (is_mine(board[r][c])){
                mines++;
            }
            if (is_flag(board[r][c])){
                flags++;
            }
        }
    }
    return mines - flags;
}

bool is_solved(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int covered = 0;
    int mines = 0;
    for (size_t r = 0; r < rows; r++){
        for (size_t c = 0; c < cols; c++){
            if (is_mine(board[r][c])){
                mines++;
            }
            if (!is_revealed(board[r][c])){
                covered++;
            }
        }
    }
    return covered == mines;
}

/* ************************************************************** *
 *                         BONUS FUNCTIONS                        *
 * ************************************************************** */

int generate_random_board(size_t rows, size_t cols, uint16_t board[rows][cols], size_t mines)
{
    srand(time(NULL));
    size_t mine_coords;
    size_t free_cells = rows * cols - 4;
    bool already_set = false;
    if (rows < MIN_SIZE || rows > MAX_SIZE || cols < MIN_SIZE || cols > MAX_SIZE ){
        return -1;
    }
    if (mines == 0 || mines > free_cells){
        return -1;
    }
    for (size_t row = 0; row < rows; row++){
        for (size_t col = 0; col < cols; col++){
            set_cell(&board[row][col], 'X');
        }
    }
    for (size_t mine = 0; mine < mines; mine++){
        mine_coords = rand() % free_cells;
        for (size_t row = 0; row < rows; row++){
            if (already_set){
                already_set = false;
                break;
            }
            for (size_t col = 0; col < cols; col++){
                if (is_corner_cell(row, col, rows, cols)) {
                    continue;
                }
                if (!is_mine(board[row][col])) {
                    if (mine_coords == 0) {
                        set_cell(&board[row][col], 'M');
                        already_set = true;
                        break;
                    }
                    mine_coords--;
                }
            }
            break;
        }
        free_cells--;
    }
    return postprocess(rows, cols, board);
}

bool is_valid_input(char input)
{
    if ((isdigit(input) && input != '9') || input == 'X' || input == 'x'){
        return true;
    }
    return false;
}

int find_adj_mines(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    int covered = 0;
    int new_flags = 0;
    for (int r = -1; r <= 1; r++){
        for (int c = -1; c <= 1; c++){
            if (is_valid_cell((int) row + r, (int) col + c, (int) rows, (int) cols)){
                if (!is_revealed(board[row + r][col + c])) {
                    covered++;
                }
            }
        }
    }
    if (covered == get_number(board[row][col])){
        for (int r = -1; r <= 1; r++){
            for (int c = -1; c <= 1; c++){
                if (is_valid_cell((int) row + r, (int) col + c, (int) rows, (int) cols)){
                    if (!is_flag(board[row + r][col + c])) {
                        flag_cell(rows, cols, board, row + r, col + c);
                        new_flags++;
                    }
                }
            }
        }
    }
    return new_flags;
}

int find_mines(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    int mines = 0;
    int input;
    for (size_t row = 0; row < rows; row++){
        for (size_t col = 0; col < cols; col++){
            input = getchar();
            if (input == EOF || !is_valid_input((char) input)){
                return -1;
            }
            set_cell(&board[row][col], (char) input);
        }
    }

    for (size_t row = 0; row < rows; row++){
        for (size_t col = 0; col < cols; col++){
            if (is_revealed(board[row][col])){
                mines += find_adj_mines(rows, cols, board, row, col);
            }
        }
    }
    return mines;
}
