#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;
typedef struct Node Node;

//
// tokenizer.c
//

// Token
typedef enum {
  TK_IDENT,   // Identifiers
  TK_PUNCT,   // Punctuators
  TK_NUM,     // Numeric literals
  TK_KEYWORD, // Keywords
  TK_EOF,     // End-of-file markers
} TokenKind;

// Variable or function
typedef struct Obj Obj;
struct Obj {
  Obj *next;
  char *name;     // Variable name
  Type *ty;       // Type
  bool is_local;  // local or global/function

  // Local variable
  int offset;

  // Global variable or function
  bool is_function;

  // Function
  Obj *params;
  Node *body;
  Obj *locals;
  int stack_size;
};

// Token type
typedef struct Token Token;
struct Token {
  TokenKind kind; // Token kind
  Token *next;    // Next token
  int val;        // If kind is TK_NUM, its value
  char *loc;      // Token location
  int len;        // Token length
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
bool consume(Token **rest, Token *tok, char *str);
Token *tokenize(char *input);

//
// parser.c
//

// AST node
typedef enum {
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_NEG,       // unary -
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_ADDR,      // unary &
  ND_DEREF,     // unary *
  ND_RETURN,    // "return"
  ND_IF,        // "if"
  ND_FOR,       // "for" or "while"
  ND_BLOCK,     // { ... }
  ND_FUNCALL,   // Function call
  ND_EXPR_STMT, // Expression statement
  ND_VAR,       // Variable
  ND_NUM,       // Integer
} NodeKind;

// AST node type
struct Node {
  NodeKind kind;  // Node Kind
  Node *next;     // Next node
  Type *ty;       // Type, e.g. int or pointer to int
  Token *tok;     // Representative token
  Node *lhs;      // Left-hand side
  Node *rhs;      // Right-hand side
  Node *body;     // Block

  // Function call
  char *funcname;
  Node *args;

  // "if" or "for" statement
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;

  Obj *var;       // Used if kind == ND_VAR
  int val;        // Used if kind == ND_NUM
};

Obj *parse(Token *tok);

//
// type.c
//
typedef enum {
  TY_CHAR,
  TY_INT,
  TY_PTR,
  TY_FUNC,
  TY_ARRAY,
} TypeKind;

struct Type {
  TypeKind kind;

  int size; // sizeof() value

  // Pointer-to or array-of type. We intentionally use the same member
  // to represent pointer/array duality in C.
  //
  // In many contexts in which a pointer is expected, we examine this
  // member instead of "kind" member to determine wheter a type is a
  // pointer or not. That means in many contexts "array of T" is
  // naturally handled as if it were "pointer to T", as required by
  // the C spec.
  Type *base;

  // Declaration
  Token *name;

  // Array
  int array_len;

  // Function Type
  Type *return_ty;
  Type *params;
  Type *next;
};

extern Type *ty_int;
extern Type *ty_char;

bool is_integer(Type *ty);
Type *copy_type(Type *ty);
Type *pointer_to(Type *base);
void add_type(Node *node);
Type *func_type(Type *return_ty);
Type *array_of(Type *base, int size);

//
// codegen.c
//

void codegen(Obj *prog);
