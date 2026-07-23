/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include "calc_movs.h"
#include "evaluate.h"
#include "mutuals.h"

static void add_possible_move(int* psbmoves, int* board, int* psbcounter, int sq1, int sq2row, int sq2column, int turn);

void calculate_moves (int* board, int* psbmoves, int turn) {
    int i = 1, psbcounter = 0;
    repeat(64) {
        // to be continued
        /*
         * Make another function that determines possible moves of a piece, inputting;
         * piece type, square that its on
         * use add_possible_function to use it
         * and then try to implement each move separately
         * if there's still a check after the move, eliminate that possible move from the list
         * if there's no possible move; it's checkmate, terminate
         */
    }
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
        spare = squareval(row - 1, column + 1);
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
        spare = squareval(row + 1, column + 1);
        if (board[spare] == WHITE_PAWN) {
            return 1;
        }
        spare = squareval(row - 1, column + 1);
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

static void add_possible_move (int* psbmoves, int* board, int* psbcounter, int sq1, int sq2row, int sq2column, int turn) {
    int sq2 = squareval(sq2row, sq2column);
    if (sq2 == NULL_SQUARE) return; // do not move outside board
    if (!checkpiece(sq1, turn)) return; // do not move enemy pieces
    if (checkpiece(sq2, turn)) return; // do not eat your own pieces

    if (turn == WHITE_TURN) { // do not eat enemy king
        if (board[sq2] == BLACK_KING) return;
    } else {
        if (board[sq2] == WHITE_KING) return;
    }

    psbmoves[*psbcounter++] = sq1;
    psbmoves[*psbcounter++] = sq2;

    return;
}
