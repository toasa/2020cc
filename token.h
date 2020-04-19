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

Token *tokenize(char *input);

