#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parse.h"
#include "util.h"

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
        error("expect token error");
    }
    next_token();
}

Node *parse_expr();

Type *int_t = &(Type){INT, 8};

Type *new_type(TypeKind tk, Type *base) {
    Type *t = calloc(1, sizeof(Type));
    t->tk = tk;
    t->base = base;
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

int is_pointer(Node *n) {
    if (n->ty == NULL) { return 0; }
    return (n->ty->tk == PTR) || (n->ty->tk == ARRAY);
}

int is_integer(Node *n) {
    if (n->ty == NULL) { return 0; }
    return n->ty->tk == INT;
}

FuncData new_func_data() {
    FuncData data;
    data.name = NULL;
    data.idents = NULL;
    data.body = NULL;
    data.args_num = 0;
    return data;
}

Ident new_ident(IdentKind ik, char *name, Type *t) {
    Ident i;
    i.ik = ik;
    i.name = name;
    i.type = t;
    return i;
}

IdentNode *new_ident_node(Ident i) {
    IdentNode *ident_node = calloc(1, sizeof(IdentNode));
    ident_node->data = i;
    ident_node->next = NULL;
    return ident_node;
}

// a head of linked list stored identifier infomation.
IdentNode *ident_head = NULL;

// add one `IdentNode` at tail of linked list which stored idefifier
// which occurs in currently parsing function.
void register_new_ident(Ident i) {
    IdentNode *new_i = new_ident_node(i);

    // To calculate a offset for new identifier, add size of each identifier while iterate the linked list.
    int offset = 0;

    if (ident_head == NULL) {
        new_i->data.offset = i.type->size;
        ident_head = new_i;
        return;
    }

    offset += ident_head->data.type->size;

    IdentNode *ident_iter = ident_head;
    while (ident_iter->next != NULL) {
        char *cur_name = ident_iter->data.name;
        // check duplication of identifier declaration.
        if (equal_strings(cur_name, i.name)) {
            error("duplicate of identifier declaration.");
        }

        offset += ident_iter->data.type->size;
        ident_iter = ident_iter->next;
    }

    new_i->data.offset = i.type->size + offset;

    ident_iter->next = new_i;
}

// search the 'name' from the linked list of identifier.
// if the identifier 'name' is not declared then occur compile error
// and abort.
Ident get_ident(char *name) {
    IdentNode *ident_iter = ident_head;
    while (ident_iter != NULL) {
        char *cur_name = ident_iter->data.name;
        if (equal_strings(cur_name, name)) {
            break;
        }
        ident_iter = ident_iter->next;
    }

    if (ident_iter == NULL) {
        // found a undeclared identifier.
        error("%s: undeclared identifier.", name);
    }

    return ident_iter->data;
}

// memory size allocated in stack.
size_t get_type_msize(TypeKind t) {
    if (t == INT) {
        return 8;
    }
    return 8;
}

// to calculate `sizeof` operator.
size_t size_of(Type *t) {
    if (t->tk == ARRAY) {
        return t->arr_size * size_of(t->base);
    } else if (t->tk == PTR) {
        return 8;
    } else {
        // INT
        return 4;
    }
}

Type *parse_type() {
    Type *t;
    assert(cur_tokenkind_is(TK_TYPE), "%s is not type", token->str);

    TypeKind type_base = INT;

    // local identifier
    t = new_type(type_base, NULL);
    t->size = get_type_msize(type_base);

    next_token();

    if (cur_token_is("*")) {
        // pointer
        do {
            Type *p = new_type(PTR, t);
            p->size = get_type_msize(PTR);
            t = p;
            next_token();
        } while (cur_token_is("*"));
    }

    Type *base = t;

    if  (cur_tokenkind_is(TK_IDENT) && next_tokenkind_is(TK_LBRACKET)) {
        // array
        t = new_type(ARRAY, NULL);
        t->arr_name = token->str;
        t->base = base;

        expect(TK_IDENT);
        expect(TK_LBRACKET);

        if (cur_token_is("]")) {
            // array initialization allow that the number of array elements is not specified.
        } else {
            assert(cur_tokenkind_is(TK_NUM), "array size must be integer literal");
            t->arr_size = token->val;
            t->size = base->size * t->arr_size;
            next_token();
        }

        expect(TK_RBRACKET);
    }

    return t;
}

Node *parse_array(Node *lhs) {
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

// return value of `parse_exprs`.
typedef struct Exprs {
    int count;
    Node *head;
} Exprs;

// parse some expressions which is separated comma,
// and return a linked list of expressions.
// ex. 20, 40, x, y + 20
Exprs parse_exprs(char *terminator) {
    Exprs result;

    int count = 1;
    Node *head = calloc(1, sizeof(Node));
    Node *cur = parse_expr();
    head->next = cur;

    while (!cur_token_is(terminator)) {
        expect(TK_COMMA);
        Node *tmp = parse_expr();
        cur->next = tmp;
        cur = tmp;
        count++;
    }

    result.count = count;
    result.head = head->next;
    return result;
}

Node *parse_primary() {
    Node *n;
    if (token->tk == TK_LPARENT) {
        next_token();
        n = parse_expr();
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
            // identifier (it must be declared already)
            n = new_node(ND_LVAR, 0);
            n->ident = get_ident(token->str);
            next_token();
        }
    } else {
        n = new_node(ND_NUM, token->val);
        next_token();
    }

    Node *new_n;

    if (cur_token_is("[")) {
        next_token();
        n = parse_array(n);
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
    }

    return n;
}

Node *parse_unary() {
    Node *n;

    if (cur_token_is("+")) {
        next_token();
        n = parse_primary();
    } else if (cur_token_is("-")) {
        next_token();
        Node *lhs = new_node(ND_NUM, 0);
        Node *rhs = parse_primary();
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
        n->expr = parse_primary();
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
        n->expr = parse_primary();
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
        n = parse_primary();
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

    // `int` + `int`
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

    // `int` - `int`
    if (is_integer(lhs) && is_integer(rhs)) {
        return new_node_with_lr(ND_SUB, lhs, rhs);
    }

    // `ptr` - `int`
    if (is_pointer(lhs) && !is_pointer(rhs)) {
        rhs = new_node_with_lr(ND_MUL, rhs, new_node(ND_NUM, lhs->ty->base->size));
        return new_node_with_lr(ND_SUB, lhs, rhs);
    }

    // `ptr` - `ptr`
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

Node *parse_declaration(IdentKind ik) {
    Node *n = new_node(ND_DECL, 0);

    // declaration statement;
    Type *t = parse_type();
    n->ty = t;

    char *ident_name;
    if (t->tk == ARRAY) {
        ident_name = t->arr_name;
    } else {
        ident_name = token->str;
        next_token();
    }

    Ident i = new_ident(ik, ident_name, t);

    // add new ident node at tail of linked list.
    register_new_ident(i);

    n->ident = i;

    return n;
}

Node *parse_stmt() {
    Node *n;
    if (cur_token_is("return")) {
        n = new_node(ND_RETURN, 0);
        next_token();
        n->expr = parse_expr();
        expect(TK_SEMICOLON);
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
    } else if (cur_token_is("while")) {
        n = new_node(ND_WHILE, 0);
        next_token();
        expect(TK_LPARENT);
        n->cond = parse_expr();
        expect(TK_RPARENT);
        n->then = parse_stmt();
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
    } else if (cur_token_is("{")) {
        n = new_node(ND_BLOCK, 0);
        next_token();
        if (!cur_token_is("}")) {
            Node *head = calloc(1, sizeof(Node));
            Node *cur = parse_stmt();
            add_type(cur);
            head->next = cur;
            while (!cur_token_is("}")) {
                Node *tmp = parse_stmt();
                cur->next = tmp;
                cur = tmp;
                add_type(cur);
            }
            n->block = head->next;
        }
        expect(TK_RBRACE);
    } else if (cur_tokenkind_is(TK_TYPE)) {
        // `n` 's NodeKind is 'ND_DECL'.
        n = parse_declaration(ID_LOCAL);

        // simultaneously initialize identifer on declaration.
        if (cur_token_is("=")) {
            next_token();

            if (n->ident.type->tk == ARRAY) {
                expect(TK_LBRACE);

                // parse elements of array.
                Exprs result = parse_exprs("}");
                Node *assigns = new_node(ND_BLOCK, 0);

                // preparation of linked list
                Node *head = calloc(1, sizeof(Node));
                Node *cur = calloc(1, sizeof(Node));
                head->next = cur;

                // if array size not determined yet, resolve here.
                if (n->ident.type->arr_size == 0) {
                    IdentNode *ident_iter = ident_head;
                    int offset = 0;
                    while (1) {
                        if (equal_strings(n->ident.name, ident_iter->data.name)) {
                            ident_iter->data.type->arr_size = result.count;
                            size_t size = ident_iter->data.type->base->size * result.count;
                            ident_iter->data.type->size = size;
                            ident_iter->data.offset = offset + size;
                            break;
                        }
                        offset += ident_iter->data.type->size;
                        ident_iter = ident_iter->next;
                    }
                }

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
                    array->ident = get_ident(n->ident.name);

                    // array index expression
                    Node *index = new_node(ND_MUL, 0);
                    index->lhs = new_node(ND_NUM, i);

                    size_t element_count = array->ident.type->arr_size;
                    size_t total_bytes = array->ident.type->size;
                    size_t size_of_elem = total_bytes / element_count;
                    index->rhs = new_node(ND_NUM, size_of_elem);

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
                lhs->ident = get_ident(n->ident.name);
                n = new_node_with_lr(ND_ASSIGN, lhs, parse_equality());
            }
        }
        expect(TK_SEMICOLON);
    } else {
        n = parse_expr();
        expect(TK_SEMICOLON);
    }
    return n;
}

void init_function_context() {
    ident_head = NULL;
}

Node *parse_toplevel_func() {
    init_function_context();
    Node *n = new_node(ND_FUNC, 0);
    FuncData func_data = new_func_data();

    // return type
    func_data.return_type = parse_type();

    // function name
    func_data.name = token->str;
    next_token();

    // parse argument
    expect(TK_LPARENT);
    if (!cur_token_is(")")) {
        // function arguments
        int args_num = 1;
        parse_declaration(ID_ARG);
        while (!cur_token_is(")")) {
            expect(TK_COMMA);
            parse_declaration(ID_ARG);
            args_num++;
        }
        func_data.args_num = args_num;
    }

    expect(TK_RPARENT);

    // parse function body
    func_data.body = parse_stmt();

    // store the head of linked list which include identifiers.
    func_data.idents = ident_head;

    n->func = func_data;
    return n;
}

Node *funcs[100];

void parse_program() {
    int i = 0;
    while (token->tk != TK_EOF) {
        funcs[i] = parse_toplevel_func();
        i++;
    }
    funcs[i] = NULL;
}

Node **parse(Token *t) {
    token = t;
    parse_program();
    return funcs;
}
