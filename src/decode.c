/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include "mutuals.h"
#include "decode.h"

void decode_fen (char* fen, int* board, int* halfmove, int* fullmove) {
    int i = 0;
    int i_2;
    repeat(64) {
        if (fen[i] == '/') {
            i++;
        }
        board[i] = fen[i];
        if (fen[i] > 48 && fen[i] < 57) {
            i_2 = i;
            repeat(fen[i] - '0') {
                board[i_2] = 0;
                i_2++;
            }
            i+=(fen[i] - '1');
        }
        i++;
        if (i > 64) {
            break;
        }
    }
    i++;
    if (fen[i] == 'w') {
        board[0] = 0;
    } else if (fen[i] == 'b'){
        board[0] = 1;
    }
    i++;
}
