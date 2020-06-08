#include <stdio.h>
#include <stdlib.h>
#include "2020cc.h"

Type *int_t = &(Type){INT, 4, 4};
Type *char_t = &(Type){CHAR, 1, 1};

Type *new_type(TypeKind tk, Type *base, int align) {
    Type *t = calloc(1, sizeof(Type));
    t->tk = tk;
    t->base = base;
    t->align = align;
    return t;
}

// to calculate `sizeof` operator.
size_t size_of(Type *t) {
    if (t->tk == ARRAY) {
        return t->arr_size * size_of(t->base);
    } else if (t->tk == PTR) {
        return 8;
    } else if (t->tk == CHAR) {
        return 1;
    } else if (t->tk == STRUCT) {
        size_t size = 0;
        for (Member *m = t->member; m != NULL; m = m->next) {
            size += size_of(m->type);
        }
        return size;
    } else if (t->tk == UNION) {
        size_t size = 0;
        for (Member *m = t->member; m != NULL; m = m->next) {
            if (size < size_of(m->type)) {
                size = size_of(m->type);
            }
        }
        return size;
    }
    // INT
    return 4;
}

int align_of(int n, int align) {
    return (n + align - 1) & ~(align - 1);
}

Type *pointer_to(Type *base) {
    Type *t = new_type(PTR, base, 8);
    t->size = 8;
    return t;
}

Type *array_of(Type *base, int len) {
    Type *t = new_type(ARRAY, base, base->align);
    t->arr_size = len;
    t->size = base->size * len;
    return t;
}

void add_type(Node *n) {
    if (n == NULL || n->ty != NULL) {
        return;
    }

    add_type(n->lhs);
    add_type(n->rhs);
    add_type(n->init);
    add_type(n->cond);
    add_type(n->then);
    add_type(n->alt);
    add_type(n->expr);
    add_type(n->post);
    add_type(n->block);
    add_type(n->inc);

    for (Node *n_i = n->block; n_i != NULL; n_i = n_i->next) {
        add_type(n_i);
    }

    if (n->nk == ND_FUNC || n->nk == ND_CALL) {
        for (Node *n_i = n->func.body; n_i != NULL; n_i = n_i->next) {
            add_type(n_i);
        }
        if (n->func.args_num > 0) {
            for (Node *n_i = n->func.args; n_i != NULL; n_i = n_i->next) {
                add_type(n_i);
            }
        }
    }

    Node *stmt;

    switch (n->nk) {
    case ND_NUM:
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_CALL:
        n->ty = int_t;
        return;
    case ND_LVAR:
    case ND_DECL:
        n->ty = n->var.type;
        return;
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_REM:
    case ND_LSHIFT:
    case ND_RSHIFT:
    case ND_ASSIGN:
        n->ty = n->lhs->ty;
        return;
    case ND_MEMBER:
        n->ty = n->member->type;
        return;
    case ND_STMT_EXPR:
        stmt = n->block;
        while (stmt->next != NULL) {
            stmt = stmt->next;
        }
        n->ty = stmt->ty;
        return;
    case ND_ADDR:
        if (n->expr->ty->tk == ARRAY) {
            n->ty = pointer_to(n->expr->ty->base);
        } else {
            n->ty = pointer_to(n->expr->ty);
        }
        return;
    case ND_DEREF:
        n->ty = n->expr->ty->base;
        return;
    default:;
    }
}
