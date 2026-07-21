/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include "decode.h"
#include <stdlib.h>
#include "mutuals.h"
#include "square_rep.h"

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
    board[0] = (fen[fen_i] == 'w' ? WHITE_TURN : BLACK_TURN);

    // castling availability
    fen_i += 2;
    board[65] = board[66] = board[67] = board[68] = EMPTY_SQUARE;

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
        board[69] = EMPTY_SQUARE;

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
        case 'p':
            piece_rep = BLACK_PAWN;
            break;
        case 'n':
            piece_rep = BLACK_KNIGHT;
            break;
        case 'b':
            piece_rep = BLACK_BISHOP;
            break;
        case 'r':
            piece_rep = BLACK_ROOK;
            break;
        case 'q':
            piece_rep = BLACK_QUEEN;
            break;
        case 'k':
            piece_rep = BLACK_KING;
            break;
        case 'P':
            piece_rep = WHITE_PAWN;
            break;
        case 'N':
            piece_rep = WHITE_KNIGHT;
            break;
        case 'B':
            piece_rep = WHITE_BISHOP;
            break;
        case 'R':
            piece_rep = WHITE_ROOK;
            break;
        case 'Q':
            piece_rep = WHITE_QUEEN;
            break;
        case 'K':
            piece_rep = WHITE_KING;
            break;
        default:
            piece_rep = EMPTY_SQUARE;
            break;
    }

    return piece_rep;
}

static void flip_board (int* board) {
    int i = 1;

    repeat(4) {
        int target_row = 58 - i;

        for (int j = 0; j < 8; j++) {
            int temp = board[i + j];
            board[i + j] = board[target_row + j];
            board[target_row + j] = temp;
        }

        i += 8;
    }

    return;
}
