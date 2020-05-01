#include <stdio.h>
#include <string.h>
#include "token.h"
#include "parse.h"
#include "util.h"

void gen_lval(Node *n) {
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", n->offset);
    printf("    push rax\n");
}

char *regs[6] = {
    "rdi", "rsi", "rdx",
    "rcx", "r8", "r9"
};

// push the expression value onto a stack,
// so caller of this function must call `pop` instruction.
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
    } else if (n->nk == ND_CALL) {
        if (n->args_num > 0) {
            Node *arg = n->next;
            for (int count = 0; count < n->args_num; count++) {
                gen_expr(arg);
                printf("    pop rax\n");
                printf("    mov %s, rax\n", regs[count]);
                arg = arg->next;
            }
        }

        printf("    mov rax, 0\n");
        printf("    call %s\n", n->name);
        printf("    push rax\n");
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
char cur_func[100];

void gen_stmt(Node *n) {
    if (n->nk == ND_RETURN) {
        gen_expr(n->expr);
        printf("    pop rax\n");
        // TODO: switch return label for each function.
        printf("    jmp .Lreturn_%s\n", cur_func);
    } else if (n->nk == ND_IF) {
        gen_expr(n->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");

        if (n->alt == NULL) {
            printf("    je  .Lend%03d\n", label_count);
            gen_stmt(n->then);
            printf(".Lend%03d:\n", label_count);
        } else {
            printf("    je  .Lelse%03d\n", label_count);
            gen_stmt(n->then);
            printf("    jmp .Lend%03d\n", label_count);
            printf(".Lelse%03d:\n", label_count);
            gen_stmt(n->alt);
            printf(".Lend%03d:\n", label_count);
        }

        label_count++;
    } else if (n->nk == ND_WHILE) {
        printf(".Lstart%03d:\n", label_count);
        gen_expr(n->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%03d\n", label_count);
        gen_stmt(n->then);
        printf("    jmp .Lstart%03d\n", label_count);
        printf(".Lend%03d:\n", label_count);

        label_count++;
    } else if (n->nk == ND_FOR) {
        if (n->expr != NULL) {
            gen_stmt(n->expr);
        }
        printf(".Lstart%03d:\n", label_count);
        if (n->cond != NULL) {
            gen_expr(n->cond);
            printf("    pop rax\n");
        } else {
            // if a condition does not exist, to will be a infinite loop.
            printf("    mov rax, 1\n");
        }
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%03d\n", label_count);
        gen_stmt(n->then);
        if (n->post != NULL) {
            gen_stmt(n->post);
        }
        printf("    jmp .Lstart%03d\n", label_count);
        printf(".Lend%03d:\n", label_count);

        label_count++;
    } else if (n->nk == ND_BLOCK) {
        Node *stmt = n->block;
        while (stmt != NULL) {
            int is_return = (stmt->nk == ND_RETURN);
            gen_stmt(stmt);
            if (is_return) {
                break;
            }
            stmt = stmt->next;
        }
    } else if (n->nk == ND_ASSIGN) {
        gen_lval(n->lhs);
        gen_expr(n->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
    } else {
        gen_expr(n);
        printf("    pop rax\n");
    }
}


void gen_func(Node *n) {
    assert(n->nk == ND_FUNC);
    strcpy(cur_func, n->name);
    printf(".global %s\n", n->name);
    printf("%s:\n", n->name);

    // prologue
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", n->ident_num * 8);

    gen_stmt(n->body);

    // epilogue
    printf(".Lreturn_%s:\n", n->name);
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
}

void gen(Node **funcs) {
    printf(".intel_syntax noprefix\n");

    for (int i = 0; funcs[i] != NULL; i++) {
        gen_func(funcs[i]);
    }

}

int main(int argc, char **argv) {
    char *input = argv[1];
    Token *t = tokenize(input);
    Node **code = parse(t);
    gen(code);
}

