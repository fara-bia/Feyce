/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "calc_movs.h"
#include "evaluate.h"
#include "mutuals.h"
#include "decode.h"

// <FEN> <Depth>
int main (int argc, char* argv[]) {
    printf("You have run %s\n", argv[0]);

    int depth, halfmove, fullmove, turn, eval_outp, psbmoves[218] = {0};
    char* fen = "";

    switch (argc) {
        case 3:
            fen = argv[1];
            depth = atoi(argv[2]);
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


    calculate_moves(board, psbmoves, turn);

    #ifdef DEBUG
    eval_outp = evaluate_board(board);
    printf("Evaluation result for %s is: %d\n", turn == WHITE_TURN ? "white" : "black", eval_outp);
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
