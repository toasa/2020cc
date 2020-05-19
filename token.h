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
    TK_SEMICOLON, // ;
    TK_COMMA,     // ,
    TK_TYPE,      // 'int', 'void', 'char',...
    TK_STR,
    TK_EOF,
} TokenKind;

typedef struct Token {
    TokenKind tk;
    int val;
    char *str;
    int str_len;        // tkがTK_STRの場合に使用
    struct Token *next;
} Token;

Token *tokenize(char *input);

