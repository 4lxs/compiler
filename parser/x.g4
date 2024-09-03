grammar x;

program
    : moddef tlstmt+ EOF;

moddef
    : 'module' PATH ';';

//---
// statements
//---

tlstmt
    : fnStmt
    | varDef
    | typeDef;

varDef
    : 'let' name_with_type '=' expr ';';

typeDef
    : 'type' name '=' type ';';

fnStmt
    : 'fn' name name_with_type '->' type block;

name
    : IDENT;

block
    : '{' stmt* expr '}';

stmt
    : expr ';'
    | ifExpr
    | varDef
    | typeDef;

ifExpr
    : 'if' expr block ('else' block)?;

// expressions are set by precedence expr1, expr2, expr3...
// lower number means lower precedence
expr
    : ifExpr #conditional
    | primitive #primary
    | '(' expr ')' #grouping
	| left=expr bop=( '*' | '/' ) right=expr #multiplication
	| left=expr bop=( '+' | '-' ) right=expr #addition
	| left=expr bop=( '>' | '<' ) right=expr #comparison
    ;

primitive
    : NUMBER
    ;

//---
// types
//---

name_with_type
    : typed_name
    | typed_struct;

type
    : IDENT
    | struct_type;

typed_name
    : name ':' type;

typed_struct
    : '(' (typed_name ',')* typed_name? ')';

struct_type
    : '(' (type ',')* type? ')';

//---
// lexer
//---

IF: 'if';
ELSE: 'else';
FN: 'fn';
LET: 'let';
MODULE: 'module';
TYPE: 'type';
PATH: [a-z]*('::'[a-z]+)+;

PLUS: '+';
MINUS: '-';
STAR: '*';
SLASH: '/';
EQEQ: '==';
NEQ: '!=';
LESS: '<';
GREATER: '>';
LEQ: '<=';
GEQ: '>=';

LBRACE: '{';
RBRACE: '}';
LPAREN: '(';
RPAREN: ')';

SEMICOLON: ';';
COLON: ':';
COMMA: ',';
TO: '->';
EQ: '=';

NUMBER: [0-9]+;
IDENT: [a-zA-Z_][a-zA-Z0-9_]*;

WHITESPACE: [ \n] -> skip;

// if we get this, it means we're not handling everything
ANY: .;