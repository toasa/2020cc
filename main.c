#include <stdio.h>

int main(int argc, char **argv) {
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %s\n", argv[1]);
    printf("    ret\n");
}
