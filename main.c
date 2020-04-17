#include <stdio.h>

typedef enum {
    TK_NUM,
    TK_RESERVED,
    TK_EOF,
} TokenKind;

typedef struct {
    TokenKind tk;
    int val;
    char *str;
} Token;

Token new_token(TokenKind tk, int val, char *str) {
    Token t = {tk, val, str};
    return t;
}

Token tokens[100];

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

void gen_asm() {
    int i = 0;
    Token t = tokens[i];
    for (; t.tk != TK_EOF; t = tokens[i]) {
        if (t.tk == TK_NUM) {
            printf("    mov rax, %d\n", t.val);
            i++;
        } else if (t.tk == TK_RESERVED) {
            if (equal_string("+", t.str)) {
                i++;
                Token num_token = tokens[i];
                printf("    add rax, %d\n", num_token.val);
            } else if (equal_string("-", t.str)) {
                i++;
                Token num_token = tokens[i];
                printf("    sub rax, %d\n", num_token.val);
            }
            i++;
        }
    }
}

void skip(char **input) {
    while (is_space(**input)) {
        (*input)++;
    }
}

void tokenize(char *input) {
    int i = 0;
    while (*input) {
        skip(&input);
        if (is_digit(*input)) {
            int num = read_number(&input);
            tokens[i] = new_token(TK_NUM, num, "");
        } else if (*input == '+') {
            input++;
            tokens[i] = new_token(TK_RESERVED, 0, "+");
        } else if (*input == '-') {
            input++;
            tokens[i] = new_token(TK_RESERVED, 0, "-");
        } else {
            break;
        }
        i++;
    }
    tokens[i] = new_token(TK_EOF, 0, "");
}

int main(int argc, char **argv) {
    char *input = argv[1];
    tokenize(input);
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    gen_asm();
    printf("    ret\n");
}
