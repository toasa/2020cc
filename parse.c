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
        if (i.type->tk == ARRAY) {
            new_i->data.offset = i.type->size;
        } else {
            new_i->data.offset = i.type->size;
        }
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

Type *new_type(TypeKind tk, Type *ptr_to) {
    Type *t = calloc(1, sizeof(Type));
    t->tk = tk;
    t->ptr_to = ptr_to;
    return t;
}

// when a type is array in parsing, a name of array was passed because continue to array size after the name. so preserve the array name to the global variable `arr_name`.
char *arr_name;

size_t get_type_size(TypeKind t) {
    if (t == INT) {
        // TODO: It should will be 4, but currently any local variable assign to 64bit register in code generating, so treat size of 'INT' as 8bytes.
        return 8;
    }
    return 8;
}

Type *parse_type() {
    Type *t;
    assert(cur_tokenkind_is(TK_TYPE), "%s is not type", token->str);

    TypeKind type_base = INT;

    next_token();

    if (cur_token_is("*")) {
        // pointer
        t = new_type(type_base, NULL);
        t->size = get_type_size(type_base);

        do {
            Type *p = new_type(PTR, t);
            p->size = get_type_size(PTR);
            t = p;
            next_token();
        } while (cur_token_is("*"));
    } else if  (cur_tokenkind_is(TK_IDENT) && next_tokenkind_is(TK_LBRACKET)) {
        // array

        // In `parse_type()` skip the identifier name, so preserve global variable: `arr_name`.
        arr_name = token->str;
        expect(TK_IDENT);
        expect(TK_LBRACKET);

        assert(cur_tokenkind_is(TK_NUM), "array size must be integer literal");
        t = new_type(ARRAY, NULL);
        t->array_size = token->val;
        // TODO: other type array
        t->size = get_type_size(INT) * t->array_size;
        next_token();
        expect(TK_RBRACKET);
    } else {
        // local identifier
        t = new_type(type_base, NULL);
        t->size = get_type_size(type_base);
    }

    return t;
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
                int args_num = 1;
                Node *head = calloc(1, sizeof(Node));
                Node *cur = parse_expr();
                head->next = cur;
                while (!cur_token_is(")")) {
                    expect(TK_COMMA);
                    Node *tmp = parse_expr();
                    cur->next = tmp;
                    cur = tmp;
                    args_num++;
                }
                fd.args = head->next;
                fd.args_num = args_num;
            }

            n->func = fd;
            expect(TK_RPARENT);
        } else if (next_tokenkind_is(TK_LBRACKET)) {
            // array index operator
            Node *array = new_node(ND_LVAR, 0);
            array->ident = get_ident(token->str);

            expect(TK_IDENT);
            expect(TK_LBRACKET);

            // array index expression
            Node *index = new_node(ND_MUL, 0);
            index->lhs = parse_expr();
            size_t element_count = array->ident.type->array_size;
            size_t total_bytes = array->ident.type->size;
            size_t size_of_elem = total_bytes / element_count;
            index->rhs = new_node(ND_NUM, size_of_elem);

            Node *add = new_node(ND_ADD, 0);
            add->lhs = array;
            add->rhs = index;

            expect(TK_RBRACKET);

            n = new_node(ND_DEREF, 0);
            n->expr = add;
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
    return n;
}

int is_pointer(Node *n) {
    if (n->nk == ND_ADDR) { return 1; }
    if (n->nk == ND_LVAR) {
        if (n->ident.type->tk == PTR || n->ident.type->tk == ARRAY) {
            return 1;
        }
    }

    // TODO? it should be recursive?
    if (n->lhs != NULL) {
        if (is_pointer(n->lhs)) {
            return 1;
        }
    }
    if (n->rhs != NULL) {
        if (is_pointer(n->rhs)) {
            return 1;
        }
    }
    return 0;
}

