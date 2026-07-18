/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */
#include "evaluate.h"
#include "mutuals.h"

/* bot's centipawn values */
#define PAWNVAL 100
#define KNIGHTVAL 300
#define BISHOPVAL 300
#define ROOKVAL 500
#define QUEENVAL 900

static int piece_val(int piece, int greed, int turn);
static int checkpiece(int piece);

/* checks for material, development and proximity */
int evaluate_board (int* board, int greed) {
    int turn = board[0], i = 1;
    int material_val = 0, development = 0, proximity = 0;

    // material value
    repeat(64) {
        material_val += piece_val(board[i], greed, turn);

        i++;
    }

    /* development */


    /* proximity (unfinished) */
    i = 1;
    repeat(64) {
        if (!checkpiece(board[i])) {
            continue;
        }
        proximity +=

        i++;
    }

    return 0;
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

static int checkpiece (int piece) {
    if (piece >= WHITE_PAWN || piece <= WHITE_KING) return 1;
    else return 0;
}

static int calc_dist_betw_squares (int square1, int square2) {
    int sqdistance;

    return sqdistance;
}
