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
#define NULL_SQUARE 70
#define NULL_VAL 863 /* no special reason to have this */
#define ENPASSANTSQ 69
#define ENPASSANTMOV -1
#define WHITE_KING_CASTLEMOV -2
#define BLACK_KING_CASTLEMOV -3
#define WHITE_QUEEN_CASTLEMOV -4
#define BLACK_QUEEN_CASTLEMOV -5
#define PROMOTE_MOVE -8
#define WHITE_TURN 1
#define BLACK_TURN 0
#define NO_MOVE -6
#define END_OF_PBMOVES -7
#define KINGSIDE_WHITE 65
#define KINGSIDE_BLACK 67
#define QUEENSIDE_WHITE 66
#define QUEENSIDE_BLACK 68
#define MAX_POSSIBLE_MOVES 512

#endif
