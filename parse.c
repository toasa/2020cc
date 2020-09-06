#include "2020cc.h"

Node *new_node(NodeKind nk) {
    Node *n = calloc(1, sizeof(Node));
    n->nk = nk;
    return n;
}

Node *new_num_node(int val) {
    Node *n = new_node(ND_NUM);
    n->val = val;
    return n;
}

Node *new_node_with_lr(NodeKind nk, Node *lhs, Node *rhs) {
    Node *n = new_node(nk);
    n->lhs = lhs;
    n->rhs = rhs;
    return n;
}

Node *new_struct_member_node(Node *stru, Member *mem, char *mem_name) {
    Node *mem_node = new_node(ND_MEMBER);
    mem_node->member = mem;
    mem_node->expr = stru;
    return mem_node;
}

Node *new_lvar_node(Var v) {
    Node *n = new_node(ND_LVAR);
    n->var = v;
    return n;
}

Node *new_unary_node(NodeKind nk, Node *expr) {
    Node *n = new_node(nk);
    n->expr = expr;
    return n;
}

Node *new_cast_node(Node *expr, Type *ty) {
    add_type(expr);

    Node *n = calloc(1, sizeof(Node));
    n->nk = ND_CAST;
    n->expr = expr;
    n->ty = copy_type(ty);
    return n;
}

// 現在検査中のトークン
Token *token;

int cur_token_is(char *s) {
    return equal_strings(s, token->str);
}

int cur_tokenkind_is(TokenKind tk) {
    return (token->tk == tk);
}

int next_tokenkind_is(TokenKind tk) {
    if (token->tk == TK_EOF) {
        return 0;
    }
    return (token->next->tk == tk);
}

void next_token() {
    token = token->next;
}

void expect(TokenKind tk) {
    if (token->tk != tk) {
        error("expect %d token, but got %d", tk, token->tk);
    }
    next_token();
}

Node *parse_expr();

FuncData *new_func_data() {
    return calloc(1, sizeof(FuncData));
}

Var new_var(VarKind vk, char *name, Type *t) {
    Var v;
    v.vk = vk;
    v.name = name;
    v.type = t;
    v.offset = 0;
    v.str = NULL;
    v.str_len = 0;
    v.attr.is_typedef = false;
    v.attr.is_static = false;
    if (vk == GLOBAL) {
        v.is_global = 1;
    } else {
        v.is_global = 0;
    }
    return v;
}

VarNode *new_var_node(Var v) {
    VarNode *var_node = calloc(1, sizeof(VarNode));
    var_node->data = v;
    var_node->next = NULL;
    return var_node;
}

Scope *new_scope(int depth) {
    Scope *s = calloc(1, sizeof(Scope));
    s->depth = depth;
    return s;
}

// a head of linked list which store global variables.
VarNode *gvar_head = NULL;

size_t scope_depth = 0;
Scope *toplevel_scope = NULL;
Scope *cur_scope = NULL;
Type *cur_function_return_type;

// Points to a node representing a switch if we are
// parsing a switch statement.
Node *cur_switch;
size_t stack_frame_size = 0;

const int MAX_FUNC_NUM = 500;
FuncData *funcs[MAX_FUNC_NUM];
int func_count = 0;

void enter_scope() {
    Scope *s = new_scope(scope_depth);

    if (scope_depth == 0) {
        toplevel_scope = s;
    }

    s->high = cur_scope;
    cur_scope = s;
    scope_depth++;
}

void leave_scope() {
    cur_scope = cur_scope->high;
    scope_depth--;
}

// add one `VarNode` at head of linked list which stored idefifier in current scope.
void register_new_lvar(Var v) {
    VarNode *new_v = new_var_node(v);

    if (!v.attr.is_typedef) {
        cur_scope->total_var_size += v.type->size;
        stack_frame_size += v.type->size;
        new_v->data.offset = stack_frame_size;
    }

    new_v->next = cur_scope->lvar_head;
    cur_scope->lvar_head = new_v;
}

// add one `VarNode` at head of linked list of global variables.
void register_new_gvar(Var v) {
    VarNode *new_v = new_var_node(v);
    new_v->next = gvar_head;
    gvar_head = new_v;
}

// `head` is `lvar_head` or `gvar_head`.
//
// `name`: name of variable which to search for.
// `head`: head node of linked list of variables, `lvar_head` or `gvar_head`.
//
VarNode *look_up_var(char *name, VarNode *head) {
    VarNode *var_iter = head;
    while (var_iter != NULL) {
        char *cur_name = var_iter->data.name;
        if (equal_strings(cur_name, name)) {
            return var_iter;
        }
        var_iter = var_iter->next;
    }

    return NULL;
}

FuncData *look_up_func(char *name) {
    for (int i = 0; funcs[i] != NULL; i++) {
        if (equal_strings(funcs[i]->name, name)) {
            return funcs[i];
        }
    }
    return NULL;
}

// Search the 'name' from the linked list of variable.
// First, look up the local variables from current level scope to highest.
// The next, look up the global variables.
// If not found then occur compile error and abort.
VarNode *get_var(char *name) {
    VarNode *v;

    Scope *tmp_scope = cur_scope;
    while (tmp_scope != NULL) {
        v = look_up_var(name, tmp_scope->lvar_head);
        if (v != NULL) {
            return v;
        }
        tmp_scope = tmp_scope->high;
    }

    v = look_up_var(name, gvar_head);
    return v;
}

bool is_defined_type(char *t_name) {
    VarNode *v = get_var(t_name);
    if (v != NULL && v->data.attr.is_typedef) {
        return true;
    }
    return false;
}

Tag *new_tag(char *tag_name, Type *type) {
    assert(
        type->tk == STRUCT
        || type->tk == UNION
        || type->tk == ENUM,
        "tag is used for struct, union, or enum"
    );
    Tag *t = calloc(1, sizeof(Tag));
    t->name = tag_name;
    t->type = type;
    return t;
}

void register_new_tag(Tag *tag) {
    if (cur_scope->tag_head == NULL) {
        cur_scope->tag_head = tag;
        return;
    }

    Tag *t_iter = cur_scope->tag_head;
    while (t_iter->next != NULL) {
        t_iter = t_iter->next;
    }

    t_iter->next = tag;
}

Tag *get_tag(char *tag_name) {
    Tag *t_iter = cur_scope->tag_head;
    while (t_iter != NULL) {
        if (equal_strings(t_iter->name, tag_name)) {
            return t_iter;
        }
        t_iter = t_iter->next;
    }
    return NULL;
}

