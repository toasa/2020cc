#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "2020cc.h"

Node *new_node(NodeKind nk, int val) {
    Node *n = calloc(1, sizeof(Node));
    n->nk = nk;
    n->val = val;
    return n;
}

Node *new_node_with_lr(NodeKind nk, Node *lhs, Node *rhs) {
    Node *n = new_node(nk, 0);
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

Node *new_struct_member_node(Node *stru, Member *mem, char *mem_name) {
    Node *mem_node = new_node(ND_MEMBER, 0);
    mem_node->member = mem;
    mem_node->expr = stru;
    return mem_node;
}

// 現在検査中のトークン
Token *token;

int cur_token_is(char *s) {
    return equal_strings(s, token->str);
}

int cur_tokenkind_is(TokenKind tk) {
    return (token->tk == tk);
}

int next_tokenkind_is(TokenKind tk) {
    if (token->tk == TK_EOF) {
        return 0;
    }
    return (token->next->tk == tk);
}

void next_token() {
    token = token->next;
}

void expect(TokenKind tk) {
    if (token->tk != tk) {
        error("expect %d token, but got %d", tk, token->tk);
    }
    next_token();
}

Node *parse_expr();

int is_pointer(Node *n) {
    if (n->ty == NULL) { return 0; }
    return (n->ty->tk == PTR) || (n->ty->tk == ARRAY);
}

int is_integer(Node *n) {
    if (n->ty == NULL) { return 0; }
    return (n->ty->tk == INT) || (n->ty->tk == CHAR);
}

FuncData new_func_data() {
    FuncData data;
    data.name = NULL;
    data.body = NULL;
    data.args_num = 0;
    data.args = NULL;
    data.toplevel_scope = NULL;
    data.stack_frame_size = 0;
    return data;
}

Var new_var(VarKind vk, char *name, Type *t) {
    Var v;
    v.vk = vk;
    v.name = name;
    v.type = t;
    v.offset = 0;
    v.str = NULL;
    v.str_len = 0;
    if (vk == GLOBAL) {
        v.is_global = 1;
    } else {
        v.is_global = 0;
    }
    return v;
}

VarNode *new_var_node(Var v) {
    VarNode *var_node = calloc(1, sizeof(VarNode));
    var_node->data = v;
    var_node->next = NULL;
    return var_node;
}

Scope *new_scope(int depth) {
    Scope *s = calloc(1, sizeof(Scope));
    s->depth = depth;
    return s;
}

// a head of linked list which store global variables.
VarNode *gvar_head = NULL;

size_t scope_depth = 0;
Scope *toplevel_scope = NULL;
Scope *cur_scope = NULL;
size_t stack_frame_size = 0;

void enter_scope() {
    Scope *s = new_scope(scope_depth);

    if (scope_depth == 0) {
        toplevel_scope = s;
    }

    s->high = cur_scope;
    cur_scope = s;
    scope_depth++;
}

void leave_scope() {
    cur_scope = cur_scope->high;
    scope_depth--;
}

// add one `VarNode` at tail of linked list which stored idefifier in current scope.
void register_new_lvar(Var v) {
    VarNode *lvar_head = cur_scope->lvar_head;
    VarNode *new_v = new_var_node(v);

    if (lvar_head == NULL) {
        stack_frame_size += v.type->size;
        new_v->data.offset = stack_frame_size;
        cur_scope->total_var_size = v.type->size;
        cur_scope->lvar_head = new_v;
        return;
    }

    // jump to tail of linked list.
    VarNode *var_iter = lvar_head;
    while (var_iter->next != NULL) {
        var_iter = var_iter->next;
    }

    cur_scope->total_var_size += v.type->size;
    stack_frame_size += v.type->size;
    new_v->data.offset = stack_frame_size;
    var_iter->next = new_v;
}

void register_new_gvar(Var v) {
    VarNode *new_v = new_var_node(v);

    if (gvar_head == NULL) {
        new_v->data.offset = v.type->size;
        gvar_head = new_v;
        return;
    }

    VarNode *var_iter = gvar_head;
    while (var_iter->next != NULL) {
        char *cur_name = var_iter->data.name;
        // check duplication of variable declaration.
        if (equal_strings(cur_name, v.name)) {
            error("duplicate of variable declaration.");
        }

        var_iter = var_iter->next;
    }

    var_iter->next = new_v;
}

// `head` is `lvar_head` or `gvar_head`.
VarNode *look_up_var(char *name, VarNode *head) {
    VarNode *var_iter = head;
    while (var_iter != NULL) {
        char *cur_name = var_iter->data.name;
        if (equal_strings(cur_name, name)) {
            return var_iter;
        }
        var_iter = var_iter->next;
    }

    return NULL;
}

// Search the 'name' from the linked list of variable.
// First, look up the local variables from current level scope to highest.
// The next, look up the global variables.
// If not found then occur compile error and abort.
Var get_var(char *name) {
    VarNode *v;

    Scope *tmp_scope = cur_scope;
    while (tmp_scope != NULL) {
        v = look_up_var(name, tmp_scope->lvar_head);
        if (v != NULL) {
            return v->data;
        }
        tmp_scope = tmp_scope->high;
    }

    v = look_up_var(name, gvar_head);
    if (v == NULL) {
        error("%s: undeclared variable.", name);
    }
    return v->data;
}

Tag *new_tag(char *tag_name, Type *type) {
    assert(type->tk == STRUCT, "tag is used for struct only");
    Tag *t = calloc(1, sizeof(Tag));
    t->name = tag_name;
    t->type = type;
    return t;
}

void register_new_tag(Tag *tag) {
    if (cur_scope->tag_head == NULL) {
        cur_scope->tag_head = tag;
        return;
    }

    Tag *t_iter = cur_scope->tag_head;
    while (t_iter->next != NULL) {
        t_iter = t_iter->next;
    }

    t_iter->next = tag;
}

Tag *get_tag(char *tag_name) {
    Tag *t_iter = cur_scope->tag_head;
    while (t_iter != NULL) {
        if (equal_strings(t_iter->name, tag_name)) {
            return t_iter;
        }
        t_iter = t_iter->next;
    }
    return NULL;
}

// return value of `parse_exprs`.
typedef struct Exprs {
    int count;
    Node *head;
} Exprs;

Type *parse_type();
Exprs parse_exprs(char *terminator);
Node *parse_primary();
Node *parse_unary();
Node *parse_mul();
Node *parse_add();
Node *parse_shift();
Node *parse_relational();
Node *parse_equality();
Node *parse_assign();
Node *parse_expr();
Node *parse_declaration(VarKind vk);
Node *parse_compound_stmt(Node*, int);
Node *parse_stmt();
Node *parse_toplevel_func(Type *ret_t, char *func_name);
void parse_toplevel_global_var(Type *t, char *gname);
void parse_toplevel();
void parse_program();

Member *new_member(char *name, Type *t) {
    Member *m = calloc(1, sizeof(Member));
    m->name = name;
    m->type = t;
    return m;
}

Member *get_member(Member *m, char *name) {
    Member *m_iter = m;
    while (m_iter != NULL) {
        if (equal_strings(m_iter->name, name)) {
            return m_iter;
        }
        m_iter = m_iter->next;
    }
    return NULL;
}

Type *parse_struct_decl(Type *t) {
    Member head;
    Member *cur = calloc(1, sizeof(Member));
    head.next = cur;
    int total_size = 0;

    while (!cur_tokenkind_is(TK_RBRACE)) {
        Type *member_t = parse_type();

        // parse some members which have same type `member_t`.
        do {
            if (cur_token_is(",")) {
                next_token();
            }

            char *member_name;
            if (member_t->tk == ARRAY) {
                member_name = member_t->arr_name;
            } else {
                member_name = token->str;
                next_token();
            }
            Member *tmp = new_member(member_name, member_t);
            total_size = align_of(total_size, member_t->align);
            tmp->offset = total_size;
            total_size += member_t->size;

            if (t->align < member_t->align) {
                t->align = member_t->align;
            }
            cur->next = tmp;
            cur = tmp;

        } while(!cur_token_is(";"));

        expect(TK_SEMICOLON);
    }
    t->member = head.next->next;
    t->size = total_size;

    return t;
}

Type *parse_union_decl(Type *t) {
    Member head;
    Member *cur = calloc(1, sizeof(Member));
    head.next = cur;

    while (!cur_tokenkind_is(TK_RBRACE)) {
        Type *member_t = parse_type();

        // parse some members which have same type `member_t`.
        do {
            if (cur_token_is(",")) {
                next_token();
            }

            char *member_name;
            if (member_t->tk == ARRAY) {
                member_name = member_t->arr_name;
            } else {
                member_name = token->str;
                next_token();
            }
            Member *tmp = new_member(member_name, member_t);
            if (t->align < member_t->align) {
                t->align = member_t->align;
            }
            if (t->size < member_t->size) {
                t->size = member_t->size;
            }
            cur->next = tmp;
            cur = tmp;

        } while(!cur_token_is(";"));

        expect(TK_SEMICOLON);
    }
    t->member = head.next->next;

    return t;
}

Type *parse_struct_union_decl(TypeKind tk) {
    if (tk != STRUCT && tk != UNION) {
        error("invalid type kind specified: %d", tk);
    }

    Type *t = new_type(tk, NULL, 0);

    // A tag of struct is omittable.
    int tag_exists = 0;
    char *tag_name;
    if (cur_tokenkind_is(TK_IDENT)) {
        tag_name = token->str;
        Tag *registered = get_tag(tag_name);
        if (registered != NULL) {
            next_token();
            return registered->type;
        }

        tag_exists = 1;
        next_token();
    }

    expect(TK_LBRACE);

    if (tk == STRUCT) {
        t = parse_struct_decl(t);
    } else if (tk == UNION) {
        t = parse_union_decl(t);
    }

    expect(TK_RBRACE);

    if (tag_exists) {
        Tag *tag = new_tag(tag_name, t);
        register_new_tag(tag);
    }

    return t;
}

Type *parse_type_prefix() {
    Type *t;
    assert(cur_tokenkind_is(TK_TYPE), "%s is not type", token->str);

    TypeKind type_base;
    if (cur_token_is("int")) {
        next_token();
        t = int_t;
    } else if (cur_token_is("char")) {
        next_token();
        t = char_t;
    } else if (cur_token_is("struct")) {
        next_token();
        t = parse_struct_union_decl(STRUCT);
    } else if (cur_token_is("union")) {
        next_token();
        t = parse_struct_union_decl(UNION);
    } else {
        error("invalid base type: %s", token->str);
    }
    return t;
}

Type *parse_pointer_type(Type *base) {
    do {
        base = pointer_to(base);
        next_token();
    } while (cur_token_is("*"));
    return base;
}

// array
Type *parse_type_suffix(Type *base, char *ident_name) {
    if (!cur_token_is("[")) {
        return base;
    }

    expect(TK_LBRACKET);

    int arr_size = 0;
    if (cur_token_is("]")) {
        // array initialization allow that the number of array elements is not specified.
    } else {
        assert(cur_tokenkind_is(TK_NUM), "array size must be integer literal");
        arr_size = token->val;
        next_token();
    }

    expect(TK_RBRACKET);

    // handle a multi-dimensional array
    base = parse_type_suffix(base, ident_name);

    Type *t = array_of(base, arr_size);
    t->arr_name = ident_name;
    return t;
}

Type *parse_type() {
    Type *t = parse_type_prefix();

    if (cur_token_is("*")) {
        t = parse_pointer_type(t);
    }

    // array
    if (cur_tokenkind_is(TK_IDENT) && next_tokenkind_is(TK_LBRACKET)) {
        char *ident_name = token->str;
        expect(TK_IDENT);
        t = parse_type_suffix(t, ident_name);
    }

    return t;
}

// `[]` operator
Node *parse_indexing(Node *lhs) {
    Node *rhs = parse_expr();
    add_type(lhs);
    add_type(rhs);

    expect(TK_RBRACKET);

    // canonicalize 'expr[ptr]' to 'ptr[expr]'.
    if (!is_pointer(lhs) && is_pointer(rhs)) {
        Node *tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }

    rhs = new_node_with_lr(ND_MUL, rhs, new_node(ND_NUM, lhs->ty->base->size));

    Node *n = new_node(ND_DEREF, 0);
    n->expr = new_node_with_lr(ND_ADD, lhs, rhs);

    return n;
}

// parse some expressions which is separated comma,
// and return a linked list of expressions.
// ex. 20, 40, x, y + 20
Exprs parse_exprs(char *terminator) {
    Exprs result;

    Node head;
    Node *cur = calloc(1, sizeof(Node));
    head.next = cur;
    int count = 0;

    while (!cur_token_is(terminator)) {
        if (cur_token_is(",")) {
            expect(TK_COMMA);
        }
        Node *tmp = parse_expr();
        cur->next = tmp;
        cur = tmp;
        count++;
    }

    result.count = count;
    result.head = head.next->next;
    return result;
}

char *new_label(void) {
    static int count = 0;
    char *buf = malloc(20);
    sprintf(buf, ".L.data.%d", count++);
    return buf;
}

Node *parse_primary() {
    Node *n;
    if (token->tk == TK_LPARENT) {
        next_token();
        if (cur_token_is("{")) {
            next_token();
            n = new_node(ND_STMT_EXPR, 0);
            n = parse_compound_stmt(n, 0);
            expect(TK_RBRACE);
            add_type(n);
        } else {
            n = parse_expr();
        }
        expect(TK_RPARENT);
    } else if (token->tk == TK_IDENT) {
        if (next_tokenkind_is(TK_LPARENT)) {
            // function call
            n = new_node(ND_CALL, 0);
            FuncData fd = new_func_data();

            fd.name = token->str;

            expect(TK_IDENT);
            expect(TK_LPARENT);

            if (!cur_token_is(")")) {
                // function arguments
                Exprs result = parse_exprs(")");
                fd.args = result.head;
                fd.args_num = result.count;
            }

            n->func = fd;
            expect(TK_RPARENT);
        } else {
            // variable (it must be declared already)
            n = new_node(ND_LVAR, 0);
            n->var = get_var(token->str);
            next_token();
        }
    } else if (cur_tokenkind_is(TK_NUM)) {
        n = new_node(ND_NUM, token->val);
        next_token();
    } else if (cur_tokenkind_is(TK_STR)) {
        // string literal
        Type *t = array_of(char_t, token->str_len);
        Var v = new_var(GLOBAL, new_label(), t);
        v.str = token->str;
        v.str_len = token->str_len;

        register_new_gvar(v);

        n = new_node(ND_LVAR, 0);
        n->var = v;

        next_token();
    } else {
        error("invalid primary");
    }
    return n;
}

Node *parse_suffix() {
    Node *n = parse_primary();
    Node *new_n;

    while (1) {
        if (cur_token_is("[")) {
            next_token();
            n = parse_indexing(n);
            continue;
        } else if (cur_token_is("++")) {
            // post increment
            next_token();
            new_n = new_node(ND_POSTINC, 0);
            new_n->expr = n;
            add_type(n);
            if (is_pointer(n)) {
                new_n->inc = new_node(ND_NUM, n->ty->size);
            } else {
                new_n->inc = new_node(ND_NUM, 1);
            }

            n = new_n;
            continue;
        } else if (cur_token_is("--")) {
            // post decrement
            next_token();
            new_n = new_node(ND_POSTDEC, 0);
            new_n->expr = n;
            add_type(n);
            if (is_pointer(n)) {
                new_n->inc = new_node(ND_NUM, n->ty->size);
            } else {
                new_n->inc = new_node(ND_NUM, 1);
            }

            n = new_n;
            continue;
        } else if (cur_token_is(".")) {
            // member access operator
            next_token();
            add_type(n);
            char *member_name = token->str;
            next_token();
            Member *m = get_member(n->ty->member, member_name);
            if (m == NULL) {
                error("unknown member specified: %s", member_name);
            }
            n = new_struct_member_node(n, m, member_name);
            continue;
        } else if (cur_token_is("->")) {
            next_token();
            add_type(n);

            char *member_name = token->str;
            next_token();
            Member *m = get_member(n->ty->base->member, member_name);
            if (m == NULL) {
                error("unknown member specified: %s", member_name);
            }


            Node *deref = new_node(ND_DEREF, 0);
            deref->expr = n;

            n = new_struct_member_node(deref, m, member_name);
            continue;
        }

        break;
    }
    return n;
}

Node *parse_unary() {
    Node *n;

    if (cur_token_is("+")) {
        next_token();
        n = parse_suffix();
    } else if (cur_token_is("-")) {
        next_token();
        Node *lhs = new_node(ND_NUM, 0);
        Node *rhs = parse_suffix();
        n = new_node_with_lr(ND_SUB, lhs, rhs);
    } else if (cur_token_is("*")) {
        next_token();
        n = new_node(ND_DEREF, 0);
        n->expr = parse_unary();
    } else if (cur_token_is("&")) {
        next_token();
        n = new_node(ND_ADDR, 0);
        n->expr = parse_unary();
    } else if (cur_token_is("++")) {
        // pre increment
        next_token();
        n = new_node(ND_PREINC, 0);
        n->expr = parse_suffix();
        add_type(n->expr);
        if (is_pointer(n->expr)) {
            n->inc = new_node(ND_NUM, n->expr->ty->size);
        } else {
            n->inc = new_node(ND_NUM, 1);
        }
    } else if (cur_token_is("--")) {
        // pre decrement
        next_token();
        n = new_node(ND_PREDEC, 0);
        n->expr = parse_suffix();
        add_type(n->expr);
        if (is_pointer(n->expr)) {
            n->inc = new_node(ND_NUM, n->expr->ty->size);
        } else {
            n->inc = new_node(ND_NUM, 1);
        }
    } else if (cur_token_is("sizeof")) {
        next_token();
        Node *tmp = parse_unary();
        add_type(tmp);
        n = new_node(ND_NUM, size_of(tmp->ty));
    } else {
        n = parse_suffix();
    }

    return n;
}

Node *parse_mul() {
    Node *lhs = parse_unary();

    while (cur_token_is("*") || cur_token_is("/") || cur_token_is("%")) {
        if (cur_token_is("*")) {
            next_token();
            lhs = new_node_with_lr(ND_MUL, lhs, parse_unary());
        } else if (cur_token_is("/")) {
            next_token();
            lhs = new_node_with_lr(ND_DIV, lhs, parse_unary());
        } else {
            next_token();
            lhs = new_node_with_lr(ND_REM, lhs, parse_unary());
        }
    }

    return lhs;
}

Node *new_add(Node *lhs) {
    Node *rhs = parse_mul();
    add_type(lhs);
    add_type(rhs);

    // num + num
    if (is_integer(lhs) && is_integer(rhs)) {
        return new_node_with_lr(ND_ADD, lhs, rhs);
    }

    // Canonicalize `num + ptr` to `ptr + num`.
    if (!is_pointer(lhs) && is_pointer(rhs)) {
        Node *tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }

    // ptr + num
    rhs = new_node_with_lr(ND_MUL, rhs, new_node(ND_NUM, lhs->ty->base->size));
    return new_node_with_lr(ND_ADD, lhs, rhs);
}

Node *new_sub(Node *lhs) {
    Node *rhs = parse_mul();
    add_type(lhs);
    add_type(rhs);

    // num - num
    if (is_integer(lhs) && is_integer(rhs)) {
        return new_node_with_lr(ND_SUB, lhs, rhs);
    }

    // ptr - num
    if (is_pointer(lhs) && !is_pointer(rhs)) {
        rhs = new_node_with_lr(ND_MUL, rhs, new_node(ND_NUM, lhs->ty->base->size));
        return new_node_with_lr(ND_SUB, lhs, rhs);
    }

    // ptr - ptr
    if (is_pointer(lhs) && is_pointer(rhs)) {
        Node *sub = new_node_with_lr(ND_SUB, lhs, rhs);
        return new_node_with_lr(ND_DIV, sub, new_node(ND_NUM, lhs->ty->size));
    }

    error("invalid - operation");
    return NULL;
}

Node *parse_add() {
    Node *lhs = parse_mul();

    while (cur_token_is("+") || cur_token_is("-")) {
        if (cur_token_is("+")) {
            next_token();
            lhs = new_add(lhs);
        } else {
            next_token();
            lhs = new_sub(lhs);
        }
    }
    
    return lhs;
}

Node *parse_shift() {
    Node *lhs = parse_add();

    while (cur_token_is("<<") || cur_token_is(">>")) {
        if (cur_token_is("<<")) {
            next_token();
            lhs = new_node_with_lr(ND_LSHIFT, lhs, parse_add());
        } else {
            next_token();
            lhs = new_node_with_lr(ND_RSHIFT, lhs, parse_add());
        }
    }

    return lhs;
}

Node *parse_relational() {
    Node *lhs = parse_shift();

    while (cur_token_is("<") || cur_token_is("<=")
        || cur_token_is(">") || cur_token_is(">=")) {

        if (cur_token_is("<")) {
            next_token();
            lhs = new_node_with_lr(ND_LT, lhs, parse_shift());
        } else if (cur_token_is("<=")) {
            next_token();
            lhs = new_node_with_lr(ND_LE, lhs, parse_shift());
        } else if (cur_token_is(">")) {
            next_token();
            lhs = new_node_with_lr(ND_LT, lhs, parse_shift());
        } else {
            next_token();
            lhs = new_node_with_lr(ND_LE, lhs, parse_shift());
        }
    }

    return lhs;
}

Node *parse_equality() {
    Node *lhs = parse_relational();

    while (cur_token_is("==") || cur_token_is("!=")) {
        if (cur_token_is("==")) {
            next_token();
            lhs = new_node_with_lr(ND_EQ, lhs, parse_relational());
        } else {
            next_token();
            lhs = new_node_with_lr(ND_NE, lhs, parse_relational());
        }
    }

    return lhs;
}

Node *parse_assign() {
    Node *lhs = parse_equality();

    if (cur_token_is("=")) {
        next_token();
        lhs = new_node_with_lr(ND_ASSIGN, lhs, parse_assign());
    } else if (cur_token_is("+=")) {
        next_token();
        Node *add = new_node_with_lr(ND_ADD, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, add);
    } else if (cur_token_is("-=")) {
        next_token();
        Node *sub = new_node_with_lr(ND_SUB, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, sub);
    } else if (cur_token_is("*=")) {
        next_token();
        Node *mul = new_node_with_lr(ND_MUL, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, mul);
    } else if (cur_token_is("/=")) {
        next_token();
        Node *div = new_node_with_lr(ND_DIV, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, div);
    } else if (cur_token_is("%=")) {
        next_token();
        Node *rem = new_node_with_lr(ND_REM, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, rem);
    } else if (cur_token_is("<<=")) {
        next_token();
        Node *lshift = new_node_with_lr(ND_LSHIFT, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, lshift);
    } else if (cur_token_is(">>=")) {
        next_token();
        Node *rshift = new_node_with_lr(ND_RSHIFT, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, rshift);
    }

    return lhs;
}

Node *parse_expr() {
    return parse_assign();
}

Node *parse_initialize(Node *n) {
    // skip '='
    next_token();

    if (n->var.type->tk == ARRAY) {
        expect(TK_LBRACE);

        // parse elements of array.
        Exprs result = parse_exprs("}");
        Node *assigns = new_node(ND_BLOCK, 0);

        // if array size not determined yet, resolve here.
        if (n->var.type->arr_size == 0) {
            VarNode *var_iter = cur_scope->lvar_head;
            while (1) {
                if (equal_strings(n->var.name, var_iter->data.name)) {
                    var_iter->data.type->arr_size = result.count;
                    size_t size = var_iter->data.type->base->size * result.count;
                    var_iter->data.type->size = size;
                    stack_frame_size += size;
                    var_iter->data.offset = stack_frame_size;
                    cur_scope->total_var_size += size;
                    break;
                }
                var_iter = var_iter->next;
            }
        }

        // preparation of linked list
        Node *head = calloc(1, sizeof(Node));
        Node *cur = calloc(1, sizeof(Node));
        head->next = cur;

        Node *elem = result.head;
        // create a linked list which preserve the assigning of each array index.
        // ex.
        // ```
        //     arr[3] = { 10, 20, 30 };
        //
        //     // above converts to below
        //
        //     arr[0] = 10;
        //     arr[1] = 20;
        //     arr[2] = 30;
        // ```
        for (int i = 0; i < result.count; i++) {
            Node *array = new_node(ND_LVAR, 0);
            array->var = get_var(n->var.name);

            // array index expression
            Node *index = new_node_with_lr(
                    ND_MUL,
                    new_node(ND_NUM, i),
                    new_node(ND_NUM, array->var.type->base->size));

            Node *add = new_node_with_lr(ND_ADD, array, index);

            Node *deref = new_node(ND_DEREF, 0);
            deref->expr = add;

            Node *assign = new_node_with_lr(ND_ASSIGN, deref, elem);

            cur->next = assign;
            cur = assign;

            elem = elem->next;
        }

        assigns->block = head->next->next;
        n = assigns;
        expect(TK_RBRACE);
    } else {
        Node *lhs = new_node(ND_LVAR, 0);
        lhs->var = get_var(n->var.name);

        n = new_node_with_lr(ND_ASSIGN, lhs, parse_equality());
    }
    return n;
}

// parse declaration either variables or struct tag.
// 1. variables
//   - <type> ('*'? <ident name> ) (',' '*'? <ident name>)* ('[' <integer> ']')*
// 2. struct tag
//   - 'struct' <tag name> '{' <member declaration> '}' ';'
Node *parse_declaration(VarKind vk) {
    Type *t = parse_type();

    // declaration of struct tag only (not any variables).
    if (t->tk == STRUCT && cur_tokenkind_is(TK_SEMICOLON)) {
        return new_node(ND_DECL, 0);
    }

    Node n;
    Node *cur = new_node(ND_DECL, 0);
    n.next = cur;

    // handle multi variable declarations of the type `t`.
    do {
        Type *each_type = t;

        if (cur_token_is(",")) {
            next_token();
        }

        // pointer
        if (cur_token_is("*")) {
            each_type = parse_pointer_type(each_type);
        }

        char *var_name;
        if (each_type->tk == ARRAY) {
            var_name = each_type->arr_name;
        } else {
            var_name = token->str;
            next_token();
        }

        // array
        if (cur_token_is("[")) {
            each_type = parse_type_suffix(each_type, var_name);
        }

        Var v = new_var(vk, var_name, each_type);
        // add new node of variable at tail of linked list.
        register_new_lvar(v);

        Node *tmp = new_node(ND_DECL, 0);
        tmp->ty = each_type;
        tmp->var = v;
        // simultaneously initialize variable on declaration.
        if (cur_token_is("=")) {
            tmp = parse_initialize(tmp);
        }

        cur->next = tmp;
        cur = tmp;

    // if next token is a certain type (not `t`), another declaration begins.
    } while (cur_token_is(",") && !next_tokenkind_is(TK_TYPE));

    Node *b = new_node(ND_BLOCK, 0);
    b->block = n.next->next;
    return b;
}

// parse stmt, stmt, ... "}"
// if this function is called from top level function, not enter the scope.
Node *parse_compound_stmt(Node *n, int from_func_body) {
    if (!from_func_body) {
        enter_scope();
    }
    if (!cur_token_is("}")) {
        Node head;
        Node *cur = calloc(1, sizeof(Node));
        head.next = cur;

        while (!cur_token_is("}")) {
            Node *tmp = parse_stmt();
            add_type(tmp);
            cur->next = tmp;
            cur = tmp;
        }
        n->block = head.next->next;
    }
    if (!from_func_body) {
        leave_scope();
    }
    return n;
}

Node *parse_stmt() {
    Node *n;
    if (cur_token_is("return")) {
        n = new_node(ND_RETURN, 0);
        next_token();
        n->expr = parse_expr();
        expect(TK_SEMICOLON);
        return n;
    } else if (cur_token_is("if")) {
        n = new_node(ND_IF, 0);
        next_token();
        expect(TK_LPARENT);
        n->cond = parse_expr();
        expect(TK_RPARENT);
        n->then = parse_stmt();
        if (cur_token_is("else")) {
            next_token();
            n->alt = parse_stmt();
        }
        return n;
    } else if (cur_token_is("while")) {
        n = new_node(ND_WHILE, 0);
        next_token();
        expect(TK_LPARENT);
        n->cond = parse_expr();
        expect(TK_RPARENT);
        n->then = parse_stmt();
        return n;
    } else if (cur_token_is("for")) {
        n = new_node(ND_FOR, 0);
        next_token();
        expect(TK_LPARENT);
        if (!cur_tokenkind_is(TK_SEMICOLON)) {
            n->init = parse_stmt();
        } else {
            expect(TK_SEMICOLON);
        }
        if (!cur_tokenkind_is(TK_SEMICOLON)) {
            n->cond = parse_expr();
        }
        expect(TK_SEMICOLON);
        if (!cur_tokenkind_is(TK_RPARENT)) {
            n->post = parse_expr();
        }
        expect(TK_RPARENT);
        n->then = parse_stmt();
        return n;
    } else if (cur_token_is("{")) {
        n = new_node(ND_BLOCK, 0);
        next_token();
        n = parse_compound_stmt(n, 0);
        expect(TK_RBRACE);
        return n;
    } else if (cur_tokenkind_is(TK_TYPE)) {
        // `n` 's NodeKind is 'ND_DECL'.
        n = parse_declaration(LOCAL);

        expect(TK_SEMICOLON);
        return n;
    } else {
        n = parse_expr();
        expect(TK_SEMICOLON);
    }
    return n;
}

void init_function_context() {
    toplevel_scope = NULL;
    cur_scope = NULL;
    scope_depth = 0;
    stack_frame_size = 0;
}

Node *parse_toplevel_func(Type *ret_t, char *func_name) {
    init_function_context();
    enter_scope();

    Node *n = new_node(ND_FUNC, 0);
    FuncData func_data = new_func_data();

    // return type
    func_data.return_type = ret_t;

    // function name
    func_data.name = func_name;

    // parse argument
    expect(TK_LPARENT);
    if (!cur_token_is(")")) {
        // function arguments
        int args_num = 0;
        while (!cur_token_is(")")) {
            if (cur_token_is(",")) {
                expect(TK_COMMA);
            }
            Node *arg = parse_declaration(ARG);
            args_num++;
        }
        func_data.args_num = args_num;
    }
    expect(TK_RPARENT);

    // parse function body
    Node *body = new_node(ND_BLOCK, 0);
    expect(TK_LBRACE);
    func_data.body = parse_compound_stmt(body, 1);
    expect(TK_RBRACE);

    // store the head of linked list which include local variables.
    // func_data.vars = cur_scope->lvar_head;
    func_data.toplevel_scope = toplevel_scope;
    func_data.stack_frame_size = stack_frame_size;

    n->func = func_data;
    leave_scope();
    return n;
}

void parse_toplevel_global_var(Type *t, char *gname) {
    Var v = new_var(GLOBAL, gname, t);
    register_new_gvar(v);
    expect(TK_SEMICOLON);
}

Node *funcs[100];
int func_count = 0;

// parse type and identifier, and switch parsing if token is '(' or not.
void parse_toplevel() {
    Type *t = parse_type();
    char *ident_name;
    if (t->tk == ARRAY) {
        ident_name = t->arr_name;
    } else {
        ident_name = token->str;
        next_token();
    }

    if (cur_token_is("(")) {
        // top level function definition
        funcs[func_count++] = parse_toplevel_func(t, ident_name);
    } else {
        // global variable declaration
        parse_toplevel_global_var(t, ident_name);
    }
}

void parse_program() {
    while (token->tk != TK_EOF) {
        parse_toplevel();
    }
    funcs[func_count] = NULL;
}

Program *parse(struct Token *t) {
    token = t;
    parse_program();
    Program *p = calloc(1, sizeof(Program));
    p->funcs = funcs;
    p->gvars = gvar_head;
    return p;
}
