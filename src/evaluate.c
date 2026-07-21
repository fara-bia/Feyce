/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */
#include "evaluate.h"
#include "mutuals.h"
#include <math.h>

/* bot's centipawn values */
#define PAWNVAL 100
#define KNIGHTVAL 300
#define BISHOPVAL 300
#define ROOKVAL 500
#define QUEENVAL 900

static int piece_val(int piece, int greed, int turn);
static int checkpiece(int piece, int turn);
static int calc_dist_betw_squares(int square1, int square2);
static int getrowindex(int square);
static int getcolumnindex(int square);

/* checks for material, development and proximity */
int evaluate_board (int* board, int greed) {
    int turn = board[0], i = 1;
    int material_val = 0, development = 0, proximity = 0;

    // material value
    repeat(64) {
        material_val += piece_val(board[i], greed, turn);

        i++;
    }

    /* development (unstarted) */


    /* proximity */
    i = 1;
    int opponent_king_square, target_piece = turn == WHITE_TURN ? BLACK_KING : WHITE_KING;

    while (1) {
        if (board[i] == target_piece) {
            opponent_king_square = i;
            break;
        }

        i++;
    }

    i = 0;
    repeat(64) {
        ++i;
        if (checkpiece(board[i], turn)) {
            continue;
        }
        if (board[i] == WHITE_KING || board[i] == BLACK_KING) { // checking both kings since one is already skipped
            continue;
        }

        proximity += calc_dist_betw_squares(i, opponent_king_square);
    }

    return (material_val + development - proximity);
}

static int piece_val (int piece, int greed, int turn) {
    int return_val;

    if (turn == WHITE_TURN) {
        switch (piece) {
            case WHITE_PAWN:
                return_val = PAWNVAL;
                break;
            case WHITE_KNIGHT:
                return_val = KNIGHTVAL;
                break;
            case WHITE_BISHOP:
                return_val = BISHOPVAL;
                break;
            case WHITE_ROOK:
                return_val = ROOKVAL;
                break;
            case WHITE_QUEEN:
                return_val = QUEENVAL;
                break;
            case WHITE_KING:
                return_val = greed;
                break;
            default:
                return_val = 0;
                break;
        }
    } else {
        switch (piece) {
            case BLACK_PAWN:
                return_val = PAWNVAL;
                break;
            case BLACK_KNIGHT:
                return_val = KNIGHTVAL;
                break;
            case BLACK_BISHOP:
                return_val = BISHOPVAL;
                break;
            case BLACK_ROOK:
                return_val = ROOKVAL;
                break;
            case BLACK_QUEEN:
                return_val = QUEENVAL;
                break;
            case BLACK_KING:
                return_val = greed;
                break;
            default:
                return_val = 0;
                break;
        }
    }

    return return_val;
}

static int checkpiece (int piece, int turn) {
    if (turn == WHITE_TURN) {
        if (piece >= BLACK_PAWN && piece <= BLACK_KING) return 0;
        else return 1;
    } else {
        if (piece >= WHITE_PAWN && piece <= WHITE_KING) return 0;
        else return 1;
    }
}

static int calc_dist_betw_squares (int square1, int square2) { //euclidian
    int sqdistance, row1, row2, column1, column2;

    row1 = getrowindex(square1);
    row2 = getrowindex(square2);
    column1 = getcolumnindex(square1);
    column2 = getcolumnindex(square2);

    sqdistance = (int) sqrt((double) ((row2 - row1)*(row2 - row1) + (column2 - column1)*(column2 - column1)));

    return sqdistance;
}

static int getrowindex (int square) {
    return ((square - 1) / 8) + 1;
}

static int getcolumnindex (int square) {
    return ((square - 1) % 8) + 1;
}
