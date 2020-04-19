#include <stdio.h>
#include "token.h"
#include "parse.h"

void gen_asm(Node *n) {
    if (n->nk == ND_NUM) {
        printf("    push %d\n", n->val);
    } else if (n->nk == ND_ADD) {
        gen_asm(n->lhs);
        gen_asm(n->rhs);
        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    add rax, rdi\n");
        printf("    push rax\n");
    } else if (n->nk == ND_SUB) {
        gen_asm(n->lhs);
        gen_asm(n->rhs);
        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    sub rax, rdi\n");
        printf("    push rax\n");
    } else if (n->nk == ND_MUL) {
        gen_asm(n->lhs);
        gen_asm(n->rhs);
        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mul rdi\n");
        printf("    push rax\n");
    } else if (n->nk == ND_DIV) {
        gen_asm(n->lhs);
        gen_asm(n->rhs);
        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    cqo\n");
        printf("    div rdi\n");
        printf("    push rax\n");
    }
}

void gen(Node *root) {
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    gen_asm(root);
    printf("    pop rax\n");
    printf("    ret\n");
}

int main(int argc, char **argv) {
    char *input = argv[1];
    Token *t = tokenize(input);
    Node *root = parse(t);
    gen(root);
}

