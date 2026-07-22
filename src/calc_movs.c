/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include "calc_movs.h"
#include "evaluate.h"
#include "mutuals.h"

static int squareval(int row, int column);

void calculate_moves (int* board, int* psbmoves, int turn) {

}

int checkforcheck (int* board, int turn) {
    int kingsq, row, column, spare;
    int i = 1;

    if (turn == WHITE_TURN) {
        repeat(64) {
            if (board[i] == WHITE_KING) {
                kingsq = i;

                break;
            }

            i++;
        }
        row = getrowindex(kingsq);
        column = getcolumnindex(kingsq);

        // check for knights
        spare = squareval(row + 1, column + 2);
        if (board[spare] == BLACK_KNIGHT) {
            // to be continued
        }

    }

    return 0;
}

int checkforcheckmate (int* board) {

}

static int squareval (int row, int column) {
    return (((row - 1) * 8) + column);
}