// return value of `parse_exprs`.
typedef struct Exprs {
    int count;
    Node *head;
} Exprs;

Type *parse_struct_decl(Type *t);
Type *parse_union_decl(Type *t);
Type *parse_type_suffix(Type *t);
Type *parse_typename(VarAttr *attr);
Type *parse_type_specifier(VarAttr *attr);
Type *parse_pointer(Type *base);
Type *parse_direct_declarator(Type *t);
Type *parse_declarator(Type *t);
Node *parse_initializer(Var v);
Node *parse_init_declarator(VarKind vk, Type *t, VarAttr *attr);
Node *parse_declaration(VarKind vk);
Exprs parse_exprs(char *terminator);
Node *parse_primary();
Node *parse_postfix();
Node *parse_cast();
Node *parse_unary();
Node *parse_mul();
Node *new_add(Node *lhs, Node *rhs);
Node *new_sub(Node *lhs, Node *rhs);
Node *parse_add();
Node *parse_shift();
Node *parse_relational();
Node *parse_equality();
Node *parse_bitand();
Node *parse_bitxor();
Node *parse_bitor();
Node *parse_logand();
Node *parse_logor();
Node *parse_cond();
Node *parse_assign();
Node *parse_expr();
long const_expr();
Node *parse_compound_stmt(Node*, int);
Node *parse_stmt_sub();
Node *parse_stmt();
FuncData *parse_toplevel_func(Type *ret_t, bool is_static);
void parse_toplevel_global_var(Type *t);
void parse_toplevel();
void parse_program();

Member *new_member(Type *t) {
    Member *m = calloc(1, sizeof(Member));
    m->name = t->name;
    m->type = t;
    return m;
}

Member *get_member(Member *m, char *name) {
    Member *m_iter = m;
    while (m_iter != NULL) {
        if (equal_strings(m_iter->name, name)) {
            return m_iter;
        }
        m_iter = m_iter->next;
    }
    return NULL;
}

Type *parse_struct_decl(Type *t) {
    Member head;
    Member *cur = calloc(1, sizeof(Member));
    head.next = cur;
    int total_size = 0;

    while (!cur_token_is("}")) {
        Type *member_t = parse_type_specifier(NULL);
        int i = 0;

        do {
            if (i++ > 0) {
                expect(TK_COMMA);
            }
            Type *each_member_t = parse_declarator(member_t);
            Member *tmp = new_member(each_member_t);
            total_size = align_to(total_size, each_member_t->align);
            tmp->offset = total_size;
            total_size += each_member_t->size;

            if (t->align < each_member_t->align) {
                t->align = each_member_t->align;
            }
            cur->next = tmp;
            cur = tmp;
        } while (!cur_token_is(";"));

        expect(TK_SEMICOLON);
    }

    t->member = head.next->next;
    t->size = total_size;

    return t;
}

Type *parse_union_decl(Type *t) {
    Member head;
    Member *cur = calloc(1, sizeof(Member));
    head.next = cur;

    while (!cur_token_is("}")) {
        Type *member_t = parse_type_specifier(NULL);
        int i = 0;

        // parse some members which have same type `member_t`.
        do {
            if (i++ > 0) {
                expect(TK_COMMA);
            }
            Type *each_member_t = parse_declarator(member_t);
            Member *tmp = new_member(each_member_t);
            if (t->align < each_member_t->align) {
                t->align = each_member_t->align;
            }
            if (t->size < each_member_t->size) {
                t->size = each_member_t->size;
            }
            cur->next = tmp;
            cur = tmp;
        } while(!cur_token_is(";"));

        expect(TK_SEMICOLON);
    }

    t->member = head.next->next;
    t->size = align_to(t->size, t->align);

    return t;
}

// struct-union-decl = ('struct'|'union') (tag-name)? '{' struct-declaration-list '}'
Type *parse_struct_union_decl(TypeKind tk) {
    if (tk != STRUCT && tk != UNION) {
        error("invalid type kind specified: %d", tk);
    }

    Type *t = new_type(tk, NULL, 0);

    // A tag of struct is omittable.
    int tag_exists = 0;
    char *tag_name;
    if (cur_tokenkind_is(TK_IDENT)) {
        tag_name = token->str;
        next_token();
        tag_exists = 1;
    }

    if (tag_exists && !cur_token_is("{")) {
        Tag *registered = get_tag(tag_name);
        if (registered != NULL) {
            return registered->type;
        }
        Tag *tag = new_tag(tag_name, t);
        register_new_tag(tag);
        return t;
    }

    expect(TK_LBRACE);

    if (tk == STRUCT) {
        t = parse_struct_decl(t);
    } else if (tk == UNION) {
        t = parse_union_decl(t);
    }

    expect(TK_RBRACE);

    if (tag_exists) {
        Tag *registered = get_tag(tag_name);
        // redefinition of struct tag.
        if (registered != NULL) {
            *registered->type = *t;
            return t;
        }

        Tag *tag = new_tag(tag_name, t);
        register_new_tag(tag);
    }

    return t;
}

// enum-decl = ident? '{' enum-list? '}'
//           | ident ('{' enum-list? '}')?
//
// enum-list = ident ('=' num)? (',' ident ('=' num)?)*
Type *parse_enum_decl() {
    Type *t = new_type(ENUM, NULL, 4);
    t->size = 4;

    int tag_exists = 0;
    char *tag_name = NULL;
    if (cur_tokenkind_is(TK_IDENT)) {
        tag_name = token->str;
        tag_exists = 1;
        next_token();
    }

    if (tag_name != NULL && !cur_token_is("{")) {
        t->name = tag_name;
        Tag *registered = get_tag(tag_name);
        if (registered == NULL) {
            error("unknown enum type: %s", registered->name);
        }
        if (registered->type->tk != ENUM) {
            error("enum type not found: %s", registered->name);
        }
        return registered->type;
    }

    expect(TK_LBRACE);

    int i = 0;
    int count = 0;
    while (!cur_token_is("}")) {
        if (i++ > 0) {
            expect(TK_COMMA);
        }

        char *name = token->str;
        next_token();

        Var v = new_var(ENUMMEMBER, name, t);

        if (cur_token_is("=")) {
            next_token();
            count = const_expr();
        }
        v.enum_val = count++;

        register_new_lvar(v);
    }

    expect(TK_RBRACE);

    if (tag_exists) {
        Tag *tag = new_tag(tag_name, t);
        register_new_tag(tag);
    }

    return t;
}

