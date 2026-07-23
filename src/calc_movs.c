/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include "calc_movs.h"
#include "evaluate.h"
#include "mutuals.h"
#include <stdlib.h>

static int squareval(int row, int column);

void calculate_moves (int* board, int* psbmoves, int turn) {

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
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column + i);
            if (board[spare] == BLACK_BISHOP || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column - i);
            if (board[spare] == BLACK_BISHOP || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row + i, column - i);
            if (board[spare] == BLACK_BISHOP || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
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
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column);
            if (board[spare] == BLACK_ROOK || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row, column + i);
            if (board[spare] == BLACK_ROOK || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row, column - i);
            if (board[spare] == BLACK_ROOK || board[spare] == BLACK_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
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
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column + i);
            if (board[spare] == WHITE_BISHOP || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column - i);
            if (board[spare] == WHITE_BISHOP || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row + i, column - i);
            if (board[spare] == WHITE_BISHOP || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
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
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row - i, column);
            if (board[spare] == WHITE_ROOK || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row, column + i);
            if (board[spare] == WHITE_ROOK || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
        i = 1;
        while (1) {
            spare = squareval(row, column - i);
            if (board[spare] == WHITE_ROOK || board[spare] == WHITE_QUEEN) {
                return 1;
            } else if (board[i] != EMPTY_SQUARE) {
                break;
            }

            i++;
        }
    }

    return 0;
}

int checkforcheckmate (int* board) {

}

static int squareval (int row, int column) {
    int returnval = (((row - 1) * 8) + column);
    if (!(returnval >= 1 && returnval <= 64)) returnval = NULL_SQUARE;
    return returnval;
}
