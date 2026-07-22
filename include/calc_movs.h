/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#ifndef CALC_MOVS
#define CALC_MOVS

void calculate_moves(int* board, int* psbmoves, int turn);
int checkforcheck(int* board, int turn);
int checkforcheckmate(int* board);

#endif