// handle a specific type combination.
//
//     1. int long
//        long int      => long
//     2. short int
//        int short     => short
//     3. long long
//        long long int => long
//
Type *parse_type_specifier(VarAttr *attr) {
    int counter = 0;

    enum {
        TY_VOID = 1 << 0,
        TY_BOOL = 1 << 2,
        TY_CHAR = 1 << 4,
        TY_SHORT = 1 << 6,
        TY_INT = 1 << 8,
        TY_LONG = 1 << 10,
        TY_STRUCT = 1 << 12,
        TY_UNION = 1 << 14,
        TY_ENUM = 1 << 16,
    };

    VarNode *v = get_var(token->str);
    if (v != NULL && v->data.attr.is_typedef) {
        next_token();
        attr->is_typedef = false;
        return v->data.type;
    }

    // type default is 'int'.
    Type *t = int_t;

    while (cur_tokenkind_is(TK_TYPE) || cur_tokenkind_is(TK_STORAGE)) {
        if (cur_tokenkind_is(TK_STORAGE)) {
            if (cur_token_is("typedef")) {
                attr->is_typedef = true;
            } else if (cur_token_is("static")) {
                attr->is_static = true;
            }
            next_token();
            continue;
        }
        if (cur_token_is("void")) {
            counter += TY_VOID;
        } else if (cur_token_is("_Bool")) {
            counter += TY_BOOL;
        } else if (cur_token_is("char")) {
            counter += TY_CHAR;
        } else if (cur_token_is("short")) {
            counter += TY_SHORT;
        } else if (cur_token_is("int")) {
            counter += TY_INT;
        } else if (cur_token_is("long")) {
            counter += TY_LONG;
        } else if (cur_token_is("struct")) {
            counter += TY_STRUCT;
        } else if (cur_token_is("union")) {
            counter += TY_UNION;
        } else if (cur_token_is("enum")) {
            counter += TY_ENUM;
        } else {
            error("unknown type name: %s", token->str);
        }

        next_token();

        switch (counter) {
        case TY_VOID:
            t = void_t;
            break;
        case TY_BOOL:
            t = bool_t;
            break;
        case TY_CHAR:
            t = char_t;
            break;
        case TY_SHORT:
        case TY_SHORT + TY_INT:
            t = short_t;
            break;
        case TY_INT:
            t = int_t;
            break;
        case TY_LONG:
        case TY_LONG + TY_INT:
        case TY_LONG + TY_LONG:
        case TY_LONG + TY_LONG + TY_INT:
            t = long_t;
            break;
        case TY_STRUCT:
            t = parse_struct_union_decl(STRUCT);
            break;
        case TY_UNION:
            t = parse_struct_union_decl(UNION);
            break;
        case TY_ENUM:
            t = parse_enum_decl();
            break;
        default:
            error("invalid type");
        }
    }

    assert(t != NULL, "valid type wasn't specified");
    return t;
}

void resolve_array_size(Var var) {
    // count element of array.
    int arr_size = 1;
    Token *t_org = token;
    while (!cur_token_is("}")) {
        if (cur_token_is(",")) {
            arr_size++;
        }
        next_token();
    }
    token = t_org;

    VarNode *var_iter = cur_scope->lvar_head;
    while (1) {
        if (equal_strings(var.name, var_iter->data.name)) {
            var_iter->data.type->arr_size = arr_size;
            size_t size = var_iter->data.type->base->size * arr_size;
            var_iter->data.type->size = size;
            stack_frame_size += size;
            var_iter->data.offset = stack_frame_size;
            cur_scope->total_var_size += size;
            break;
        }
        var_iter = var_iter->next;
    }
}

// singly linked list.
struct SLL {
    Node *head;
    Node *tail;
};

// Get source array or pointer type recursively.
Type *get_src_array_type(Type *t) {
    if (is_pointer(t) && !is_pointer(t->base)) {
        return t;
    }
    return get_src_array_type(t->base);
}

// To parse nested initialization of array, `array_elements_count`
// holds a index of array which is parsed already.
int array_elements_count;

// parse one dimensional array.
struct SLL parse_array_init_one_dim(Type *type, char *name) {
    expect(TK_LBRACE);

    // preparation of linked list
    Node head;
    Node *cur = calloc(1, sizeof(Node));
    head.next = cur;

    VarNode *v = get_var(name);
    if (v == NULL) {
        error("Undeclared variable: %s\n", name);
    }

    Type *src_type = get_src_array_type(type);
    Node *array = new_lvar_node(v->data);
    array->ty = src_type;

    for (int i = 0; i < type->arr_size; i++) {
        if (i != 0 && !cur_tokenkind_is(TK_RBRACE)) {
            expect(TK_COMMA);
        }

        // We have to serialize a multidimensional array
        // into a one dimensinal to handle a initialization
        // of the array. So the base type must be a source type
        // in any multidimensinal array.
        Node *add = new_add(array, new_num_node(array_elements_count++));
        add->ty = src_type;

        Node *deref = new_unary_node(ND_DEREF, add);
        Node *assign;

        // If a element of array is not initialized explicitly,
        // compiler inserts zero initialization.
        if (cur_tokenkind_is(TK_RBRACE)) {
            assign = new_node_with_lr(ND_ASSIGN, deref, new_num_node(0));
        } else {
            assign = new_node_with_lr(ND_ASSIGN, deref, parse_assign());
        }

        cur->next = assign;
        cur = assign;

    }

    expect(TK_RBRACE);

    struct SLL sll;
    sll.head = head.next->next;
    sll.tail = cur;
    return sll;
}

struct SLL parse_array_initializer_rec(Type *type, char *name) {
    if (type->base->tk != ARRAY) {
        return parse_array_init_one_dim(type, name);
    }

    expect(TK_LBRACE);

    VarNode *v = get_var(name);
    if (v == NULL) {
        error("Undeclared variable: %s\n", name);
    }

    Node *head = NULL;
    Node *tail = NULL;
    for (int i = 0; i < type->arr_size; i++) {
        if (i != 0) {
            expect(TK_COMMA);
        }
        struct SLL sll = parse_array_initializer_rec(type->base, name);

        if (i == 0) {
            head = sll.head;
            tail = sll.tail;
            continue;
        }

        // merge two linked lists.
        tail->next = sll.head;
        tail = sll.tail;
    }

