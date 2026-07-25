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
    int depth, halfmove, fullmove, turn;
    short psbmoves[MAX_POSSIBLE_MOVES] = {0};
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

    if (depth > 15) {
        printf("Max depth is 15\n");
        return 1;
    }

    int boardsize = /* active color */ (1) + /* piece placement */ (64) + /* castling availability */ (2) + (2) \
        + /* en passant target square */ (1) + /* null square */ (1);
    int board[boardsize];

    if(decode_fen(fen, board, &halfmove, &fullmove)) {
        printf("Your board is successfully received.\n");
    }
    turn = board[0];

    tt_clear();
    calculate_moves(board, psbmoves, turn);

    if (psbmoves[0] == END_OF_PBMOVES) {
        if (checkforcheck(board, turn)) {
            printf("Checkmate\n");
        } else {
            printf("Stalemate\n");
        }
        return 0;
    }

    int best_eval = (turn == WHITE_TURN) ? -999999 : 999999;
    short best_move[4] = {0};
    int best_move_type = 0;
    int alpha = -999999;
    int beta = 999999;
    int next_turn = (turn == WHITE_TURN) ? BLACK_TURN : WHITE_TURN;

    for (int i = 0; psbmoves[i] != END_OF_PBMOVES; ) {
        if (psbmoves[i] == NO_MOVE) {
            i += 2;
            continue;
        }

        int eval;

        if (psbmoves[i] == PROMOTE_MOVE) {
            /* [PROMOTE_MOVE][src][dst][piece] */
            int src = psbmoves[i + 1], dst = psbmoves[i + 2], promo = psbmoves[i + 3];
            int save_src = board[src], save_dst = board[dst];
            board[dst] = promo;
            board[src] = EMPTY_SQUARE;
            eval = minimax(board, depth - 1, next_turn, alpha, beta);
            board[src] = save_src;
            board[dst] = save_dst;
            if ((turn == WHITE_TURN && eval > best_eval) || (turn == BLACK_TURN && eval < best_eval)) {
                best_eval = eval;
                best_move[0] = (short)src;
                best_move[1] = (short)dst;
                best_move[2] = (short)promo;
                best_move_type = PROMOTE_MOVE;
                if (turn == WHITE_TURN) alpha = eval; else beta = eval;
            }
            i += 4;
        } else if (psbmoves[i] == ENPASSANTMOV) {
            /* [ENPASSANTMOV][src][dst_ep_square] */
            int src = psbmoves[i + 1], dst = psbmoves[i + 2];
            int cap_sq = dst + (getrowindex(dst) == 6 ? -8 : 8);
            int save_src = board[src], save_cap = board[cap_sq];
            board[dst] = board[src];
            board[cap_sq] = EMPTY_SQUARE;
            board[src] = EMPTY_SQUARE;
            eval = minimax(board, depth - 1, next_turn, alpha, beta);
            board[src] = save_src;
            board[cap_sq] = save_cap;
            board[dst] = EMPTY_SQUARE;
            if ((turn == WHITE_TURN && eval > best_eval) || (turn == BLACK_TURN && eval < best_eval)) {
                best_eval = eval;
                best_move[0] = (short)src;
                best_move[1] = (short)dst;
                best_move_type = ENPASSANTMOV;
                if (turn == WHITE_TURN) alpha = eval; else beta = eval;
            }
            i += 3;
        } else if (psbmoves[i] == WHITE_KING_CASTLEMOV || psbmoves[i] == WHITE_QUEEN_CASTLEMOV ||
                   psbmoves[i] == BLACK_KING_CASTLEMOV || psbmoves[i] == BLACK_QUEEN_CASTLEMOV) {
            /* [CASTLE_TOKEN][king_dst] */
            int move_type = psbmoves[i];
            int dst = psbmoves[i + 1];
            int src = (move_type == WHITE_QUEEN_CASTLEMOV || move_type == BLACK_QUEEN_CASTLEMOV)
                      ? dst + 2 : dst - 2;
            int save_src = board[src], save_dst = board[dst];
            board[dst] = board[src];
            board[src] = EMPTY_SQUARE;
            eval = minimax(board, depth - 1, next_turn, alpha, beta);
            board[src] = save_src;
            board[dst] = save_dst;
            if ((turn == WHITE_TURN && eval > best_eval) || (turn == BLACK_TURN && eval < best_eval)) {
                best_eval = eval;
                best_move[0] = (short)src;
                best_move[1] = (short)dst;
                best_move_type = move_type;
                if (turn == WHITE_TURN) alpha = eval; else beta = eval;
            }
            i += 2;
        } else {
            int src = psbmoves[i], dst = psbmoves[i + 1];
            int save_src = board[src], save_dst = board[dst];
            board[dst] = board[src];
            board[src] = EMPTY_SQUARE;
            eval = minimax(board, depth - 1, next_turn, alpha, beta);
            board[src] = save_src;
            board[dst] = save_dst;
            if ((turn == WHITE_TURN && eval > best_eval) || (turn == BLACK_TURN && eval < best_eval)) {
                best_eval = eval;
                best_move[0] = (short)src;
                best_move[1] = (short)dst;
                best_move_type = src;
                if (turn == WHITE_TURN) alpha = eval; else beta = eval;
            }
            i += 2;
        }
    }

    char notation[16];
    move_to_notation(board, best_move, notation);

    printf("Engine moves: %s\n", notation);

    return 0;
}
