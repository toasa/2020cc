typedef enum {
    ND_NUM,
    ND_LVAR,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_REM,     // %
    ND_LSHIFT,  // <<
    ND_RSHIFT,  // >>
    ND_DEREF,   // *
    ND_ADDR,    // &
    ND_EQ,      // ==
    ND_NE,      // !=
    ND_LT,      // <
    ND_LE,      // <=
    ND_ASSIGN,  // =
    ND_PREINC,  // ++
    ND_POSTINC, // ++
    ND_PREDEC,  // --
    ND_POSTDEC, // --
    ND_RETURN,  // return
    ND_IF,      // if
    ND_WHILE,   // while
    ND_FOR,     // for
    ND_BLOCK,   // { ... }
    ND_CALL,    // function call
    ND_FUNC,    // function definition
    ND_DECL,    // local variable declaration
} NodeKind;

typedef enum {
    INT,
    PTR,
    ARRAY
} TypeKind;

typedef struct Type {
    TypeKind tk;
    size_t size;
    struct Type *base; // base type of array or pointer
    char *arr_name;
    size_t arr_size;   // the number of elements
} Type;

typedef enum IdentKind {
    ID_LOCAL,
    ID_GLOBAL,
    ID_ARG,
} IdentKind;

typedef struct Ident {
    IdentKind ik;
    char *name;
    int offset;
    Type *type;
    int is_global;
} Ident;

typedef struct IdentNode {
    Ident data;
    struct IdentNode *next;
} IdentNode;

typedef struct FuncData {
    char *name;
    struct Node *body;
    Type *return_type;

    int args_num;
    struct Node *args;   // nkがND_CALLの場合に使う。次の引数には`next`メンバからアクセスする。

    IdentNode *idents;   // nkがND_FUNCの場合に引数のオフセット値を取得する
} FuncData;

typedef struct Node {
    NodeKind nk;
    struct Node *lhs;
    struct Node *rhs;
    int val;

    Type *ty;

    Ident ident;        // nkがND_LVAR, ND_DECLの場合に使う
    FuncData func;      // nkがND_CALL, ND_FUNCの場合に使う

    struct Node *init;  // nkがND_FORの場合に使う
    struct Node *cond;  // nkがND_IF, ND_WHILE, ND_FORの場合に使う
    struct Node *then;  // nkがND_IF, ND_WHILE, ND_FORの場合に使う
    struct Node *alt;   // nkがND_IFの場合に使う
    struct Node *expr;  // nkがND_RETURN, ND_DEREF, ND_ADDRの場合に使う
    struct Node *post;  // nkがND_FORの場合に使う
    struct Node *next;  // nkがND_BLOCK, ND_CALL, ND_FUNCの場合に使う
    struct Node *block; // nkがND_BLOCKの場合に使う
    struct Node *inc;   // nkがND_PREINC, ND_PREDEC, ND_POSTINC, ND_POSTDECの場合に使う
} Node;

Node **parse(Token *t);
