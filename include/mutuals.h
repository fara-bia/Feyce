/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#ifndef MUTUALS
#define MUTUALS

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define repeat(n) for (int CONCAT(_i, __LINE__) = 0; CONCAT(_i, __LINE__) < (n); CONCAT(_i, __LINE__)++)

#define BLACK_PAWN 1
#define BLACK_KNIGHT 2
#define BLACK_BISHOP 3
#define BLACK_ROOK 4
#define BLACK_QUEEN 5
#define BLACK_KING 6
#define WHITE_PAWN 7
#define WHITE_KNIGHT 8
#define WHITE_BISHOP 9
#define WHITE_ROOK 10
#define WHITE_QUEEN 11
#define WHITE_KING 12
#define EMPTY_SQUARE 0
#define WHITE_TURN 1
#define BLACK_TURN 0

#endif
