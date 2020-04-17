#include <stdio.h>

int is_digit(char c) {
    return ('0' <= c && c <= '9');
}

int is_char(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int read_number(char **input) {
    int num = 0;
    while (is_digit(**input)) {
        num = num * 10 + (**input - '0');
        (*input)++;
    }
    return num;
}

void gen_asm(char *input) {
    char *c = input;
    while (*c) {
        if (is_digit(*c)) {
            int num = read_number(&c);
            printf("    mov rax, %d\n", num);
        } else if (*c == '+') {
            c++;
            int num = read_number(&c);
            printf("    add rax, %d\n", num);
        } else if (*c == '-') {
            c++;
            int num = read_number(&c);
            printf("    sub rax, %d\n", num);
        }
    }
}

int main(int argc, char **argv) {
    char *input = argv[1];
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    gen_asm(input);
    printf("    ret\n");
}