    expect(TK_RBRACE);

    struct SLL sll;
    sll.head = head;
    sll.tail = tail;
    return sll;
}

// Create a linked list which preserve the assigning of each array index.
// For example, convert
//
// ```
//     arr[2][3] = { {1,2,3}, {4,5,6} };
// ```
//
// into
//
// ```
//     arr[0] = 1;
//     arr[1] = 2;
//     arr[2] = 3;
//     arr[3] = 4;
//     arr[4] = 5;
//     arr[5] = 6;
// ```
// .
Node *parse_array_initializer(Type *type, char *name) {
    Node *b = new_node(ND_BLOCK);
    array_elements_count = 0;
    b->block = parse_array_initializer_rec(type, name).head;
    add_type(b);
    return b;
}

// initializer = assignment-expression
//             | '{' initializer-list ','? '}'
Node *parse_initializer(Var var) {
    if (var.type->tk == ARRAY) {

        // if array size not determined yet, resolve here.
        // TODO? currently, can handle a one dimensional array only.
        if (var.type->arr_size == 0) {
            resolve_array_size(var);
        }

        return parse_array_initializer(var.type, var.type->name);
    }

    VarNode *v = get_var(var.name);
    if (v == NULL) {
        error("Undeclared variable: %s\n", var.name);
    }
    Node *lhs = new_lvar_node(v->data);
    return new_node_with_lr(ND_ASSIGN, lhs, parse_equality());
}

// type-suffix = ('[' const-expr ']')*
Type *parse_type_suffix(Type *t) {
    if (!cur_token_is("[")) {
        return t;
    }

    expect(TK_LBRACKET);
    int arr_size = 0;
    if (cur_token_is("]")) {
        // array initialization allow that the number of array elements is not specified.
    } else {
        arr_size = const_expr();
    }
    expect(TK_RBRACKET);

    t = parse_direct_declarator(t);
    return array_of(t, arr_size);
}

// direct-declarator = identifier (type-suffix)?
//                   | '(' declarator ')' (type-suffix)?
Type *parse_direct_declarator(Type *t) {
    if (cur_tokenkind_is(TK_IDENT)) {
        t->name = token->str;
        next_token();
    } else if (cur_token_is("(")) {
        expect(TK_LPARENT);
        Type *placeholder = calloc(1, sizeof(Type));
        Type *new_ty = parse_declarator(placeholder);
        expect(TK_RPARENT);
        *placeholder = *parse_type_suffix(t);
        return new_ty;
    }

    return parse_type_suffix(t);
}

// pointer = '*'+
Type *parse_pointer(Type *base) {
    do {
        base = pointer_to(base);
        next_token();
    } while (cur_token_is("*"));
    return base;
}

// declarator = pointer? direct-declarator
Type *parse_declarator(Type *t) {
    if (cur_token_is("*")) {
        t = parse_pointer(t);
    }
    return parse_direct_declarator(t);
}

// typename = type_specifier declarator
Type *parse_typename(VarAttr *attr) {
    Type *t = parse_type_specifier(attr);
    return parse_declarator(t);
}

// init_declarator = declarator
//                 | declarator '=' initializer
Node *parse_init_declarator(VarKind vk, Type *t, VarAttr *attr) {
    t = parse_declarator(t);

    Var v = new_var(vk, t->name, t);
    if (attr != NULL) {
        v.attr.is_typedef = attr->is_typedef;
    }

    // add new node of variable at head of linked list.
    if (vk == GLOBAL) {
        register_new_gvar(v);
    } else {
        register_new_lvar(v);
    }

    Node *n;
    n = new_node(ND_DECL);
    n->ty = t;
    n->var = v;

    if (cur_token_is("=")) {
        next_token();
        return parse_initializer(v);
    }

    return n;
}

// declaration = type_specifier (init_declarator (',' init_declarator)*)? ';'
Node *parse_declaration(VarKind vk) {
    VarAttr attr = {};
    Type *t = parse_type_specifier(&attr);

    Node n;
    Node *cur = new_node(ND_DECL);

    if (cur_token_is(";")) {
        next_token();
        return cur;
    }

    n.next = cur;

    int i = 0;
    while (!cur_token_is(";")) {
        if (i++ > 0) {
            expect(TK_COMMA);
        }

        Node *tmp = parse_init_declarator(vk, t, &attr);

        cur->next = tmp;
        cur = tmp;
    }

    expect(TK_SEMICOLON);

    Node *b = new_node(ND_BLOCK);
    b->block = n.next->next;
    return b;
}


// `[]` operator
Node *parse_indexing(Node *lhs) {
    expect(TK_LBRACKET);
    Node *rhs = parse_expr();
    expect(TK_RBRACKET);

    Node *n = new_node(ND_DEREF);
    n->expr = new_add(lhs, rhs);
    return n;
}

// parse some expressions which is separated comma,
// and return a linked list of expressions.
// ex. 20, 40, x, y + 20
Exprs parse_exprs(char *terminator) {
    Exprs result;

    Node head;
    Node *cur = calloc(1, sizeof(Node));
    head.next = cur;
    int count = 0;

    while (!cur_token_is(terminator)) {
        if (count++ > 0) {
            expect(TK_COMMA);
        }
        Node *tmp = parse_assign();
        cur->next = tmp;
        cur = tmp;
    }

    result.count = count;
    result.head = head.next->next;
    return result;
}

Node *adapt_casts(Node *exprs, Node *casts) {
    Node head;
    Node *cur = calloc(1, sizeof(Node));
    head.next = cur;

    Node *expr = exprs;
    Node *cast = casts;

    while (expr != NULL) {
        Node *tmp = new_cast_node(expr, cast->ty);
        cur->next = tmp;
        cur = tmp;

        expr = expr->next;
        cast = cast->next;
    }
    return head.next->next;
}

char *new_label(void) {
    static int count = 0;
    char *buf = malloc(20);
    sprintf(buf, ".L.data.%d", count++);
    return buf;
}

