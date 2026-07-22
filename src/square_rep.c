/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#include "square_rep.h"
#include "mutuals.h"

int square_to_int (char* square) {
    int index;

    index = (((square[1] - '0') - 1) * 8) + ((square[0] - 'a') + 1);

    return index;
}
