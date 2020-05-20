#include <stdio.h>
#include "2020cc.h"

int main(int argc, char **argv) {
    char *input = argv[1];
    Token *t = tokenize(input);
    Program *p = parse(t);
    gen(p);
}