// primary = '(' expr ')'
//         | '(' '{' compound-stmt '}' ')'
//         | ident
//         | ident '(' arguments ')'
//         | constatnt
//         | string-literal
Node *parse_primary() {
    Node *n;
    if (token->tk == TK_LPARENT) {
        next_token();
        if (cur_token_is("{")) {
            next_token();
            n = new_node(ND_STMT_EXPR);
            n = parse_compound_stmt(n, 0);
            expect(TK_RBRACE);
        } else {
            n = parse_expr();
        }
        expect(TK_RPARENT);
    } else if (token->tk == TK_IDENT) {
        if (next_tokenkind_is(TK_LPARENT)) {
            // function call
            n = new_node(ND_CALL);
            FuncData *fd = new_func_data();
            fd->name = token->str;

            FuncData *defined = look_up_func(fd->name);

            expect(TK_IDENT);
            expect(TK_LPARENT);

            if (!cur_token_is(")")) {
                // function arguments
                Exprs result = parse_exprs(")");
                if (defined != NULL) {
                    // cast each arguments to each defined parameters type.
                    result.head = adapt_casts(result.head, defined->args);
                }
                fd->args = result.head;
                fd->args_num = result.count;
            }

            n->func = fd;

            // When callee function is defined in this compiler, solve the type of this node 'n'.
            if (defined != NULL) {
                n->ty = defined->return_type;
            }

            expect(TK_RPARENT);
        } else {
            // variable (it must be declared already)
            VarNode *v = get_var(token->str);
            if (v == NULL) {
                error("Undeclared variable: %s\n", token->str);
            }

            if (v->data.vk == ENUMMEMBER) {
                n = new_num_node(v->data.enum_val);
            } else {
                n = new_lvar_node(v->data);
            }
            next_token();
        }
    } else if (cur_tokenkind_is(TK_NUM)) {
        n = new_num_node(token->val);
        next_token();
    } else if (cur_tokenkind_is(TK_STR)) {
        // string literal
        Type *t = array_of(char_t, token->str_len);
        t->name = new_label();
        Var v = new_var(GLOBAL, t->name, t);
        v.str = token->str;
        v.str_len = token->str_len;

        register_new_gvar(v);

        n = new_lvar_node(v);
        next_token();
    } else {
        error("invalid primary");
    }
    return n;
}

// Convert x++ to `tmp = &A, *tmp = *tmp + 1, *tmp - 1`
// where tmp is a fresh pointer variable.
Node *new_inc_dec(Node *n, int imm) {
    add_type(n);
    Var v = new_var(LOCAL, "", pointer_to(n->ty));

    Node *expr1 = new_node_with_lr(
        ND_ASSIGN,
        new_lvar_node(v),
        new_unary_node(ND_ADDR, n)
    );

    Node *expr2 = new_node_with_lr(
        ND_ASSIGN,
        new_unary_node(ND_DEREF, new_lvar_node(v)),
        new_add(
            new_unary_node(ND_DEREF, new_lvar_node(v)),
            new_num_node(imm)
        )
    );

    Node *expr3 = new_add(
        new_unary_node(ND_DEREF, new_lvar_node(v)),
        new_num_node(-imm)
    );

    return new_node_with_lr(
        ND_COMMA,
        expr1,
        new_node_with_lr(
            ND_COMMA,
            expr2,
            expr3
        )
    );
}

// postfix = primary ('[' expr ']'
//                   | '.' ident
//                   | '->' ident
//                   | '++'
//                   | '--' )*
Node *parse_postfix() {
    Node *n = parse_primary();
    Node *new_n;

    while (1) {
        if (cur_token_is("[")) {
            n = parse_indexing(n);
            continue;
        } else if (cur_token_is("++")) {
            // post increment
            next_token();
            n = new_inc_dec(n, 1);
            continue;
        } else if (cur_token_is("--")) {
            // post decrement
            next_token();
            n = new_inc_dec(n, -1);
            continue;
        } else if (cur_token_is(".")) {
            // member access operator
            next_token();
            add_type(n);

            char *member_name = token->str;
            next_token();
            Member *m = get_member(n->ty->member, member_name);
            if (m == NULL) {
                error("unknown member specified: %s", member_name);
            }

            n = new_struct_member_node(n, m, member_name);
            continue;
        } else if (cur_token_is("->")) {
            next_token();
            add_type(n);

            char *member_name = token->str;
            next_token();
            Member *m = get_member(n->ty->base->member, member_name);
            if (m == NULL) {
                error("unknown member specified: %s", member_name);
            }

            Node *deref = new_node(ND_DEREF);
            deref->expr = n;

            n = new_struct_member_node(deref, m, member_name);
            continue;
        }

        break;
    }
    return n;
}

// unary = postfix
//       | '++' unary
//       | '--' unary
//       | ('&' | '*' | '+' | '-' | '!' | '~') cast
//       | 'sizeof' unary
//       | 'sizeof' '(' type-name ')'
Node *parse_unary() {
    Node *n;

    if (cur_token_is("+")) {
        next_token();
        n = parse_cast();
    } else if (cur_token_is("-")) {
        next_token();
        Node *lhs = new_node(ND_NUM);
        Node *rhs = parse_cast();
        n = new_node_with_lr(ND_SUB, lhs, rhs);
    } else if (cur_token_is("*")) {
        next_token();
        n = new_unary_node(ND_DEREF, parse_cast());
    } else if (cur_token_is("&")) {
        next_token();
        n = new_unary_node(ND_ADDR, parse_cast());
    } else if (cur_token_is("!")) {
        next_token();
        n = new_unary_node(ND_NOT, parse_cast());
    } else if (cur_token_is("~")) {
        next_token();
        n = new_unary_node(ND_BITNOT, parse_cast());
    } else if (cur_token_is("++")) {
        // pre increment
        next_token();
        Node *lhs = parse_unary();
        Node *add = new_add(lhs, new_num_node(1));
        n = new_node_with_lr(ND_ASSIGN, lhs, add);
    } else if (cur_token_is("--")) {
        // pre decrement
        next_token();
        Node *lhs = parse_unary();
        Node *add = new_sub(lhs, new_num_node(1));
        n = new_node_with_lr(ND_ASSIGN, lhs, add);
    } else if (cur_token_is("sizeof")) {
        next_token();
        Type *t;
        if (cur_token_is("(") && next_tokenkind_is(TK_TYPE)) {
            expect(TK_LPARENT);
            t = parse_typename(NULL);
            expect(TK_RPARENT);
        } else {
            Node *tmp = parse_unary();
            add_type(tmp);
            t = tmp->ty;
        }
        n = new_num_node(t->size);
    } else {
        n = parse_postfix();
    }

    return n;
}

