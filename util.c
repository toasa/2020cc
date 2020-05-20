#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "2020cc.h"

int equal_strings(char *s1, char *s2) {
    return (strcmp(s1, s2) == 0);
}

void assert(int result, char *fmt, ...) {
    if (!result) {
        fprintf(stderr, "assertion failed\n");
        error(fmt);
    }
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
