#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TK_NUM,
    TK_RESERVED,
    TK_EOF,
} TokenKind;

typedef struct Token {
    TokenKind tk;
    int val;
    char *str;
    struct Token *next;
} Token;

Token *new_token(Token *prev, TokenKind tk, int val, char *str) {
    Token *t = calloc(1, sizeof(Token));
    t->tk = tk;
    t->val = val;
    t->str = str;
    prev->next = t;
    return t;
}

int is_digit(char c) {
    return ('0' <= c && c <= '9');
}

int is_char(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int is_space(char c) {
    return c == ' ';
}

int read_number(char **input) {
    int num = 0;
    while (is_digit(**input)) {
        num = num * 10 + (**input - '0');
        (*input)++;
    }
    return num;
}

int equal_string(char *s1, char *s2) {
    do {
        if (*s1 != *s2) { return 0; }
        s1++;
        s2++;
    } while (*s1 && *s2);
    return 1;
}

void gen_asm(Token *t) {
    for (; t->tk != TK_EOF; t = t->next) {
        if (t->tk == TK_NUM) {
            printf("    mov rax, %d\n", t->val);
        } else if (t->tk == TK_RESERVED) {
            if (equal_string("+", t->str)) {
                t = t->next;
                printf("    add rax, %d\n", t->val);
            } else if (equal_string("-", t->str)) {
                t = t->next;
                printf("    sub rax, %d\n", t->val);
            }
        }
    }
}

void skip(char **input) {
    while (is_space(**input)) {
        (*input)++;
    }
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
        } else {
            break;
        }
    }
    cur_token = new_token(cur_token, TK_EOF, 0, "");
    return head_token->next;
}

int main(int argc, char **argv) {
    char *input = argv[1];
    Token *t = tokenize(input);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    gen_asm(t);
    printf("    ret\n");
}
