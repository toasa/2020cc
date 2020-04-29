typedef enum {
    ND_NUM,
    ND_LVAR,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
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
} NodeKind;

typedef struct Node {
    NodeKind nk;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    int offset;        // nkがND_LVARの場合に使う

    struct Node *cond; // nkがND_IF, ND_WHILE, ND_FORの場合に使う
    struct Node *then; // nkがND_IF, ND_WHILE, ND_FORの場合に使う
    struct Node *alt;  // nkがND_IFの場合に使う
    struct Node *expr; // nkがND_FORの場合に使う
    struct Node *post; // nkがND_FORの場合に使う
    struct Node *next; // nkがND_BLOCK, ND_CALLの場合に使う

    char *name;   // nkがND_CALLの場合に使う
    int args_num; // nkがND_CALLの場合に使う
} Node;

Node **parse(Token *t);
