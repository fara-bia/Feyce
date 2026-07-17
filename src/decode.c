/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include "mutuals.h"
#include "square_rep.h"
#include "decode.h"

static void flip_board(int* board);

void decode_fen (char* fen, int* board, int* halfmove, int* fullmove) {
    int fen_i = 0;
    int board_i = 1;

    // piece placement
    repeat(8) {
        while ((fen[fen_i] != '/') && (fen[fen_i] != ' ')) {
            if ((fen[fen_i] >= '0') && (fen[fen_i] <= '9')) {
                repeat(fen[fen_i] - '0') {
                    board[board_i] = piece_int_rep(' ');
                    board_i++;
                }
                board_i -= 1;
            } else {
                board[board_i] = piece_int_rep(fen[fen_i]);
            }

            fen_i++;
            board_i++;
        }

        if (fen[fen_i] == '/') fen_i++;
    }
    // active color
    fen_i++;
    board[0] = (fen[fen_i] == 'w' ? 1 : 0);

    // castling availability
    fen_i += 2;
    board[65] = board[66] = board[67] = board[68] = 0;

    if (fen[fen_i] == '-') {
        fen_i++;
        goto avail_end;
    }

    while (1) {
        if (fen[fen_i] == ' ')
            goto avail_end;

        switch(fen[fen_i]) {
            case 'K':
                board[65] = 1;
                break;
            case 'Q':
                board[66] = 1;
                break;
            case 'k':
                board[67] = 1;
                break;
            case 'q':
                board[68] = 1;
                break;
        }
        fen_i++;
    }
    avail_end:

    // en passant target square
    fen_i++;
    if (fen[fen_i] == '-') {
        board[69] = 0;

        fen_i++;
    } else {
        char piece[3];

        piece[0] = fen[fen_i];
        piece[1] = fen[fen_i + 1];
        piece[2] = '\0';

        board[69] = square_to_int(piece);

        fen_i+=2;
    }

    flip_board(board);

    // halfmove and fullmove
    fen_i++;
    char hfmove[3];
    char fmove[3];

    if (fen[fen_i + 1] == ' ') {
        hfmove[0] = '0';
        hfmove[1] = fen[fen_i];

        fen_i++;
    } else {
        hfmove[0] = fen[fen_i];
        hfmove[1] = fen[fen_i + 1];

        fen_i += 2;
    }
    //// I'll continue later
}

int piece_int_rep (char piece) {
    int piece_rep;

    switch (piece) {
        case 'p': // Black Pawn
            piece_rep = 1;
            break;
        case 'n': // Black Knight
            piece_rep = 2;
            break;
        case 'b': // Black Bishop
            piece_rep = 3;
            break;
        case 'r': // Black Rook
            piece_rep = 4;
            break;
        case 'q': // Black Queen
            piece_rep = 5;
            break;
        case 'k': // Black King
            piece_rep = 6;
            break;
        case 'P': // White Pawn
            piece_rep = 7;
            break;
        case 'N': // White Knight
            piece_rep = 8;
            break;
        case 'B': // White Bishop
            piece_rep = 9;
            break;
        case 'R': // White Rook
            piece_rep = 10;
            break;
        case 'Q': // White Queen
            piece_rep = 11;
            break;
        case 'K': // White King
            piece_rep = 12;
            break;
        default:
            piece_rep = 0;
            break;
    }

    return piece_rep;
}

static void flip_board (int* board) {
    int i = 1, piece1, piece2;

    repeat(32) {
        piece1 = board[i];
        piece2 = board[65 - i];

        board[i] = piece2;
        board[65 - i] = piece1;

        i++;
    }
}
