#include "2020cc.h"

Type *char_t = &(Type){CHAR, 1, 1};
Type *bool_t = &(Type){BOOL, 1, 1};
Type *short_t = &(Type){SHORT, 2, 2};
Type *int_t = &(Type){INT, 4, 4};
Type *long_t = &(Type){LONG, 8, 8};
Type *void_t = &(Type){VOID, 0, 0};

Type *new_type(TypeKind tk, Type *base, int align) {
    Type *t = calloc(1, sizeof(Type));
    t->tk = tk;
    t->base = base;
    t->align = align;
    return t;
}

int align_to(int n, int align) {
    return (n + align - 1) & ~(align - 1);
}

Type *pointer_to(Type *base) {
    Type *t = new_type(PTR, base, 8);
    t->size = 8;
    if (base->name != NULL) {
        t->name = base->name;
    }
    return t;
}

Type *array_of(Type *base, int len) {
    Type *t = new_type(ARRAY, base, base->align);
    t->arr_size = len;
    t->size = base->size * len;
    if (base->name != NULL) {
        t->name = base->name;
    }
    return t;
}

int is_pointer(Type *ty) {
    if (ty == NULL) { return 0; }
    return (ty->tk == PTR) || (ty->tk == ARRAY);
}

int is_integer(Type *ty) {
    if (ty == NULL) { return 0; }
    return (ty->tk == INT)
        || (ty->tk == CHAR)
        || (ty->tk == BOOL)
        || (ty->tk == SHORT)
        || (ty->tk == LONG)
        || (ty->tk == ENUM);
}

int is_scalar(Type *ty) {
    if (ty == NULL) { return 0; }
    return is_integer(ty) || ty->base != NULL;
}

Type *copy_type(Type *ty) {
    Type *ret = calloc(1, sizeof(Type));
    *ret = *ty;
    return ret;
}

Type *get_common_type(Type *ty1, Type *ty2) {
    if (ty1->base != NULL) {
        return pointer_to(ty1->base);
    }
    if (ty1->size == 8 || ty2->size == 8) {
        return long_t;
    }
    return int_t;
}

void usual_arith_conv(Node **lhs, Node **rhs) {
    Type *ty = get_common_type((*lhs)->ty, (*rhs)->ty);
    *lhs = new_cast_node(*lhs, ty);
    *rhs = new_cast_node(*rhs, ty);
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

    for (Node *n_i = n->block; n_i != NULL; n_i = n_i->next) {
        add_type(n_i);
    }

    if (n->nk == ND_CALL) {
        for (Node *n_i = n->func->body; n_i != NULL; n_i = n_i->next) {
            add_type(n_i);
        }
        if (n->func->args_num > 0) {
            for (Node *n_i = n->func->args; n_i != NULL; n_i = n_i->next) {
                add_type(n_i);
            }
        }
    }

    Node *stmt;

    switch (n->nk) {
    case ND_NUM:
        n->ty = (n->val == (int)n->val) ? int_t : long_t;
        return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
        n->ty = int_t;
        return;
    case ND_CALL:
        n->ty = long_t;
        return;
    case ND_LVAR:
    case ND_DECL:
        n->ty = n->var.type;
        return;
    case ND_COMMA:
        n->ty = n->rhs->ty;
        return;
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_REM:
    case ND_LSHIFT:
    case ND_RSHIFT:
    case ND_BITAND:
    case ND_BITOR:
    case ND_BITXOR:
        usual_arith_conv(&n->lhs, &n->rhs);
        n->ty = n->lhs->ty;
        return;
    case ND_ASSIGN:
        if (is_scalar(n->rhs->ty)) {
            n->rhs = new_cast_node(n->rhs, n->lhs->ty);
        }
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
    case ND_NOT:
    case ND_LOGAND:
    case ND_LOGOR:
        n->ty = int_t;
        return;
    case ND_BITNOT:
        n->ty = n->expr->ty;
        return;
    default:;
    }
}
