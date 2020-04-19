#include <stdlib.h>
#include "token.h"

int is_digit(char c) {
    return ('0' <= c && c <= '9');
}

int is_char(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int is_space(char c) {
    return c == ' ';
}

void skip(char **input) {
    while (is_space(**input)) {
        (*input)++;
    }
}

int read_number(char **input) {
    int num = 0;
    while (is_digit(**input)) {
        num = num * 10 + (**input - '0');
        (*input)++;
    }
    return num;
}

Token *new_token(Token *prev, TokenKind tk, int val, char *str) {
    Token *t = calloc(1, sizeof(Token));
    t->tk = tk;
    t->val = val;
    t->str = str;
    prev->next = t;
    return t;
}

Token *tokenize(char *input) {
    Token *head_token = calloc(1, sizeof(Token));
    Token *cur_token = head_token;
    
    while (*input) {
        skip(&input);
        if (is_digit(*input)) {
            int num = read_number(&input);
            cur_token = new_token(cur_token, TK_NUM, num, "");
        } else if (*input == '+') {
            input++;
            cur_token = new_token(cur_token, TK_RESERVED, 0, "+");
        } else if (*input == '-') {
            input++;
            cur_token = new_token(cur_token, TK_RESERVED, 0, "-");
        } else if (*input == '*') {
            input++;
            cur_token = new_token(cur_token, TK_RESERVED, 0, "*");
        } else if (*input == '/') {
            input++;
            cur_token = new_token(cur_token, TK_RESERVED, 0, "/");
        } else if (*input == '(') {
            input++;
            cur_token = new_token(cur_token, TK_LPARENT, 0, "(");
        } else if (*input == ')') {
            input++;
            cur_token = new_token(cur_token, TK_RPARENT, 0, ")");
        } else if (*input == '=') {
            input++;
            if (*input == '=') {
                input++;
                cur_token = new_token(cur_token, TK_EQ, 0, "==");
            }
        } else if (*input == '!') {
            input++;
            if (*input == '=') {
                input++;
                cur_token = new_token(cur_token, TK_NE, 0, "!=");
            }
        } else {
            break;
        }
    }
    cur_token = new_token(cur_token, TK_EOF, 0, "");
    return head_token->next;
}

