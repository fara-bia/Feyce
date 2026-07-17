/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "mutuals.h"
#include "square_rep.h"
#include "decode.h"

static void flip_board(int* board);

int decode_fen (char* fen, int* board, int* halfmove, int* fullmove) {
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

                fen_i++;
            } else {
                board[board_i] = piece_int_rep(fen[fen_i]);

                fen_i++;
                board_i++;
            }
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

    fen_i++;

    if (fen[fen_i + 1] == ' ') {
        fmove[0] = '0';
        fmove[1] = fen[fen_i];
    } else {
        fmove[0] = fen[fen_i];
        fmove[1] = fen[fen_i + 1];
    }
    hfmove[2] = '\0';
    fmove[2] = '\0';

    *halfmove = atoi(hfmove);
    *fullmove = atoi(fmove);

    flip_board(board);

    return 1;
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
    int i = 1, p1, p2, p3, p4, p5, p6, p7, p8;

    repeat(4) {
        p1 = board[i];
        p2 = board[i + 1];
        p3 = board[i + 2];
        p4 = board[i + 3];
        p5 = board[i + 4];
        p6 = board[i + 5];
        p7 = board[i + 6];
        p8 = board[i + 7];

        board[i + 7] = board[65 - i];
        board[i + 6] = board[64 - i];
        board[i + 5] = board[63 - i];
        board[i + 4] = board[62 - i];
        board[i + 3] = board[61 - i];
        board[i + 2] = board[60 - i];
        board[i + 1] = board[59 - i];
        board[i] = board[58 - i];

        board[65 - i] = p8;
        board[64 - i] = p7;
        board[63 - i] = p6;
        board[62 - i] = p5;
        board[61 - i] = p4;
        board[60 - i] = p3;
        board[59 - i] = p2;
        board[58 - i] = p1;

        i += 8;
    }

    return;
}
