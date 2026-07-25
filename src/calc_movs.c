/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include "calc_movs.h"
#include "evaluate.h"
#include "mutuals.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define TRANSPOSITION_TABLE_SIZE 1000000

typedef struct {
    uint64_t hash;
    int eval;
    int depth;
    int flag;  // 0=exact, 1=lower bound, 2=upper bound
} TranspositionEntry;

static TranspositionEntry tt[TRANSPOSITION_TABLE_SIZE];
static int tt_initialized = 0;

static uint64_t zobrist_hash(int* board) {
    uint64_t hash = 0;
    uint64_t seed = 0x9e3779b97f4a7c15ULL;
    for (int i = 1; i <= 64; i++) {
        hash ^= (board[i] * seed) + (i << 24);
    }
    hash ^= board[0] * 0x123456789abcdefULL;
    return hash;
}

void tt_clear(void) {
    memset(tt, 0, sizeof(tt));
    tt_initialized = 1;
}

static void tt_store(uint64_t hash, int eval, int depth, int flag) {
    int idx = hash % TRANSPOSITION_TABLE_SIZE;
    if (tt[idx].depth < depth) {
        tt[idx].hash = hash;
        tt[idx].eval = eval;
        tt[idx].depth = depth;
        tt[idx].flag = flag;
    }
}

/* Returns 1 and sets *out if a usable TT entry is found, else returns 0. */
static int tt_lookup(uint64_t hash, int depth, int alpha, int beta, int* out) {
    int idx = hash % TRANSPOSITION_TABLE_SIZE;
    if (tt[idx].hash == hash && tt[idx].depth >= depth) {
        if (tt[idx].flag == 0) { *out = tt[idx].eval; return 1; }
        if (tt[idx].flag == 1 && tt[idx].eval >= beta) { *out = tt[idx].eval; return 1; }
        if (tt[idx].flag == 2 && tt[idx].eval <= alpha) { *out = tt[idx].eval; return 1; }
    }
    return 0;
}

static void sort_moves_by_priority(int* board, short* psbmoves) {
    /* Bucket into: promotions (3), captures (2), other specials (1), quiet (0) */
    short buckets[4][MAX_POSSIBLE_MOVES];
    int   counts[4] = {0, 0, 0, 0};

    for (int i = 0; psbmoves[i] != END_OF_PBMOVES; ) {
        if (psbmoves[i] == NO_MOVE) {
            /* preserve NO_MOVE pairs in quiet bucket */
            buckets[0][counts[0]++] = NO_MOVE;
            buckets[0][counts[0]++] = NO_MOVE;
            i += 2;
            continue;
        }

        int src = psbmoves[i];
        int bucket;

        if (src == PROMOTE_MOVE) {
            bucket = 3;
            buckets[bucket][counts[bucket]++] = psbmoves[i];
            buckets[bucket][counts[bucket]++] = psbmoves[i + 1];
            buckets[bucket][counts[bucket]++] = psbmoves[i + 2];
            buckets[bucket][counts[bucket]++] = psbmoves[i + 3];
            i += 4;
        } else if (src == ENPASSANTMOV) {
            bucket = 2;
            buckets[bucket][counts[bucket]++] = psbmoves[i];
            buckets[bucket][counts[bucket]++] = psbmoves[i + 1];
            buckets[bucket][counts[bucket]++] = psbmoves[i + 2];
            i += 3;
        } else if (src == WHITE_KING_CASTLEMOV || src == WHITE_QUEEN_CASTLEMOV ||
                   src == BLACK_KING_CASTLEMOV || src == BLACK_QUEEN_CASTLEMOV) {
            bucket = 1;
            buckets[bucket][counts[bucket]++] = psbmoves[i];
            buckets[bucket][counts[bucket]++] = psbmoves[i + 1];
            i += 2;
        } else {
            /* regular move: capture or quiet */
            bucket = (board[psbmoves[i + 1]] != EMPTY_SQUARE) ? 2 : 0;
            buckets[bucket][counts[bucket]++] = psbmoves[i];
            buckets[bucket][counts[bucket]++] = psbmoves[i + 1];
            i += 2;
        }
    }

    int out = 0;
    for (int b = 3; b >= 0; b--) {
        for (int j = 0; j < counts[b]; j++) {
            psbmoves[out++] = buckets[b][j];
        }
    }
    psbmoves[out] = END_OF_PBMOVES;
}

