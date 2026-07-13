/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

// development of this source file and its header is delayed
#include <stdio.h>
#include <stdlib.h>
#include "mutuals.h"

int main (int argc, char* argv[]) {
    repeat(atoi(argv[1])) {
        printf("Test Message\n");
    }
    return 0;
}