Node *parse_unary() {
    Node *n;

    if (cur_token_is("+")) {
        next_token();
        n = parse_primary();
    } else if (cur_token_is("-")) {
        next_token();

        n = new_node(ND_SUB, 0);
        Node *lhs = new_node(ND_NUM, 0);
        Node *rhs = parse_primary();
        n->lhs = lhs;
        n->rhs = rhs;
    } else if (cur_token_is("*")) {
        next_token();
        n = new_node(ND_DEREF, 0);
        n->expr = parse_unary();
    } else if (cur_token_is("&")) {
        next_token();
        n = new_node(ND_ADDR, 0);
        n->expr = parse_unary();
    } else if (cur_token_is("sizeof")) {
        next_token();
        Node *tmp = parse_unary();
        // 構文木 n に紐付いている型が int なら 4 を
        // ポインタなら 8 を生成する
        // TODO: implement for a case of array.
        if (is_pointer(tmp)) {
            n = new_node(ND_NUM, 8);
        } else {
            n = new_node(ND_NUM, 4);
        }
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
            Node *rhs = parse_unary();
            Node *n = new_node(ND_MUL, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else if (cur_token_is("/")) {
            next_token();
            Node *rhs = parse_unary();
            Node *n = new_node(ND_DIV, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else {
            next_token();
            Node *rhs = parse_unary();
            Node *n = new_node(ND_REM, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        }
    }

    return lhs;
}

Node *new_add(Node *lhs) {
    Node *n = new_node(ND_ADD, 0);
    Node *rhs = parse_mul();

    // TODO? it should handle pointer + pointer?
    if (is_pointer(lhs) && !is_pointer(rhs)) {
        Node *new_rhs = new_node(ND_MUL, 0);
        new_rhs->lhs = new_node(ND_NUM, 8);
        new_rhs->rhs = rhs;
        rhs = new_rhs;
    } else if (!is_pointer(lhs) && is_pointer(rhs)) {
        Node *new_lhs = new_node(ND_MUL, 0);
        new_lhs->lhs = new_node(ND_NUM, 8);
        new_lhs->rhs = rhs;
        lhs = new_lhs;
    }
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

Node *new_sub(Node *lhs) {
    Node *n = new_node(ND_SUB, 0);
    Node *rhs = parse_mul();

    // TODO? it should handle pointer - pointer?
    if (is_pointer(lhs) && !is_pointer(rhs)) {
        Node *new_rhs = new_node(ND_MUL, 0);
        new_rhs->lhs = new_node(ND_NUM, 8);
        new_rhs->rhs = rhs;
        rhs = new_rhs;
    } else if (!is_pointer(lhs) && is_pointer(rhs)) {
        Node *new_lhs = new_node(ND_MUL, 0);
        new_lhs->lhs = new_node(ND_NUM, 8);
        new_lhs->rhs = rhs;
        lhs = new_lhs;
    }
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
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
            Node *rhs = parse_add();
            Node *n = new_node(ND_LSHIFT, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else {
            next_token();
            Node *rhs = parse_add();
            Node *n = new_node(ND_RSHIFT, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        }
    }

    return lhs;
}

Node *parse_relational() {
    Node *lhs = parse_shift();

    while (token->tk == TK_LT || token->tk == TK_LE
        || token->tk == TK_GT || token->tk == TK_GE) {

        if (token->tk == TK_LT) {
            next_token();
            Node *rhs = parse_shift();
            Node *n = new_node(ND_LT, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else if (token->tk == TK_LE) {
            next_token();
            Node *rhs = parse_shift();
            Node *n = new_node(ND_LE, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else if (token->tk == TK_GT) {
            next_token();
            Node *rhs = parse_shift();
            Node *n = new_node(ND_LT, 0);
            n->lhs = rhs;
            n->rhs = lhs;
            lhs = n;
        } else {
            next_token();
            Node *rhs = parse_shift();
            Node *n = new_node(ND_LE, 0);
            n->lhs = rhs;
            n->rhs = lhs;
            lhs = n;
        }
    }

    return lhs;
}

Node *parse_equality() {
    Node *lhs = parse_relational();

    while (token->tk == TK_EQ || token->tk == TK_NE) {
        if (token->tk == TK_EQ) {
            next_token();
            Node *rhs = parse_relational();
            Node *n = new_node(ND_EQ, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else {
            next_token();
            Node *rhs = parse_relational();
            Node *n = new_node(ND_NE, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        }
    }

    return lhs;
}

Node *parse_assign() {
    Node *lhs = parse_equality();

    if (token->tk == TK_ASSIGN) {
        next_token();
        Node *rhs = parse_assign();
        Node *n = new_node(ND_ASSIGN, 0);
        n->lhs = lhs;
        n->rhs = rhs;
        lhs = n;
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

    char *ident_name;
    if (t->tk == ARRAY) {
        ident_name = arr_name;
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
        next_token();
        Node *lhs = parse_expr();
        n = new_node(ND_RETURN, 0);
        n->expr = lhs;
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
        if (!next_tokenkind_is(TK_SEMICOLON)) {
            n->expr = parse_expr();
        }
        expect(TK_SEMICOLON);
        if (!next_tokenkind_is(TK_SEMICOLON)) {
            n->cond = parse_expr();
        }
        expect(TK_SEMICOLON);
        if (!next_tokenkind_is(TK_RPARENT)) {
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
            head->next = cur;
            while (!cur_token_is("}")) {
                Node *tmp = parse_stmt();
                cur->next = tmp;
                cur = tmp;
            }
            n->block = head->next;
        }
        expect(TK_RBRACE);
    } else if (cur_tokenkind_is(TK_TYPE)) {
        // `n` 's NodeKind is 'ND_DECL'.
        n = parse_declaration(ID_LOCAL);
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
