#include <stdio.h>
#include "token.h"
#include "parse.h"

void gen_lval(Node *n) {
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", n->offset);
    printf("    push rax\n");
}

void gen_expr(Node *n) {
    if (n->nk == ND_NUM) {
        printf("    push %d\n", n->val);
        return;
    } else if (n->nk == ND_LVAR) {
        gen_lval(n);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    } else if (n->nk == ND_ASSIGN) {
        gen_lval(n->lhs);
        gen_expr(n->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    }

    gen_expr(n->lhs);
    gen_expr(n->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");

    if (n->nk == ND_ADD) {
        printf("    add rax, rdi\n");
    } else if (n->nk == ND_SUB) {
        printf("    sub rax, rdi\n");
    } else if (n->nk == ND_MUL) {
        printf("    imul rdi\n");
    } else if (n->nk == ND_DIV) {
        printf("    cqo\n");
        printf("    idiv rdi\n");
    } else if (n->nk == ND_EQ) {
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
    } else if (n->nk == ND_NE) {
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
    } else if (n->nk == ND_LT) {
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
    } else if (n->nk == ND_LE) {
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
    }

    printf("    push rax\n");
}

int label_count = 0;

void gen_stmt(Node *n) {
    if (n->nk == ND_RETURN) {
        gen_expr(n->lhs);
    } else if (n->nk == ND_IF) {
        gen_expr(n->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%03d\n", label_count);
        gen_stmt(n->then);
        printf(".Lend%03d:\n", label_count);
        label_count++;
    } else {
        gen_expr(n);
    }
}

void gen(Node **code) {
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // prologue
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");

    int i = 0;
    while (code[i] != NULL) {
        Node *n = code[i];
        int is_return = (n->nk == ND_RETURN);
        gen_stmt(n);
        i++;
        printf("    pop rax\n");
        if (is_return) {
            break;
        }
    }

    // epilogue
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
}

int main(int argc, char **argv) {
    char *input = argv[1];
    Token *t = tokenize(input);
    Node **code = parse(t);
    gen(code);
}