// cast = unary
//      | '(' type-name ')' cast
Node *parse_cast() {
    if (cur_token_is("(") && next_tokenkind_is(TK_TYPE)) {

        expect(TK_LPARENT);
        Type *t = parse_typename(NULL);
        expect(TK_RPARENT);

        Node *cast_node = new_cast_node(parse_cast(), t);
        return cast_node;
    }
    return parse_unary();
}

// mul = cast ('*' cast | '/' cast | '%' cast)*
Node *parse_mul() {
    Node *lhs = parse_cast();

    while (cur_token_is("*") || cur_token_is("/") || cur_token_is("%")) {
        if (cur_token_is("*")) {
            next_token();
            lhs = new_node_with_lr(ND_MUL, lhs, parse_cast());
        } else if (cur_token_is("/")) {
            next_token();
            lhs = new_node_with_lr(ND_DIV, lhs, parse_cast());
        } else {
            next_token();
            lhs = new_node_with_lr(ND_REM, lhs, parse_cast());
        }
    }

    return lhs;
}

Node *new_add(Node *lhs, Node *rhs) {
    add_type(lhs);
    add_type(rhs);

    // num + num
    if (is_integer(lhs->ty) && is_integer(rhs->ty)) {
        return new_node_with_lr(ND_ADD, lhs, rhs);
    }

    // Canonicalize `num + ptr` to `ptr + num`.
    if (!is_pointer(lhs->ty) && is_pointer(rhs->ty)) {
        Node *tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }

    // ptr + num
    rhs = new_node_with_lr(ND_MUL, rhs, new_num_node(lhs->ty->base->size));
    return new_node_with_lr(ND_ADD, lhs, rhs);
}

Node *new_sub(Node *lhs, Node *rhs) {
    add_type(lhs);
    add_type(rhs);

    // num - num
    if (is_integer(lhs->ty) && is_integer(rhs->ty)) {
        return new_node_with_lr(ND_SUB, lhs, rhs);
    }

    // ptr - num
    if (is_pointer(lhs->ty) && !is_pointer(rhs->ty)) {
        rhs = new_node_with_lr(ND_MUL, rhs, new_num_node(lhs->ty->base->size));
        return new_node_with_lr(ND_SUB, lhs, rhs);
    }

    // ptr - ptr
    if (is_pointer(lhs->ty) && is_pointer(rhs->ty)) {
        Node *sub = new_node_with_lr(ND_SUB, lhs, rhs);
        return new_node_with_lr(ND_DIV, sub, new_num_node(lhs->ty->base->size));
    }

    error("invalid - operation");
    return NULL;
}

// add = mul ('+' mul | '-' mul)*
Node *parse_add() {
    Node *lhs = parse_mul();

    while (cur_token_is("+") || cur_token_is("-")) {
        if (cur_token_is("+")) {
            next_token();
            Node *rhs = parse_mul();
            lhs = new_add(lhs, rhs);
        } else {
            next_token();
            Node *rhs = parse_mul();
            lhs = new_sub(lhs, rhs);
        }
    }
    
    return lhs;
}

// shift = add ('<<' add | '>>' add)*
Node *parse_shift() {
    Node *lhs = parse_add();

    while (cur_token_is("<<") || cur_token_is(">>")) {
        if (cur_token_is("<<")) {
            next_token();
            lhs = new_node_with_lr(ND_LSHIFT, lhs, parse_add());
        } else {
            next_token();
            lhs = new_node_with_lr(ND_RSHIFT, lhs, parse_add());
        }
    }

    return lhs;
}

// relational = shift ('<' shift | '<=' shift | '>' shift | '>=' shift)*
Node *parse_relational() {
    Node *lhs = parse_shift();

    while (cur_token_is("<") || cur_token_is("<=")
        || cur_token_is(">") || cur_token_is(">=")) {

        if (cur_token_is("<")) {
            next_token();
            lhs = new_node_with_lr(ND_LT, lhs, parse_shift());
        } else if (cur_token_is("<=")) {
            next_token();
            lhs = new_node_with_lr(ND_LE, lhs, parse_shift());
        } else if (cur_token_is(">")) {
            next_token();
            lhs = new_node_with_lr(ND_LT, parse_shift(), lhs);
        } else {
            next_token();
            lhs = new_node_with_lr(ND_LE, parse_shift(), lhs);
        }
    }

    return lhs;
}

// equality = relational ('==' relational | '!=' relational)*
Node *parse_equality() {
    Node *lhs = parse_relational();

    while (cur_token_is("==") || cur_token_is("!=")) {
        if (cur_token_is("==")) {
            next_token();
            lhs = new_node_with_lr(ND_EQ, lhs, parse_relational());
        } else {
            next_token();
            lhs = new_node_with_lr(ND_NE, lhs, parse_relational());
        }
    }

    return lhs;
}

// bitand = equality ('&' equality)*
Node *parse_bitand() {
    Node *lhs = parse_equality();

    while (cur_token_is("&")) {
        next_token();
        lhs = new_node_with_lr(ND_BITAND, lhs, parse_equality());
    }

    return lhs;
}

// bitxor = bitand ('^' bitand)*
Node *parse_bitxor() {
    Node *lhs = parse_bitand();

    while (cur_token_is("^")) {
        next_token();
        lhs = new_node_with_lr(ND_BITXOR, lhs, parse_bitand());
    }

    return lhs;
}

// bitor = bitxor ('|' bitxor)*
Node *parse_bitor() {
    Node *lhs = parse_bitxor();

    while (cur_token_is("|")) {
        next_token();
        lhs = new_node_with_lr(ND_BITOR, lhs, parse_bitxor());
    }

    return lhs;
}

// logand = bitor ('&&' bitor)*
Node *parse_logand() {
    Node *lhs = parse_bitor();

    while (cur_token_is("&&")) {
        next_token();
        lhs = new_node_with_lr(ND_LOGAND, lhs, parse_bitor());
    }

    return lhs;
}

// logor = logand ('||' logand)*
Node *parse_logor() {
    Node *lhs = parse_logand();

    while (cur_token_is("||")) {
        next_token();
        lhs = new_node_with_lr(ND_LOGOR, lhs, parse_logand());
    }

    return lhs;
}

// cond = logor
//      | logor ? expr : cond
Node *parse_cond() {
    Node *n = parse_logor();
    if (!cur_token_is("?")) {
        return n;
    }

    expect(TK_QUEST);
    Node *cond = new_node(ND_COND);
    cond->cond = n;

    cond->then = parse_expr();
    expect(TK_COLON);
    cond->alt = parse_cond();

    return cond;
}

