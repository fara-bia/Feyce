/*
    Copyright (c) 2026 fara-bia
    SPDX-License-Identifier: Apache-2.0
 */

#ifndef MUTUALS
#define MUTUALS

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define repeat(n) for (int CONCAT(_i, __LINE__) = 0; CONCAT(_i, __LINE__) < (n); CONCAT(_i, __LINE__)++)

#endif
