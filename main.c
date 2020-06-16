#include "2020cc.h"

char *read_file(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        error("cannot open %s", path);
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        error("fail to fseek %s", path);
    }

    size_t size = ftell(fp);

    if (fseek(fp, 0, SEEK_SET) != 0) {
        error("fail to fseek %s", path);
    }

    char *buf = calloc(1, size + 2);
    fread(buf, size, 1, fp);

    if (size == 0 || buf[size - 1] != '\n') {
        buf[size++] = '\n';
    }
    buf[size] = '\0';
    fclose(fp);
    return buf;
}

int main(int argc, char **argv) {
    char *input = read_file(argv[1]);
    Token *t = tokenize(input);
    Program *p = parse(t);
    gen(p);
}
