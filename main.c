#include <stdio.h>
#include "token.h"

int equal_string(char *s1, char *s2) {
    do {
        if (*s1 != *s2) { return 0; }
        s1++;
        s2++;
    } while (*s1 && *s2);
    return 1;
}

void gen_asm(Token *t) {
    for (; t->tk != TK_EOF; t = t->next) {
        if (t->tk == TK_NUM) {
            printf("    mov rax, %d\n", t->val);
        } else if (t->tk == TK_RESERVED) {
            if (equal_string("+", t->str)) {
                t = t->next;
                printf("    add rax, %d\n", t->val);
            } else if (equal_string("-", t->str)) {
                t = t->next;
                printf("    sub rax, %d\n", t->val);
            }
        }
    }
}

int main(int argc, char **argv) {
    char *input = argv[1];
    Token *t = tokenize(input);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    gen_asm(t);
    printf("    ret\n");
}

