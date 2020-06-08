//
// token.c
//
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
    TK_PERIOD,    // .
    TK_ARROW,     // ->
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

//
// parce.c
//
typedef enum {
    ND_NUM,
    ND_LVAR,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_REM,       // %
    ND_LSHIFT,    // <<
    ND_RSHIFT,    // >>
    ND_DEREF,     // *
    ND_ADDR,      // &
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LT,        // <
    ND_LE,        // <=
    ND_ASSIGN,    // =
    ND_PREINC,    // ++
    ND_POSTINC,   // ++
    ND_PREDEC,    // --
    ND_POSTDEC,   // --
    ND_RETURN,    // return
    ND_IF,        // if
    ND_WHILE,     // while
    ND_FOR,       // for
    ND_BLOCK,     // { ... }
    ND_CALL,      // function call
    ND_FUNC,      // function definition
    ND_DECL,      // local variable declaration
    ND_STMT_EXPR, // statement expression
    ND_MEMBER,    // . (struct member access)
} NodeKind;

typedef enum {
    INT,
    CHAR,
    PTR,
    ARRAY,
    STRUCT,
    UNION,
} TypeKind;

typedef struct Member Member;

typedef struct Type {
    TypeKind tk;
    size_t size;
    int align;

    struct Type *base; // base type of array or pointer

    char *arr_name;
    size_t arr_size;   // the number of elements

    Member *member;    // struct
} Type;

struct Member {
    char *name;
    Type *type;
    Member *next;
    int offset;
};

typedef enum VarKind {
    LOCAL,
    GLOBAL,
    ARG,
    MEMBER,
} VarKind;

typedef struct Var {
    VarKind vk;
    char *name;
    Type *type;
    int is_global;

    // local variable
    int offset;

    // global variable
    char *str;
    int str_len;
} Var;

typedef struct VarNode {
    Var data;
    struct VarNode *next;
} VarNode;

typedef struct Tag {
    char *name;
    Type *type; // typekind is STRUCT only.
    struct Tag *next;
} Tag;

typedef struct Scope {
    struct Scope *high;
    VarNode *lvar_head;
    Tag *tag_head;
    size_t depth;
    size_t total_var_size;
} Scope;

typedef struct FuncData {
    char *name;
    struct Node *body;
    Type *return_type;

    int args_num;
    struct Node *args; // nkがND_CALLの場合に使う。次の引数には`next`メンバからアクセスする。

    size_t stack_frame_size;
    Scope *toplevel_scope;
} FuncData;

typedef struct Node {
    NodeKind nk;
    struct Node *lhs;
    struct Node *rhs;
    int val;

    Type *ty;

    Var var;            // nkがND_LVAR, ND_DECLの場合に使う
    FuncData func;      // nkがND_CALL, ND_FUNCの場合に使う

    Member *member;     // nkがND_MEMBERの場合に使う

    struct Node *init;  // nkがND_FORの場合に使う
    struct Node *cond;  // nkがND_IF, ND_WHILE, ND_FORの場合に使う
    struct Node *then;  // nkがND_IF, ND_WHILE, ND_FORの場合に使う
    struct Node *alt;   // nkがND_IFの場合に使う
    struct Node *expr;  // nkがND_RETURN, ND_DEREF, ND_ADDR, ND_MEMBERの場合に使う
    struct Node *post;  // nkがND_FORの場合に使う
    struct Node *next;  // nkがND_BLOCK, ND_CALL, ND_FUNC, ND_DECLの場合に使う
    struct Node *block; // nkがND_BLOCK, ND_STMT_EXPRの場合に使う
    struct Node *inc;   // nkがND_PREINC, ND_PREDEC, ND_POSTINC, ND_POSTDECの場合に使う
} Node;

typedef struct Program {
    struct Node **funcs;
    VarNode *gvars;
} Program;

Program *parse(Token *t);

//
// type.c
//
extern Type *int_t;
extern Type *char_t;
Type *new_type(TypeKind tk, Type *base, int align);
size_t size_of(Type *t);
int align_to(int n, int align);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
void add_type(Node *n);

//
// codegen.c
//
void gen(Program *p);

//
// util.c
//
int equal_strings(char *s1, char *s2);
void assert(int result, char *fmt, ...);
void error(char *fmt, ...);
