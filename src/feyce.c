/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "evaluate.h"
#include "mutuals.h"
#include "decode.h"

#define DEFAULT_GREED 1000000

// <FEN> <Depth> <Greed>
int main (int argc, char* argv[]) {
    printf("You have run %s\n", argv[0]);

    int depth, greed, halfmove, fullmove, turn, eval_outp;
    char* fen = "";

    switch (argc) {
        case 3:
            fen = argv[1];
            depth = atoi(argv[2]);
            greed = DEFAULT_GREED;
            break;
        case 4:
            fen = argv[1];
            depth = atoi(argv[2]);
            greed = atoi(argv[3]);
            break;
        default:
            printf("Invalid arguments!\n");
            return 1;
    }

    int boardsize = /* active color */ (1) + /* piece placement */ (64) + /* castling availability */ (2) + (2) \
        + /* en passant target square */ (1);
    int board[boardsize];

    if(decode_fen(fen, board, &halfmove, &fullmove)) {
        printf("Your board is successfully received.\n");
    }

    turn = board[0];

    /* temporary */
    eval_outp = evaluate_board(board, greed);

    printf("Evaluation result for %s is: %d\n", turn == WHITE_TURN ? "white" : "black", eval_outp);

    #ifdef DEBUG
    int debugindex = 1;
    repeat(64) {
        printf("square %d of the board contains: %d\n", debugindex, board[debugindex]);
        debugindex++;
    }
    printf("halfmove: %d\tfullmove: %d\n", halfmove, fullmove);
    printf("en passant square: %d\n", board[69]);
    #endif

    return 0;
}
