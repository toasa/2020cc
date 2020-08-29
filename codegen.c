#include "2020cc.h"

char *regs_64[6] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };
char *regs_32[6] = { "edi", "esi", "edx", "ecx", "r8d", "r9d" };
char *regs_16[6] = { "di", "si", "dx", "cx", "r8w", "r9w" };
char *regs_8[6] = { "dil", "sil", "dl", "cl", "r8b", "r9b" };

void gen_expr(Node *n);
void gen_stmt(Node *n);

int count() {
    static int label_count = 0;
    return label_count++;
}

void gen_addr(Node *n) {
    if (n->nk == ND_LVAR) {
        if (n->var.is_global) {
            printf("    mov rax, offset %s\n", n->var.name);
        } else {
            printf("    mov rax, rbp\n");
            printf("    sub rax, %d\n", n->var.offset);
        }
        printf("    push rax\n");
    } if (n->nk == ND_MEMBER) {
        gen_addr(n->expr);
        printf("    pop rax\n");
        printf("    mov r10, %d\n", n->member->offset);
        printf("    add rax, r10\n");
        printf("    push rax\n");
    } else if (n->nk == ND_DEREF) {
        gen_expr(n->expr);
    }
}

void load(Node *n) {
    if (n->ty->tk == ARRAY) {
        return;
    }

    printf("    pop rax\n");
    if (n->ty->size == 1) {
        printf("    movsx rax, byte ptr [rax]\n");
    } else if (n->ty->size == 2) {
        printf("    movsx rax, word ptr [rax]\n");
    } else if (n->ty->size == 4) {
        printf("    movsx rax, dword ptr [rax]\n");
    } else if (n->ty->size == 8) {
        printf("    mov rax, [rax]\n");
    } else {
        error("invalid type size");
    }

    printf("    push rax\n");
}

void cast(Type *t) {
    if (t->tk == VOID) {
        return;
    }

    printf("    pop rax\n");

    if (t->tk == BOOL) {
        printf("    cmp rax, 0\n");
        printf("    setne al\n");
        printf("    movzx rax, al\n");
        printf("    push rax\n");
        return;
    }

    if (t->size == 1) {
        printf("    movsx rax, al\n");
    } else if (t->size == 2) {
        printf("    movsx rax, ax\n");
    } else if (t->size == 4) {
        printf("    movsx rax, eax\n");
    }

    printf("    push rax\n");
}