static void add_possible_move(short* psbmoves, int* board, int* psbcounter, int sq1, int sq2row, int sq2column, int turn);
static void try_moves(short* psbmoves, int* board, int* psbcounter, int turn, int square, int type);

static short move_cache[16][MAX_POSSIBLE_MOVES];

void calculate_moves (int* board, short* psbmoves, int turn) {
    int i = 1, psbcounter = 0, sparesq1, sparesq2, checkresult;
    repeat(64) {
        try_moves(psbmoves, board, &psbcounter, turn, i, board[i]);

        i++;
    }
    psbmoves[psbcounter] = END_OF_PBMOVES;

    for (i = 0; psbmoves[i] != END_OF_PBMOVES; i += 2) {
        if (psbmoves[i] == WHITE_KING_CASTLEMOV || psbmoves[i] == WHITE_QUEEN_CASTLEMOV \
        || psbmoves[i] == BLACK_KING_CASTLEMOV || psbmoves[i] == BLACK_QUEEN_CASTLEMOV) {
            i--;
            continue;
        }

        if (psbmoves[i] == PROMOTE_MOVE) {
            i += 2;

            sparesq1 = board[psbmoves[i]];
            sparesq2 = board[psbmoves[i + 1]];

            board[psbmoves[i + 1]] = psbmoves[i + 2];
            board[psbmoves[i]] = EMPTY_SQUARE;

            checkresult = checkforcheck(board, turn);

            board[psbmoves[i]] = sparesq1;
            board[psbmoves[i + 1]] = sparesq2;

            if (checkresult) {
                psbmoves[i - 2] = psbmoves[i] = psbmoves[i + 1] = psbmoves[i + 2] = NO_MOVE;
            }

            continue;
        }

        if (psbmoves[i] == ENPASSANTMOV) {
            i++;

            sparesq1 = board[psbmoves[i]];
            sparesq2 = board[psbmoves[i + 1] + (getrowindex(psbmoves[i + 1]) == 6 ? - 8 : 8)];

            board[psbmoves[i + 1]] = board[psbmoves[i]];
            board[psbmoves[i + 1] + (getrowindex(psbmoves[i + 1]) == 6 ? - 8 : 8)] = EMPTY_SQUARE;
            board[psbmoves[i]] = EMPTY_SQUARE;

            checkresult = checkforcheck(board, turn);

            board[psbmoves[i]] = sparesq1;
            board[psbmoves[i + 1] + (getrowindex(psbmoves[i + 1]) == 6 ? - 8 : 8)] = sparesq2;
            board[psbmoves[i + 1]] = EMPTY_SQUARE;

            if (checkresult) {
                psbmoves[i - 1] = psbmoves[i] = psbmoves[i + 1] = NO_MOVE;
            }

            continue;
        }

        sparesq1 = board[psbmoves[i]];
        sparesq2 = board[psbmoves[i + 1]];

        board[psbmoves[i + 1]] = board[psbmoves[i]];
        board[psbmoves[i]] = EMPTY_SQUARE;

        checkresult = checkforcheck(board, turn);

        board[psbmoves[i]] = sparesq1;
        board[psbmoves[i + 1]] = sparesq2;

        if (checkresult) {
            psbmoves[i] = psbmoves[i + 1] = NO_MOVE;
        }
    }

    return;
}

