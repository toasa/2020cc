// #include "token.h"

typedef enum {
    ND_NUM,
    ND_ADD,
    ND_SUB,
    ND_MUL,
} NodeKind;

typedef struct Node {
    NodeKind nk;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

Node *parse(Token *t);
