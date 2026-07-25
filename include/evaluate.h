/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#ifndef EVALUATE
#define EVALUATE

int evaluate_board(int* board);
int checkpiece(int piece, int turn);
int calc_dist_betw_squares(int square1, int square2);
int getrowindex(int square);
int getcolumnindex(int square);

#endif
