typedef enum {
    TK_NUM,
    TK_RESERVED,
    TK_LPARENT,  // (
    TK_RPARENT,  // )
    TK_EQ,       // ==
    TK_NE,       // !=
    TK_EOF,
} TokenKind;

typedef struct Token {
    TokenKind tk;
    int val;
    char *str;
    struct Token *next;
} Token;

Token *tokenize(char *input);

