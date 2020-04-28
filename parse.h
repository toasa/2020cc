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
} NodeKind;

typedef struct Node {
    NodeKind nk;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    int offset;       // nkがND_LVARの場合に使う

    struct Node *cond; // nkがND_IFの場合に使う
    struct Node *then; // nkがND_IFの場合に使う
} Node;

Node **parse(Token *t);
