/*
    Copyright 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "mutuals.h"

int main (int argc, char* argv[]) {
    repeat(atoi(argv[1])) {
        printf("Test Message\n");
    }
    return 0;
}