int checkforcheck (int* board, int turn) {
    int kingsq, row, column, spare;
    int i = 1;

    if (turn == WHITE_TURN) {
        repeat(64) {
            if (board[i] == WHITE_KING) {
                kingsq = i;

                break;
            }

            i++;
        }
        row = getrowindex(kingsq);
        column = getcolumnindex(kingsq);

        // check for pawns
        spare = squareval(row + 1, column + 1);
        if (board[spare] == BLACK_PAWN) {
            return 1;
        }
        spare = squareval(row + 1, column - 1);
        if (board[spare] == BLACK_PAWN) {
            return 1;
        }

        // knights
        spare = squareval(row + 1, column + 2);
        if (board[spare] == BLACK_KNIGHT) {
            return 1;
        }
        spare = squareval(row + -1, column + 2);
        if (board[spare] == BLACK_KNIGHT) {
            return 1;
        }
        spare = squareval(row + 1, column - 2);
        if (board[spare] == BLACK_KNIGHT) {
            return 1;
        }
        spare = squareval(row + -1, column - 2);
        if (board[spare] == BLACK_KNIGHT) {
            return 1;
        }
        spare = squareval(row + 2, column + 1);
        if (board[spare] == BLACK_KNIGHT) {
            return 1;
        }
        spare = squareval(row + 2, column - 1);
        if (board[spare] == BLACK_KNIGHT) {
            return 1;
        }
        spare = squareval(row - 2, column + 1);
        if (board[spare] == BLACK_KNIGHT) {
            return 1;
        }
        spare = squareval(row - 2, column - 1);
        if (board[spare] == BLACK_KNIGHT) {
            return 1;
        }

        // diagonals
        i = 1;
        while (1) {
            spare = squareval(row + i, column + i);
            if (board[spare] == BLACK_BISHOP || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column + i);
            if (board[spare] == BLACK_BISHOP || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column - i);
            if (board[spare] == BLACK_BISHOP || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row + i, column - i);
            if (board[spare] == BLACK_BISHOP || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }

        /* vertical/horizontal lines */
        i = 1;
        while (1) {
            spare = squareval(row + i, column);
            if (board[spare] == BLACK_ROOK || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column);
            if (board[spare] == BLACK_ROOK || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row, column + i);
            if (board[spare] == BLACK_ROOK || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row, column - i);
            if (board[spare] == BLACK_ROOK || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
    } else {
        repeat(64) {
            if (board[i] == BLACK_KING) {
                kingsq = i;

                break;
            }

            i++;
        }
        row = getrowindex(kingsq);
        column = getcolumnindex(kingsq);

        // check for pawns
        spare = squareval(row - 1, column - 1);
        if (board[spare] == WHITE_PAWN) {
            return 1;
        }
        spare = squareval(row + 1, column - 1);
        if (board[spare] == WHITE_PAWN) {
            return 1;
        }

        // knights
        spare = squareval(row + 1, column + 2);
        if (board[spare] == WHITE_KNIGHT) {
            return 1;
        }
        spare = squareval(row + -1, column + 2);
        if (board[spare] == WHITE_KNIGHT) {
            return 1;
        }
        spare = squareval(row + 1, column - 2);
        if (board[spare] == WHITE_KNIGHT) {
            return 1;
        }
        spare = squareval(row + -1, column - 2);
        if (board[spare] == WHITE_KNIGHT) {
            return 1;
        }
        spare = squareval(row + 2, column + 1);
        if (board[spare] == WHITE_KNIGHT) {
            return 1;
        }
        spare = squareval(row + 2, column - 1);
        if (board[spare] == WHITE_KNIGHT) {
            return 1;
        }
        spare = squareval(row - 2, column + 1);
        if (board[spare] == WHITE_KNIGHT) {
            return 1;
        }
        spare = squareval(row - 2, column - 1);
        if (board[spare] == WHITE_KNIGHT) {
            return 1;
        }

        // diagonals
        i = 1;
        while (1) {
            spare = squareval(row + i, column + i);
            if (board[spare] == WHITE_BISHOP || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column + i);
            if (board[spare] == WHITE_BISHOP || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column - i);
            if (board[spare] == WHITE_BISHOP || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row + i, column - i);
            if (board[spare] == WHITE_BISHOP || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }

        /* vertical/horizontal lines */
        i = 1;
        while (1) {
            spare = squareval(row + i, column);
            if (board[spare] == WHITE_ROOK || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column);
            if (board[spare] == WHITE_ROOK || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row, column + i);
            if (board[spare] == WHITE_ROOK || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row, column - i);
            if (board[spare] == WHITE_ROOK || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[spare] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
    }

    return 0;
}

int is_overflow (int row, int column) {
    if ((row >= 1 && row <= 8) && (column >= 1 && column <= 8)) return 0;
    else return 1;
}

int squareval (int row, int column) {
    int returnval;

    if (is_overflow(row, column)) returnval = NULL_SQUARE;
    else returnval = (((row - 1) * 8) + column);

    return returnval;
}

static void add_possible_move (short* psbmoves, int* board, int* psbcounter, int sq1, int sq2row, int sq2column, int turn) {
    int sq2 = squareval(sq2row, sq2column);
    if (sq2 == NULL_SQUARE) return;
    if (!checkpiece(board[sq1], turn)) return;
    if (checkpiece(board[sq2], turn) == 1) return;

    if (turn == WHITE_TURN) {
        if (board[sq2] == BLACK_KING) return;
    } else {
        if (board[sq2] == WHITE_KING) return;
    }

    psbmoves[(*psbcounter)++] = sq1;
    psbmoves[(*psbcounter)++] = sq2;

    return;
}

static void try_moves (short* psbmoves, int* board, int* psbcounter, int turn, int square, int type) {
    int row = getrowindex(square);
    int column = getcolumnindex(square);
    int i, epsquare = board[ENPASSANTSQ];

    switch (type) {
        case WHITE_PAWN:
            if (row == 7) {
                int newsq = squareval(row + 1, column);
                if (newsq != NULL_SQUARE && board[newsq] == EMPTY_SQUARE) {
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = newsq;
                    psbmoves[(*psbcounter)++] = WHITE_KNIGHT;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = newsq;
                    psbmoves[(*psbcounter)++] = WHITE_BISHOP;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = newsq;
                    psbmoves[(*psbcounter)++] = WHITE_ROOK;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = newsq;
                    psbmoves[(*psbcounter)++] = WHITE_QUEEN;
                }
                int capsq_left = squareval(row + 1, column - 1);
                if (capsq_left != NULL_SQUARE && board[capsq_left] != EMPTY_SQUARE && checkpiece(board[capsq_left], turn) == 0) {
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_left;
                    psbmoves[(*psbcounter)++] = WHITE_KNIGHT;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_left;
                    psbmoves[(*psbcounter)++] = WHITE_BISHOP;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_left;
                    psbmoves[(*psbcounter)++] = WHITE_ROOK;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_left;
                    psbmoves[(*psbcounter)++] = WHITE_QUEEN;
                }
                int capsq_right = squareval(row + 1, column + 1);
                if (capsq_right != NULL_SQUARE && board[capsq_right] != EMPTY_SQUARE && checkpiece(board[capsq_right], turn) == 0) {
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_right;
                    psbmoves[(*psbcounter)++] = WHITE_KNIGHT;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_right;
                    psbmoves[(*psbcounter)++] = WHITE_BISHOP;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_right;
                    psbmoves[(*psbcounter)++] = WHITE_ROOK;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_right;
                    psbmoves[(*psbcounter)++] = WHITE_QUEEN;
                }
            } else {
                add_possible_move(psbmoves, board, psbcounter, square, row + 1, column, turn);

                if (board[squareval(row + 1, column)] == EMPTY_SQUARE && row == 2) {
                    add_possible_move(psbmoves, board, psbcounter, square, row + 2, column, turn);
                }

                if (board[squareval(row + 1, column + 1)] != EMPTY_SQUARE && \
                board[squareval(row + 1, column + 1)] != epsquare) {
                    add_possible_move(psbmoves, board, psbcounter, square, row + 1, column + 1, turn);
                }
                if (board[squareval(row + 1, column - 1)] != EMPTY_SQUARE && \
                board[squareval(row + 1, column - 1)] != epsquare) {
                    add_possible_move(psbmoves, board, psbcounter, square, row + 1, column - 1, turn);
                }

                if ((board[squareval(row + 1, column + 1)] == epsquare) \
                ||(board[squareval(row + 1, column - 1)] == epsquare)) {
                    psbmoves[(*psbcounter)++] = ENPASSANTMOV;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = epsquare;
                }
            }

            break;
        case BLACK_PAWN:
            if (row == 2) {
                int newsq = squareval(row - 1, column);
                if (newsq != NULL_SQUARE && board[newsq] == EMPTY_SQUARE) {
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = newsq;
                    psbmoves[(*psbcounter)++] = BLACK_KNIGHT;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = newsq;
                    psbmoves[(*psbcounter)++] = BLACK_BISHOP;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = newsq;
                    psbmoves[(*psbcounter)++] = BLACK_ROOK;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = newsq;
                    psbmoves[(*psbcounter)++] = BLACK_QUEEN;
                }
                int capsq_left = squareval(row - 1, column - 1);
                if (capsq_left != NULL_SQUARE && board[capsq_left] != EMPTY_SQUARE && checkpiece(board[capsq_left], turn) == 0) {
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_left;
                    psbmoves[(*psbcounter)++] = BLACK_KNIGHT;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_left;
                    psbmoves[(*psbcounter)++] = BLACK_BISHOP;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_left;
                    psbmoves[(*psbcounter)++] = BLACK_ROOK;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_left;
                    psbmoves[(*psbcounter)++] = BLACK_QUEEN;
                }
                int capsq_right = squareval(row - 1, column + 1);
                if (capsq_right != NULL_SQUARE && board[capsq_right] != EMPTY_SQUARE && checkpiece(board[capsq_right], turn) == 0) {
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_right;
                    psbmoves[(*psbcounter)++] = BLACK_KNIGHT;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_right;
                    psbmoves[(*psbcounter)++] = BLACK_BISHOP;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_right;
                    psbmoves[(*psbcounter)++] = BLACK_ROOK;
                    psbmoves[(*psbcounter)++] = PROMOTE_MOVE;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = capsq_right;
                    psbmoves[(*psbcounter)++] = BLACK_QUEEN;
                }
            } else {
                add_possible_move(psbmoves, board, psbcounter, square, row - 1, column, turn);

                if (board[squareval(row - 1, column)] == EMPTY_SQUARE && row == 7) {
                    add_possible_move(psbmoves, board, psbcounter, square, row - 2, column, turn);
                }

                if (board[squareval(row - 1, column + 1)] != EMPTY_SQUARE && \
                board[squareval(row - 1, column + 1)] != epsquare) {
                    add_possible_move(psbmoves, board, psbcounter, square, row - 1, column + 1, turn);
                }
                if (board[squareval(row - 1, column - 1)] != EMPTY_SQUARE && \
                board[squareval(row - 1, column - 1)] != epsquare) {
                    add_possible_move(psbmoves, board, psbcounter, square, row - 1, column - 1, turn);
                }

                if ((board[squareval(row - 1, column + 1)] == epsquare)\
                ||(board[squareval(row - 1, column - 1)] == epsquare)) {
                    psbmoves[(*psbcounter)++] = ENPASSANTMOV;
                    psbmoves[(*psbcounter)++] = square;
                    psbmoves[(*psbcounter)++] = epsquare;
                }
            }

            break;
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            add_possible_move(psbmoves, board, psbcounter, square, row + 1, column + 2, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row + 1, column - 2, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row - 1, column + 2, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row - 1, column - 2, turn);

            add_possible_move(psbmoves, board, psbcounter, square, row + 2, column + 1, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row + 2, column - 1, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row - 2, column + 1, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row - 2, column - 1, turn);

            break;
        case WHITE_BISHOP:
        case BLACK_BISHOP:
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row + i, column + i, turn);
                if (board[squareval(row + i, column + i)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row - i, column + i, turn);
                if (board[squareval(row - i, column + i)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row + i, column - i, turn);
                if (board[squareval(row + i, column - i)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row - i, column - i, turn);
                if (board[squareval(row - i, column - i)] != EMPTY_SQUARE) break;
                i++;
            }

            break;
        case WHITE_ROOK:
        case BLACK_ROOK:
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row + i, column, turn);
                if (board[squareval(row + i, column)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row - i, column, turn);
                if (board[squareval(row - i, column)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row, column + i, turn);
                if (board[squareval(row, column + i)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row, column - i, turn);
                if (board[squareval(row, column - i)] != EMPTY_SQUARE) break;
                i++;
            }

            break;
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row + i, column + i, turn);
                if (board[squareval(row + i, column + i)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row - i, column + i, turn);
                if (board[squareval(row - i, column + i)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row + i, column - i, turn);
                if (board[squareval(row + i, column - i)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row - i, column - i, turn);
                if (board[squareval(row - i, column - i)] != EMPTY_SQUARE) break;
                i++;
            }

            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row + i, column, turn);
                if (board[squareval(row + i, column)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row - i, column, turn);
                if (board[squareval(row - i, column)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row, column + i, turn);
                if (board[squareval(row, column + i)] != EMPTY_SQUARE) break;
                i++;
            }
            i = 1;
            repeat(7) {
                add_possible_move(psbmoves, board, psbcounter, square, row, column - i, turn);
                if (board[squareval(row, column - i)] != EMPTY_SQUARE) break;
                i++;
            }

            break;
        case WHITE_KING:
            add_possible_move(psbmoves, board, psbcounter, square, row, column + 1, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row + 1, column + 1, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row + 1, column, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row + 1, column - 1, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row, column - 1, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row - 1, column - 1, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row - 1, column, turn);
            add_possible_move(psbmoves, board, psbcounter, square, row - 1, column + 1, turn);

            if (!checkforcheck(board, WHITE_TURN)) {
                if (board[KINGSIDE_WHITE]) {
                    if (board[square + 1] == EMPTY_SQUARE && board[square + 2] == EMPTY_SQUARE) {
                        board[square] = EMPTY_SQUARE;

                        board[square + 1] = WHITE_KING;
                        if (checkforcheck(board, WHITE_TURN)) goto cancel_kingside_castle;
                        else {
                            board[square + 1] = EMPTY_SQUARE;
                            board[square + 2] = WHITE_KING;
                            if (checkforcheck(board, WHITE_TURN)) goto cancel_kingside_castle;
                        }

                        psbmoves[(*psbcounter)++] = WHITE_KING_CASTLEMOV;
                        psbmoves[(*psbcounter)++] = (short)(square + 2);

                        cancel_kingside_castle:
                        board[square + 1] = board[square + 2] = EMPTY_SQUARE;
                        board[square] = WHITE_KING;
                    }
                }
                if (board[QUEENSIDE_WHITE]) {
                    if (board[square - 1] == EMPTY_SQUARE && board[square - 2] == EMPTY_SQUARE \
                    && board[square - 3] == EMPTY_SQUARE) {

                        board[square] = EMPTY_SQUARE;

                        board[square - 1] = WHITE_KING;
                        if (checkforcheck(board, WHITE_TURN)) goto cancel_queenside_castle;
                        else {
                            board[square - 1] = EMPTY_SQUARE;
                            board[square - 2] = WHITE_KING;
                            if (checkforcheck(board, WHITE_TURN)) goto cancel_queenside_castle;
                        }

                        psbmoves[(*psbcounter)++] = WHITE_QUEEN_CASTLEMOV;
                        psbmoves[(*psbcounter)++] = (short)(square - 2);

                        cancel_queenside_castle:
                        board[square - 1] = board[square - 2] = EMPTY_SQUARE;
                        board[square] = WHITE_KING;
                    }
                }
            }

            break;
        case BLACK_KING:
        add_possible_move(psbmoves, board, psbcounter, square, row, column + 1, turn);
        add_possible_move(psbmoves, board, psbcounter, square, row + 1, column + 1, turn);
        add_possible_move(psbmoves, board, psbcounter, square, row + 1, column, turn);
        add_possible_move(psbmoves, board, psbcounter, square, row + 1, column - 1, turn);
        add_possible_move(psbmoves, board, psbcounter, square, row, column - 1, turn);
        add_possible_move(psbmoves, board, psbcounter, square, row - 1, column - 1, turn);
        add_possible_move(psbmoves, board, psbcounter, square, row - 1, column, turn);
        add_possible_move(psbmoves, board, psbcounter, square, row - 1, column + 1, turn);

        if (!checkforcheck(board, BLACK_TURN)) {
            if (board[KINGSIDE_BLACK]) {
                if (board[square + 1] == EMPTY_SQUARE && board[square + 2] == EMPTY_SQUARE) {
                    board[square] = EMPTY_SQUARE;

                    board[square + 1] = BLACK_KING;
                    if (checkforcheck(board, BLACK_TURN)) goto black_cancel_kingside_castle;
                    else {
                        board[square + 1] = EMPTY_SQUARE;
                        board[square + 2] = BLACK_KING;
                        if (checkforcheck(board, BLACK_TURN)) goto black_cancel_kingside_castle;
                    }

                    psbmoves[(*psbcounter)++] = BLACK_KING_CASTLEMOV;
                    psbmoves[(*psbcounter)++] = (short)(square + 2);

                    black_cancel_kingside_castle:
                    board[square + 1] = board[square + 2] = EMPTY_SQUARE;
                    board[square] = BLACK_KING;
                }
            }
            if (board[QUEENSIDE_BLACK]) {
                if (board[square - 1] == EMPTY_SQUARE && board[square - 2] == EMPTY_SQUARE \
                    && board[square - 3] == EMPTY_SQUARE) {
                    board[square] = EMPTY_SQUARE;

                    board[square - 1] = BLACK_KING;
                    if (checkforcheck(board, BLACK_TURN)) goto black_cancel_queenside_castle;
                    else {
                        board[square - 1] = EMPTY_SQUARE;
                        board[square - 2] = BLACK_KING;
                        if (checkforcheck(board, BLACK_TURN)) goto black_cancel_queenside_castle;
                    }

                    psbmoves[(*psbcounter)++] = BLACK_QUEEN_CASTLEMOV;
                    psbmoves[(*psbcounter)++] = (short)(square - 2);

                    black_cancel_queenside_castle:
                    board[square - 1] = board[square - 2] = EMPTY_SQUARE;
                    board[square] = BLACK_KING;
                }
            }
        }

        break;
    }

    return;
}

int minimax(int* board, int depth, int turn, int alpha, int beta) {
    if (depth == 0) {
        board[0] = turn;
        return evaluate_board(board);
    }

    uint64_t hash = zobrist_hash(board);
    int tt_result;
    if (tt_lookup(hash, depth, alpha, beta, &tt_result)) {
        return tt_result;
    }

    short* psbmoves = move_cache[15 - depth];
    memset(psbmoves, 0, MAX_POSSIBLE_MOVES * sizeof(short));
    calculate_moves(board, psbmoves, turn);
    sort_moves_by_priority(board, psbmoves);

    if (psbmoves[0] == END_OF_PBMOVES) {
        int result = (checkforcheck(board, turn)) ? ((turn == WHITE_TURN) ? -100000 : 100000) : 0;
        tt_store(hash, result, depth, 0);
        return result;
    }

    int next_turn = (turn == WHITE_TURN) ? BLACK_TURN : WHITE_TURN;
    int best_eval;
    int alpha_orig = alpha;

    if (turn == WHITE_TURN) {
        best_eval = -999999;
        for (int i = 0; psbmoves[i] != END_OF_PBMOVES; ) {
            if (psbmoves[i] == NO_MOVE) { i += 2; continue; }

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
                i += 3;
            } else if (psbmoves[i] == WHITE_KING_CASTLEMOV || psbmoves[i] == WHITE_QUEEN_CASTLEMOV ||
                       psbmoves[i] == BLACK_KING_CASTLEMOV || psbmoves[i] == BLACK_QUEEN_CASTLEMOV) {
                /* [CASTLE_TOKEN][king_dst] — find king src by scanning */
                int dst = psbmoves[i + 1];
                int src = dst - 2; /* kingside: dst = src+2, queenside: dst = src-2 */
                if (psbmoves[i] == WHITE_QUEEN_CASTLEMOV || psbmoves[i] == BLACK_QUEEN_CASTLEMOV)
                    src = dst + 2;
                int save_src = board[src], save_dst = board[dst];
                board[dst] = board[src];
                board[src] = EMPTY_SQUARE;
                eval = minimax(board, depth - 1, next_turn, alpha, beta);
                board[src] = save_src;
                board[dst] = save_dst;
                i += 2;
            } else {
                int src = psbmoves[i], dst = psbmoves[i + 1];
                int save_src = board[src], save_dst = board[dst];
                board[dst] = board[src];
                board[src] = EMPTY_SQUARE;
                eval = minimax(board, depth - 1, next_turn, alpha, beta);
                board[src] = save_src;
                board[dst] = save_dst;
                i += 2;
            }

            if (eval > best_eval) best_eval = eval;
            if (eval > alpha) alpha = eval;
            if (beta <= alpha) break;
        }
        int flag = (best_eval <= alpha_orig) ? 2 : (best_eval >= beta ? 1 : 0);
        tt_store(hash, best_eval, depth, flag);
        return best_eval;
    } else {
        best_eval = 999999;
        for (int i = 0; psbmoves[i] != END_OF_PBMOVES; ) {
            if (psbmoves[i] == NO_MOVE) { i += 2; continue; }

            int eval;
            if (psbmoves[i] == PROMOTE_MOVE) {
                int src = psbmoves[i + 1], dst = psbmoves[i + 2], promo = psbmoves[i + 3];
                int save_src = board[src], save_dst = board[dst];
                board[dst] = promo;
                board[src] = EMPTY_SQUARE;
                eval = minimax(board, depth - 1, next_turn, alpha, beta);
                board[src] = save_src;
                board[dst] = save_dst;
                i += 4;
            } else if (psbmoves[i] == ENPASSANTMOV) {
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
                i += 3;
            } else if (psbmoves[i] == WHITE_KING_CASTLEMOV || psbmoves[i] == WHITE_QUEEN_CASTLEMOV ||
                       psbmoves[i] == BLACK_KING_CASTLEMOV || psbmoves[i] == BLACK_QUEEN_CASTLEMOV) {
                int dst = psbmoves[i + 1];
                int src = dst - 2;
                if (psbmoves[i] == WHITE_QUEEN_CASTLEMOV || psbmoves[i] == BLACK_QUEEN_CASTLEMOV)
                    src = dst + 2;
                int save_src = board[src], save_dst = board[dst];
                board[dst] = board[src];
                board[src] = EMPTY_SQUARE;
                eval = minimax(board, depth - 1, next_turn, alpha, beta);
                board[src] = save_src;
                board[dst] = save_dst;
                i += 2;
            } else {
                int src = psbmoves[i], dst = psbmoves[i + 1];
                int save_src = board[src], save_dst = board[dst];
                board[dst] = board[src];
                board[src] = EMPTY_SQUARE;
                eval = minimax(board, depth - 1, next_turn, alpha, beta);
                board[src] = save_src;
                board[dst] = save_dst;
                i += 2;
            }

            if (eval < best_eval) best_eval = eval;
            if (eval < beta) beta = eval;
            if (beta <= alpha) break;
        }
        int flag = (best_eval <= alpha_orig) ? 2 : (best_eval >= beta ? 1 : 0);
        tt_store(hash, best_eval, depth, flag);
        return best_eval;
    }
}

void move_to_notation(int* board, short move_data[], char* notation) {
    int from_sq = move_data[0];
    int to_sq = move_data[1];
    int piece = board[from_sq];

    int from_col = ((from_sq - 1) % 8) + 1;
    int to_col = ((to_sq - 1) % 8) + 1;
    int to_row = ((to_sq - 1) / 8) + 1;

    char piece_letter = ' ';
    if (piece == WHITE_KNIGHT || piece == BLACK_KNIGHT) {
        piece_letter = 'N';
    } else if (piece == WHITE_BISHOP || piece == BLACK_BISHOP) {
        piece_letter = 'B';
    } else if (piece == WHITE_ROOK || piece == BLACK_ROOK) {
        piece_letter = 'R';
    } else if (piece == WHITE_QUEEN || piece == BLACK_QUEEN) {
        piece_letter = 'Q';
    } else if (piece == WHITE_KING || piece == BLACK_KING) {
        piece_letter = 'K';
    }

    int is_capture = (board[to_sq] != EMPTY_SQUARE) ? 1 : 0;

    if (piece_letter == ' ') {
        if (is_capture) {
            sprintf(notation, "%cx%c%d", 'a' + from_col - 1, 'a' + to_col - 1, to_row);
        } else {
            sprintf(notation, "%c%d", 'a' + to_col - 1, to_row);
        }
    } else {
        if (is_capture) {
            sprintf(notation, "%cx%c%d", piece_letter, 'a' + to_col - 1, to_row);
        } else {
            sprintf(notation, "%c%c%d", piece_letter, 'a' + to_col - 1, to_row);
        }
    }
}
