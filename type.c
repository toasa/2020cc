#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "parse.h"

Type *int_t = &(Type){INT, 8};

Type *new_type(TypeKind tk, Type *ptr_to) {
    Type *t = calloc(1, sizeof(Type));
    t->tk = tk;
    t->ptr_to = ptr_to;
    return t;
}

Type *pointer_to(Type *base) {
    Type *t = new_type(PTR, base);
    t->size = 8;
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
    add_type(n->inc);

    for (Node *n_i = n->block; n_i != NULL; n_i = n_i->next){
        add_type(n_i);
    }

    if (n->nk == ND_FUNC || n->nk == ND_CALL) {
        for (Node *n_i = n->func.body; n_i != NULL; n_i = n_i->next){
            add_type(n_i);
        }
        if (n->func.args_num > 0) {
            for (Node *n_i = n->func.args; n_i != NULL; n_i = n_i->next){
                add_type(n_i);
            }
        }
    }

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
        n->ty = n->ident.type;
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
    case ND_ADDR:
        if (n->expr->ty->tk == ARRAY) {
            n->ty = pointer_to(n->expr->ty->arr_of);
        } else {
            n->ty = pointer_to(n->expr->ty);
        }
        return;
    case ND_DEREF:
        if (n->expr->ty->tk == ARRAY) {
            n->ty = n->expr->ty->arr_of;
        } else if (n->expr->ty->tk == PTR) {
            n->ty = n->expr->ty->ptr_to;
        }
        return;
    default:;
    }
}
