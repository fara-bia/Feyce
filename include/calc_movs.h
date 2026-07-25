/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#ifndef CALC_MOVS
#define CALC_MOVS

void calculate_moves(int* board, short* psbmoves, int turn);
int squareval(int row, int column);
int checkforcheck(int* board, int turn);
int is_overflow(int row, int column);
int minimax(int* board, int depth, int turn, int alpha, int beta);
void move_to_notation(int* board, short move_data[], char* notation);
extern void tt_clear(void);

#endif