// assign = cond ( '='   assign
//               | '+='  assign
//               | '-='  assign
//               | '*='  assign
//               | '/='  assign
//               | '%='  assign
//               | '<<=' assign
//               | '>>=' assign
//               | '&='  assign
//               | '|='  assign
//               | '^='  assign )*
Node *parse_assign() {
    Node *lhs = parse_cond();

    if (cur_token_is("=")) {
        next_token();
        lhs = new_node_with_lr(ND_ASSIGN, lhs, parse_assign());
    } else if (cur_token_is("+=")) {
        next_token();
        Node *add = new_node_with_lr(ND_ADD, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, add);
    } else if (cur_token_is("-=")) {
        next_token();
        Node *sub = new_node_with_lr(ND_SUB, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, sub);
    } else if (cur_token_is("*=")) {
        next_token();
        Node *mul = new_node_with_lr(ND_MUL, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, mul);
    } else if (cur_token_is("/=")) {
        next_token();
        Node *div = new_node_with_lr(ND_DIV, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, div);
    } else if (cur_token_is("%=")) {
        next_token();
        Node *rem = new_node_with_lr(ND_REM, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, rem);
    } else if (cur_token_is("<<=")) {
        next_token();
        Node *lshift = new_node_with_lr(ND_LSHIFT, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, lshift);
    } else if (cur_token_is(">>=")) {
        next_token();
        Node *rshift = new_node_with_lr(ND_RSHIFT, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, rshift);
    } else if (cur_token_is("&=")) {
        next_token();
        Node *bitand = new_node_with_lr(ND_BITAND, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, bitand);
    } else if (cur_token_is("|=")) {
        next_token();
        Node *bitor = new_node_with_lr(ND_BITOR, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, bitor);
    } else if (cur_token_is("^=")) {
        next_token();
        Node *bitxor = new_node_with_lr(ND_BITXOR, lhs, parse_equality());
        lhs = new_node_with_lr(ND_ASSIGN, lhs, bitxor);
    }

    return lhs;
}

// expr = assign (',' expr)?
Node *parse_expr() {
    Node *n = parse_assign();

    if (cur_token_is(",")) {
        next_token();
        return new_node_with_lr(ND_COMMA, n, parse_expr());
    }

    return n;
}

long eval(Node *n) {
    add_type(n);

    if (n->nk == ND_ADD) {
        return eval(n->lhs) + eval(n->rhs);
    } else if (n->nk == ND_SUB) {
        return eval(n->lhs) - eval(n->rhs);
    } else if (n->nk == ND_MUL) {
        return eval(n->lhs) * eval(n->rhs);
    } else if (n->nk == ND_DIV) {
        return eval(n->lhs) / eval(n->rhs);
    } else if (n->nk == ND_REM) {
        return eval(n->lhs) % eval(n->rhs);
    } else if (n->nk == ND_BITAND) {
        return eval(n->lhs) & eval(n->rhs);
    } else if (n->nk == ND_BITOR) {
        return eval(n->lhs) | eval(n->rhs);
    } else if (n->nk == ND_BITXOR) {
        return eval(n->lhs) ^ eval(n->rhs);
    } else if (n->nk == ND_BITNOT) {
        return ~eval(n->expr);
    } else if (n->nk == ND_LSHIFT) {
        return eval(n->lhs) << eval(n->rhs);
    } else if (n->nk == ND_RSHIFT) {
        return eval(n->lhs) >> eval(n->rhs);
    } else if (n->nk == ND_EQ) {
        return eval(n->lhs) == eval(n->rhs);
    } else if (n->nk == ND_NE) {
        return eval(n->lhs) != eval(n->rhs);
    } else if (n->nk == ND_LT) {
        return eval(n->lhs) < eval(n->rhs);
    } else if (n->nk == ND_LE) {
        return eval(n->lhs) <= eval(n->rhs);
    } else if (n->nk == ND_COND) {
        return eval(n->cond) ? eval(n->then) : eval(n->alt);
    } else if (n->nk == ND_COMMA) {
        return eval(n->rhs);
    } else if (n->nk == ND_NOT) {
        return !eval(n->expr);
    } else if (n->nk == ND_LOGAND) {
        return eval(n->lhs) && eval(n->rhs);
    } else if (n->nk == ND_LOGOR) {
        return eval(n->lhs) || eval(n->rhs);
    } else if (n->nk == ND_CAST) {
        if (is_integer(n->ty)) {
            if (n->ty->size == 1) {
                return (char)eval(n->expr);
            } else if (n->ty->size == 2) {
                return (short)eval(n->expr);
            } else if (n->ty->size == 4) {
                return (int)eval(n->expr);
            }
        }
        return eval(n->expr);
    }

    if (n->nk != ND_NUM) {
        error("not a constant expression");
    }
    return n->val;
}

long const_expr() {
    Node *n = parse_cond();
    return eval(n);
}

// parse stmt, stmt, ... "}"
// if this function is called from top level function, not enter the scope.
Node *parse_compound_stmt(Node *n, int from_func_body) {
    if (!from_func_body) {
        enter_scope();
    }
    if (!cur_token_is("}")) {
        Node head;
        Node *cur = calloc(1, sizeof(Node));
        head.next = cur;

        while (!cur_token_is("}")) {
            Node *tmp = parse_stmt();
            cur->next = tmp;
            cur = tmp;
        }
        n->block = head.next->next;
    }
    if (!from_func_body) {
        leave_scope();
    }
    return n;
}

