#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

int equal_strings(char *s1, char *s2) {
    return (strcmp(s1, s2) == 0);
}

void assert(int result) {
    if (!result) {
        printf("assertion failed");
        exit(1);
    }
}
