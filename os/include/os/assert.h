#ifndef OS_ASSERT_H
#define OS_ASSERT_H
#include "os.h"
void assertion_failure(char *exp, char *file, char *base, int line);

#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

#endif