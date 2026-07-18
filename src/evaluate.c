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

int evaluate_board (int* board, int greed) {
    int turn = board[0], i = 1;
    int material_val = 0, development, proximity;

    // material value
    repeat(64) {
        material_val += piece_val(board[i], greed, turn);

        i++;
    }

    return material_val;
    // return 0;
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