// push the expression value onto a stack,
// so caller of this function must call `pop` instruction.
void gen_expr(Node *n) {
    if (n->nk == ND_NUM) {
        printf("    push %d\n", n->val);
        return;
    } else if (n->nk == ND_LVAR || n->nk == ND_MEMBER) {
        gen_addr(n);
        load(n);
        return;
    } else if (n->nk == ND_CALL) {
        FuncData *callee = n->func;

        // generate expression code for each argument.
        if (callee->args_num > 0) {
            Node *arg = callee->args;
            for (int count = 0; count < callee->args_num; count++) {
                gen_expr(arg);
                arg = arg->next;
            }
        }

        // generated value of argument move to specified register for function calling.
        for (int arg_i = callee->args_num; arg_i > 0; arg_i--) {
            printf("    pop rax\n");
            printf("    mov %s, rax\n", regs_64[arg_i-1]);
        }

        printf("    mov rax, 0\n");
        printf("    call %s\n", callee->name);
        printf("    push rax\n");
        return;
    } else if (n->nk == ND_DEREF) {
        gen_expr(n->expr);
        load(n);
        return;
    } else if (n->nk == ND_ADDR) {
        gen_addr(n->expr);
        return;
    } else if (n->nk == ND_NOT) {
        gen_expr(n->expr);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        printf("    push rax\n");
        return;
    } else if (n->nk == ND_BITNOT) {
        gen_expr(n->expr);
        printf("    pop rax\n");
        printf("    not rax\n");
        printf("    push rax\n");
        return;
    } else if (n->nk == ND_PREINC) {
        gen_expr(n->expr);
        gen_expr(n->inc);
        printf("    pop rax\n");
        printf("    pop r10\n");
        printf("    add r10, rax\n");

        gen_addr(n->expr);

        printf("    pop rax\n");
        printf("    mov [rax], r10\n");

        printf("    push r10\n");
        return;
    } else if (n->nk == ND_PREDEC) {
        gen_expr(n->expr);
        gen_expr(n->inc);
        printf("    pop rax\n");
        printf("    pop r10\n");
        printf("    sub r10, rax\n");

        gen_addr(n->expr);

        printf("    pop rax\n");
        printf("    mov [rax], r10\n");

        printf("    push r10\n");
        return;
    } else if (n->nk == ND_POSTINC) {
        gen_expr(n->expr);
        gen_expr(n->inc);
        printf("    pop rax\n");
        printf("    pop r10\n");
        printf("    mov r11, r10\n");
        printf("    add r10, rax\n");

        gen_addr(n->expr);

        printf("    pop rax\n");
        printf("    mov [rax], r10\n");

        printf("    push r11\n");
        return;
    } else if (n->nk == ND_POSTDEC) {
        gen_expr(n->expr);
        gen_expr(n->inc);
        printf("    pop rax\n");
        printf("    pop r10\n");
        printf("    mov r11, r10\n");
        printf("    sub r10, rax\n");

        gen_addr(n->expr);

        printf("    pop rax\n");
        printf("    mov [rax], r10\n");

        printf("    push r11\n");
        return;
    } else if (n->nk == ND_STMT_EXPR) {
        Node *stmt = n->block;
        while (stmt != NULL) {
            int is_return = (stmt->nk == ND_RETURN);
            gen_stmt(stmt);
            if (is_return) {
                break;
            }
            stmt = stmt->next;
        }
        printf("    push rax\n");
        return;
    } else if (n->nk == ND_CAST) {
        gen_expr(n->expr);
        cast(n->ty);
        return;
    }

    gen_expr(n->lhs);
    gen_expr(n->rhs);
    printf("    pop r10\n");
    printf("    pop rax\n");

    if (n->nk == ND_ADD) {
        printf("    add rax, r10\n");
    } else if (n->nk == ND_SUB) {
        printf("    sub rax, r10\n");
    } else if (n->nk == ND_MUL) {
        printf("    imul r10\n");
    } else if (n->nk == ND_DIV) {
        printf("    cqo\n");
        printf("    idiv r10\n");
    } else if (n->nk == ND_REM) {
        printf("    cqo\n");
        printf("    xor rdx, rdx\n");
        printf("    idiv r10\n");
        printf("    mov rax, rdx\n");
    } else if (n->nk == ND_LSHIFT) {
        printf("    mov rcx, r10\n");
        printf("    shl rax, cl\n");
    } else if (n->nk == ND_RSHIFT) {
        printf("    mov rcx, r10\n");
        printf("    shr rax, cl\n");
    } else if (n->nk == ND_EQ) {
        printf("    cmp rax, r10\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
    } else if (n->nk == ND_NE) {
        printf("    cmp rax, r10\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
    } else if (n->nk == ND_LT) {
        printf("    cmp rax, r10\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
    } else if (n->nk == ND_LE) {
        printf("    cmp rax, r10\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
    } else if (n->nk == ND_BITAND) {
        printf("    and rax, r10\n");
    } else if (n->nk == ND_BITOR) {
        printf("    or rax, r10\n");
    } else if (n->nk == ND_BITXOR) {
        printf("    xor rax, r10\n");
    } else if (n->nk == ND_LOGAND) {
        int c = count();
        printf("    cmp rax, 0\n");
        printf("    je .L.false.%d\n", c);
        printf("    cmp r10, 0\n");
        printf("    je .L.false.%d\n", c);
        printf("    mov rax, 1\n");
        printf("    jmp .L.end.%d\n", c);
        printf(".L.false.%d:\n", c);
        printf("    mov rax, 0\n");
        printf(".L.end.%d:\n", c);
    } else if (n->nk == ND_LOGOR) {
        int c = count();
        printf("    cmp rax, 0\n");
        printf("    jne .L.true.%d\n", c);
        printf("    cmp r10, 0\n");
        printf("    jne .L.true.%d\n", c);
        printf("    mov rax, 0\n");
        printf("    jmp .L.end.%d\n", c);
        printf(".L.true.%d:\n", c);
        printf("    mov rax, 1\n");
        printf(".L.end.%d:\n", c);
    }

    printf("    push rax\n");
}

// the function name which is generating assemblly currently.
char cur_func[100];

void gen_stmt(Node *n) {
    if (n->nk == ND_RETURN) {
        gen_expr(n->expr);
        printf("    pop rax\n");
        printf("    jmp .Lreturn_%s\n", cur_func);
    } else if (n->nk == ND_IF) {
        int c = count();
        gen_expr(n->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");

        if (n->alt == NULL) {
            // Not exists 'else'.
            printf("    je  .L.end.%d\n", c);
            gen_stmt(n->then);
            printf(".L.end.%d:\n", c);
        } else {
            // exists 'else'.
            printf("    je  .L.else.%d\n", c);
            gen_stmt(n->then);
            printf("    jmp .L.end.%d\n", c);
            printf(".L.else.%d:\n", c);
            gen_stmt(n->alt);
            printf(".L.end.%d:\n", c);
        }
    } else if (n->nk == ND_WHILE) {
        int c = count();
        printf(".L.start.%d:\n", c);
        gen_expr(n->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .L.end.%d\n", c);
        gen_stmt(n->then);
        printf("    jmp .L.start.%d\n", c);
        printf(".L.end.%d:\n", c);
    } else if (n->nk == ND_FOR) {
        int c = count();
        if (n->init != NULL) {
            gen_stmt(n->init);
        }
        printf(".L.start.%d:\n", c);
        if (n->cond != NULL) {
            gen_expr(n->cond);
            printf("    pop rax\n");
        } else {
            // If a condition does not exist, to will be a infinite loop.
            printf("    mov rax, 1\n");
        }
        printf("    cmp rax, 0\n");
        printf("    je  .L.end.%d\n", c);
        gen_stmt(n->then);
        if (n->post != NULL) {
            gen_stmt(n->post);
        }
        printf("    jmp .L.start.%d\n", c);
        printf(".L.end.%d:\n", c);
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
        gen_addr(n->lhs);
        gen_expr(n->rhs);

        printf("    pop r10\n");
        printf("    pop rax\n");
        if (n->ty->size == 1) {
            printf("    mov byte ptr [rax], r10b\n");
        } else if (n->ty->size == 2) {
            printf("    mov word ptr [rax], r10w\n");
        } else if (n->ty->size == 4) {
            printf("    mov dword ptr [rax], r10d\n");
        } else if (n->ty->size == 8) {
            printf("    mov [rax], r10\n");
        } else {
            error("invalid type size");
        }

        printf("    mov rax, [rax]\n");

    } else if (n->nk == ND_DECL) {
        // need to do something?
    } else {
        gen_expr(n);
        printf("    pop rax\n");
    }
}


void gen_func(FuncData *func) {
    // function declaration only. (Not a function definition)
    if (func->body == NULL) {
        return;
    }

    strcpy(cur_func, func->name);
    if (!func->is_static) {
        printf(".global %s\n", func->name);
    }
    printf("%s:\n", func->name);

    // prologue
    printf("    push rbp\n");
    printf("    push r10\n");
    printf("    push r11\n");
    printf("    push r12\n");
    printf("    push r13\n");
    printf("    push r14\n");
    printf("    push r15\n");
    printf("    mov rbp, rsp\n");

    // allocate for local variables in stack area.
    printf("    sub rsp, %zu\n", func->stack_frame_size);

    // arguments
    if (func->args_num > 0) {
        // skip a linked list of local variables to find a function argument.
        VarNode *arg = func->toplevel_scope->lvar_head;
        while (arg->data.vk != ARG) {
            arg = arg->next;
        }

        for (int i = func->args_num - 1; i >= 0; i--) {
            if (arg->data.vk != ARG) {
                error("argument preparing error");
            }

            // store values which is specified register to
            // stack area as local variables.
            // TODO? extract 'store' function.
            printf("    mov rax, rbp\n");
            printf("    sub rax, %d\n", arg->data.offset);
            int arg_size = arg->data.type->size;
            if (arg_size == 1) {
                printf("    mov [rax], %s\n", regs_8[i]);
            } else if (arg_size == 2) {
                printf("    mov [rax], %s\n", regs_16[i]);
            } else if (arg_size == 4) {
                printf("    mov [rax], %s\n", regs_32[i]);
            } else if (arg_size == 8) {
                printf("    mov [rax], %s\n", regs_64[i]);
            } else {
                error("invalid size of argument");
            }
            arg = arg->next;
        }
    }

    // function body
    gen_stmt(func->body);

    // epilogue
    printf(".Lreturn_%s:\n", func->name);
    printf("    mov rsp, rbp\n");
    printf("    pop r15\n");
    printf("    pop r14\n");
    printf("    pop r13\n");
    printf("    pop r12\n");
    printf("    pop r11\n");
    printf("    pop r10\n");
    printf("    pop rbp\n");
    printf("    ret\n\n");
}

void gen_data(Program *p) {
    printf(".data\n");

    for (VarNode *var = p->gvars; var != NULL; var = var->next) {
        printf("%s:\n", var->data.name);

        if (var->data.str == NULL) {
            printf("  .zero %zu\n\n", var->data.type->size);
            continue;
        }

        // string literal
        for (int i = 0; i < var->data.str_len; i++) {
            printf("    .byte %d\n", var->data.str[i]);
        }
    }
}

void gen_text(Program *p) {
    printf(".text\n");

    for (int i = 0; p->funcs[i] != NULL; i++) {
        gen_func(p->funcs[i]);
    }
}

void gen(Program *p) {
    printf(".intel_syntax noprefix\n");

    gen_data(p);
    gen_text(p);
}
