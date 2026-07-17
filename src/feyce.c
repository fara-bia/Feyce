/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "mutuals.h"
#include "decode.h"

// <FEN> <Depth> <Greed>
int main (int argc, char* argv[]) {
    printf("You have run %s\n", argv[0]);

    int depth, greed, halfmove, fullmove;
    char* fen = "";

    switch (argc) {
        case 3:
            fen = argv[1];
            greed = 10000;
            break;
        case 4:
            fen = argv[1];
            greed = atoi(argv[2]);
            break;
        default:
            printf("Invalid arguments!\n");
            return 1;
    }

    int boardsize = /* turn */ (1) + /* board */ (64) + /* castling */ (2) + (2) \
        + /* en passant */ (1);
    int board[boardsize];

    // decode_fen(fen, board);

    printf("Your board is successfully received.\n");

    return 0;
}