// stmt_sub = 'return' expr ';'
//          | 'if' '(' expr ')' stmt ('else' stmt)?
//          | 'while' '(' expr ')' stmt
//          | 'for' '(' stmt? ';' expr ';' expr ')' stmt
//          | 'break' ';'
//          | 'continue' ';'
//          | 'switch' '(' expr ')' stmt
//          | 'case' const-expr ':' stmt
//          | 'default' ':' stmt
//          | 'goto' ident ';'
//          | ident ':' stmt
//          | '{' stmt '}'
//          | declaration
//          | expr ';'
Node *parse_stmt_sub() {
    Node *n;
    if (cur_token_is("return")) {
        n = new_node(ND_RETURN);
        next_token();
        Node *expr = parse_expr();
        expect(TK_SEMICOLON);
        n->expr = new_cast_node(expr, cur_function_return_type);
        return n;
    } else if (cur_token_is("break")) {
        n = new_node(ND_BREAK);
        next_token();
        expect(TK_SEMICOLON);
        return n;
    } else if (cur_token_is("continue")) {
        n = new_node(ND_CONTINUE);
        next_token();
        expect(TK_SEMICOLON);
        return n;
    } else if (cur_token_is("if")) {
        n = new_node(ND_IF);
        next_token();
        expect(TK_LPARENT);
        n->cond = parse_expr();
        expect(TK_RPARENT);
        n->then = parse_stmt();
        if (cur_token_is("else")) {
            next_token();
            n->alt = parse_stmt();
        }
        return n;
    } else if (cur_token_is("while")) {
        n = new_node(ND_WHILE);
        next_token();
        expect(TK_LPARENT);
        n->cond = parse_expr();
        expect(TK_RPARENT);
        n->then = parse_stmt();
        return n;
    } else if (cur_token_is("for")) {
        n = new_node(ND_FOR);
        next_token();
        expect(TK_LPARENT);

        // for statement allow to declare a new local variable.
        enter_scope();

        if (!cur_tokenkind_is(TK_SEMICOLON)) {
            n->init = parse_stmt();
        } else {
            expect(TK_SEMICOLON);
        }

        if (!cur_tokenkind_is(TK_SEMICOLON)) {
            n->cond = parse_expr();
        }
        expect(TK_SEMICOLON);
        if (!cur_tokenkind_is(TK_RPARENT)) {
            n->post = parse_expr();
        }
        expect(TK_RPARENT);
        n->then = parse_stmt();

        leave_scope();

        return n;
    } else if (cur_token_is("switch")) {
        n = new_node(ND_SWITCH);
        next_token();
        expect(TK_LPARENT);
        n->cond = parse_expr();
        expect(TK_RPARENT);

        Node *sw = cur_switch;
        cur_switch = n;
        n->then = parse_stmt();
        cur_switch = sw;
        return n;
    } else if (cur_token_is("case")) {
        n = new_node(ND_CASE);
        next_token();
        int val = const_expr();

        expect(TK_COLON);
        n->stmt = parse_stmt();
        n->val = val;
        n->case_next = cur_switch->case_next;
        cur_switch->case_next = n;
        return n;
    } else if (cur_token_is("default")) {
        n = new_node(ND_CASE);
        next_token();
        expect(TK_COLON);
        n->stmt = parse_stmt();
        cur_switch->default_case = n;
        return n;
    } else if (cur_token_is("goto")) {
        n = new_node(ND_GOTO);
        next_token();
        n->label_name = token->str;
        next_token();
        expect(TK_SEMICOLON);
        return n;
    } else if (cur_tokenkind_is(TK_IDENT) && next_tokenkind_is(TK_COLON)) {
        n = new_node(ND_LABEL);
        n->label_name = token->str;
        next_token();
        expect(TK_COLON);
        n->stmt = parse_stmt();
        return n;
    } else if (cur_token_is("{")) {
        n = new_node(ND_BLOCK);
        next_token();
        n = parse_compound_stmt(n, 0);
        expect(TK_RBRACE);
        return n;
    } else if (cur_tokenkind_is(TK_TYPE)
          || cur_tokenkind_is(TK_STORAGE)
          || is_defined_type(token->str)) {
        return parse_declaration(LOCAL);
    }

    n = parse_expr();
    expect(TK_SEMICOLON);
    return n;
}

Node *parse_stmt() {
    Node *n = parse_stmt_sub();
    add_type(n);
    return n;
}

void init_function_context(Type *ret_t) {
    toplevel_scope = NULL;
    cur_scope = NULL;
    scope_depth = 0;
    stack_frame_size = 0;
    cur_function_return_type = ret_t;
}

void parse_parameters(FuncData *f) {
    int args_num = 0;
    Node head;
    Node *cur = calloc(1, sizeof(Node));
    head.next = cur;
    while (!cur_token_is(")")) {
        if (cur != head.next) {
            expect(TK_COMMA);
        }

        Type *t = parse_typename(NULL);

        // "array of T" is converted to "pointer to T" only in the
        // parameter context. For exmaple, *argv[] is converted to **argv.
        if (t->tk == ARRAY) {
            char *name = t->name;
            t = pointer_to(t->base);
            t->name = name;
        }

        Var v = new_var(ARG, t->name, t);
        register_new_lvar(v);

        Node *arg = new_node(ND_DECL);
        arg->ty = t;
        arg->var = v;

        cur->next = arg;
        cur = arg;
        args_num++;
    }
    f->args_num = args_num;
    f->args = head.next->next;
}

FuncData *parse_toplevel_func(Type *ret_t, bool is_static) {
    init_function_context(ret_t);
    enter_scope();

    FuncData *func_data = new_func_data();

    func_data->return_type = ret_t;
    func_data->name = ret_t->name;
    func_data->is_static = is_static;

    expect(TK_LPARENT);
    if (!cur_token_is(")")) {
        // function parameters
        parse_parameters(func_data);
    }
    expect(TK_RPARENT);

    // function declaration (Not definition)
    if (cur_token_is(";")) {
        expect(TK_SEMICOLON);
        leave_scope();
        return func_data;
    }

    // parse function body
    Node *body = new_node(ND_BLOCK);
    expect(TK_LBRACE);
    func_data->body = parse_compound_stmt(body, 1);
    expect(TK_RBRACE);

    // store the head of linked list which include local variables.
    // func_data.vars = cur_scope->lvar_head;
    func_data->toplevel_scope = toplevel_scope;
    func_data->stack_frame_size = stack_frame_size;

    leave_scope();
    return func_data;
}

void parse_toplevel_global_var(Type *t) {
    Node *n = parse_declaration(GLOBAL);
}

// parse type and identifier, and switch parsing if token is '(' or not.
void parse_toplevel() {
    Token *tok_org = token;
    VarAttr attr = {};
    Type *t = parse_typename(&attr);

    if (cur_token_is("(")) {
        // top level function definition
        funcs[func_count++] = parse_toplevel_func(t, attr.is_static);
    } else {
        // global variable declaration
        token = tok_org;
        parse_toplevel_global_var(t);
    }
}

void parse_program() {
    while (token->tk != TK_EOF) {
        parse_toplevel();
    }
    funcs[func_count] = NULL;
}

Program *parse(struct Token *t) {
    token = t;
    parse_program();
    Program *p = calloc(1, sizeof(Program));
    p->funcs = funcs;
    p->gvars = gvar_head;
    return p;
}
