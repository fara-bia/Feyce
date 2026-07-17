/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#ifndef DECODE
#define DECODE

void decode_fen(char* fen, int* board, int* halfmove, int* fullmove);
int piece_int_rep(char piece);

#endif
