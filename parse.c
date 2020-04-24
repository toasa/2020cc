#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "parse.h"

int equal_string(char *s1, char *s2) {
    do {
        if (*s1 != *s2) { return 0; }
        s1++;
        s2++;
    } while (*s1 && *s2);
    return 1;
}

Node *new_node(NodeKind nk, int val) {
    Node *n = calloc(1, sizeof(Node));
    n->nk = nk;
    n->lhs = NULL;
    n->rhs = NULL;
    n->val = val;
    return n;
}

// 現在検査中のトークン
Token *token;

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

Node *parse_primary() {
    Node *n;
    if (token->tk == TK_LPARENT) {
        next_token();
        n = parse_expr();
        expect(TK_RPARENT);
    } else if (token->tk == TK_IDENT) {
        n = new_node(ND_LVAR, 0);
        n->offset = (token->str[0] - 'a' + 1) * 8;
        next_token();
    } else {
        n = new_node(ND_NUM, token->val);
        next_token();
    }
    return n;
}

Node *parse_unary() {
    Node *n;

    if (equal_string("+", token->str)) {
        next_token();
        n = parse_primary();
    } else if (equal_string("-", token->str)) {
        next_token();

        n = new_node(ND_SUB, 0);
        Node *lhs = new_node(ND_NUM, 0);
        Node *rhs = parse_primary();
        n->lhs = lhs;
        n->rhs = rhs;
    } else {
        n = parse_primary();
    }

    return n;
}

Node *parse_mul() {
    Node *lhs = parse_unary();

    while (equal_string("*", token->str) || equal_string("/", token->str)) {
        if (equal_string("*", token->str)) {
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

    while (equal_string("+", token->str) || equal_string("-", token->str)) {
        if (equal_string("+", token->str)) {
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
    Node *n = parse_expr();
    expect(TK_SEMICOLON);
    return n;
}

Node *code[100];

void parse_program() {
    int i = 0;
    while (token->tk != TK_EOF) {
        Node *n = parse_stmt();
        code[i++] = n;
    }
    code[i] = NULL;
}

Node **parse(Token *t) {
    token = t;
    parse_program();
    return code;
}
