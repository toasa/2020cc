#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parse.h"
#include "util.h"

Node *new_node(NodeKind nk, int val) {
    Node *n = calloc(1, sizeof(Node));
    n->nk = nk;
    n->lhs = NULL;
    n->rhs = NULL;
    n->cond = NULL;
    n->then = NULL;
    n->alt = NULL;
    n->expr = NULL;
    n->post = NULL;
    n->next = NULL;
    n->val = val;
    return n;
}

// 現在検査中のトークン
Token *token;

int cur_token_is(char *s) {
    return equal_strings(s, token->str);
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
        printf("expect token error\n");
        exit(1);
    }
    next_token();
}

Node *parse_expr();

typedef struct KV {
    char *key;
    int val;
    struct KV *next;
} KV;

KV *new_KV(char *key, int val) {
    KV *kv = calloc(1, sizeof(KV));
    kv->key = key;
    kv->val = val;
    kv->next = NULL;
    return kv;
}

// Function context
// These have to be initiate at beginning in `parse_toplevel_func`.
KV *head;
int ident_num;

// insert tail of linked list that stores identifier.
void insert(char *K, int V) {
    // increment the number of identifier.
    ident_num++;

    KV *kv_new = new_KV(K, V);
    if (head == NULL) {
        head = kv_new;
        return;
    }

    KV *kv_iter = head;
    while (kv_iter->next != NULL) {
        kv_iter = kv_iter->next;
    }
    kv_iter->next = kv_new;
}

int get_offset(char *ident) {
    int count = 0;
    KV *kv_iter = head;
    while (kv_iter != NULL) {
        if (equal_strings(ident, kv_iter->key)) {
            return kv_iter->val;
        }
        count++;
        kv_iter = kv_iter->next;
    }
    int offset = (count + 1) * 8;
    insert(ident, offset);
    return offset;
}

Node *parse_primary() {
    Node *n;
    if (token->tk == TK_LPARENT) {
        next_token();
        n = parse_expr();
        expect(TK_RPARENT);
    } else if (token->tk == TK_IDENT) {
        // function call
        if (next_tokenkind_is(TK_LPARENT)) {
            n = new_node(ND_CALL, 0);
            n->name = token->str;
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
                n->next = head->next;
                n->args_num = args_num;
            }

            expect(TK_RPARENT);
        } else {
            n = new_node(ND_LVAR, 0);
            n->offset = get_offset(token->str);
            next_token();
        }
    } else {
        n = new_node(ND_NUM, token->val);
        next_token();
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
    } else {
        n = parse_primary();
    }

    return n;
}

Node *parse_mul() {
    Node *lhs = parse_unary();

    while (cur_token_is("*") || cur_token_is("/")) {
        if (cur_token_is("*")) {
            next_token();
            Node *rhs = parse_unary();
            Node *n = new_node(ND_MUL, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else {
            next_token();
            Node *rhs = parse_unary();
            Node *n = new_node(ND_DIV, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        }
    }

    return lhs;
}

Node *parse_add() {
    Node *lhs = parse_mul();

    while (cur_token_is("+") || cur_token_is("-")) {
        if (cur_token_is("+")) {
            next_token();
            Node *rhs = parse_mul();
            Node *n = new_node(ND_ADD, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else {
            next_token();
            Node *rhs = parse_mul();
            Node *n = new_node(ND_SUB, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        }
    }
    
    return lhs;
}

Node *parse_relational() {
    Node *lhs = parse_add();

    while (token->tk == TK_LT || token->tk == TK_LE
        || token->tk == TK_GT || token->tk == TK_GE) {

        if (token->tk == TK_LT) {
            next_token();
            Node *rhs = parse_add();
            Node *n = new_node(ND_LT, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else if (token->tk == TK_LE) {
            next_token();
            Node *rhs = parse_add();
            Node *n = new_node(ND_LE, 0);
            n->lhs = lhs;
            n->rhs = rhs;
            lhs = n;
        } else if (token->tk == TK_GT) {
            next_token();
            Node *rhs = parse_add();
            Node *n = new_node(ND_LT, 0);
            n->lhs = rhs;
            n->rhs = lhs;
            lhs = n;
        } else {
            next_token();
            Node *rhs = parse_add();
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
    } else {
        n = parse_expr();
        expect(TK_SEMICOLON);
    }
    return n;
}

Node *parse_toplevel_func() {
    head = NULL;
    ident_num = 0;
    Node *n = new_node(ND_FUNC, 0);
    n->name = token->str;
    next_token();

    // parse argument
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
        n->next = head->next;
        n->args_num = args_num;
    }

    expect(TK_RPARENT);

    // parse function body
    n->body = parse_stmt();

    // the number of function identifier equals
    // the number of function arguments plus local variables.
    n->ident_num = ident_num;
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
