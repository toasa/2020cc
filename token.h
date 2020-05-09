typedef enum {
    TK_NUM,
    TK_RESERVED,
    TK_IDENT,
    TK_LPARENT,   // (
    TK_RPARENT,   // )
    TK_LBRACE,    // {
    TK_RBRACE,    // }
    TK_LBRACKET,  // [
    TK_RBRACKET,  // ]
    TK_EQ,        // ==
    TK_NE,        // !=
    TK_LT,        // <
    TK_LE,        // <=
    TK_GT,        // >
    TK_GE,        // >=
    TK_ASSIGN,    // =
    TK_SEMICOLON, // ;
    TK_COMMA,     // ,
    TK_TYPE,      // 'int', 'void', 'char',...
    TK_EOF,
} TokenKind;

typedef struct Token {
    TokenKind tk;
    int val;
    char *str;
    struct Token *next;
} Token;

Token *tokenize(char *input);

