typedef enum {
    ND_NUM,
    ND_LVAR,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_DEREF,  // *
    ND_ADDR,   // &
    ND_EQ,     // ==
    ND_NE,     // !=
    ND_LT,     // <
    ND_LE,     // <=
    ND_ASSIGN, // =
    ND_RETURN, // return
    ND_IF,     // if
    ND_WHILE,  // while
    ND_FOR,    // for
    ND_BLOCK,  // { ... }
    ND_CALL,   // function call
    ND_FUNC,   // function definition
} NodeKind;

typedef struct Node {
    NodeKind nk;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    int offset;         // nkがND_LVARの場合に使う

    struct Node *cond;  // nkがND_IF, ND_WHILE, ND_FORの場合に使う
    struct Node *then;  // nkがND_IF, ND_WHILE, ND_FORの場合に使う
    struct Node *alt;   // nkがND_IFの場合に使う
    struct Node *expr;  // nkがND_FOR, ND_RETURN, ND_DEREF, ND_ADDRの場合に使う
    struct Node *post;  // nkがND_FORの場合に使う
    struct Node *next;  // nkがND_BLOCK, ND_CALL, ND_FUNCの場合に使う
    struct Node *body;  // nkがND_FUNCの場合に使う
    struct Node *block; // nkがND_BLOCKの場合に使う

    char *name;    // nkがND_CALL, ND_FUNCの場合に使う
    int args_num;  // nkがND_CALL, ND_FUNCの場合に使う
    int ident_num; // nkがND_FUNCの場合に使う
} Node;

Node **parse(Token *t);
