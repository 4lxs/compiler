grammar X;
options { tokenVocab=XLexer; }

program
    : moddef (function | staticDef | structDef)+ EOF
    ;

// allow a; b; 5+3
// jitLine
//     : stmt* (expr | varDef | typeDef)? (EOF | Eol)
//     ;

moddef
    : Module initSep=ClnCln? (Ident ClnCln)* Ident? Sc
    ;

//===
// statements
//===

staticDef
    : Let name=Ident Cln path (Eq val=expr)? Sc
    ;

varDef
    : Let name=Ident Cln type=path (Eq val=expr)? Sc
    ;

varAssign
    : primaryExpr Eq expr;

structDef
    : Struct name=Ident LPrn (structField (Cma structField)*) RPrn
    ;

structField
    : name=Ident Cln path (Eq value=expr)?
    ;

function
    : Fn name=Ident params (To ret=path)? body=block
    ;

params
    : LPrn (Ident Cln path (Cma Ident Cln path)*)? RPrn;

block
    : LBc stmt* terminator=expr? RBc
    ;

stmt
    : if_
    | block
    | expr Sc
    | varDef
    | varAssign
    | structDef
    | return
    | while
    // | function
    | Sc
    ;

return
    : Return expr? Sc
    ;

while
    : While expr block
    ;

if_
    : If expr then=block (Else else_=block)?
    ;

// expressions that can appear at the start of member chain
// primaryExpr.a.b.c()
primaryExpr
    : primaryExpr Dot memberAccess #memberE
    | if_ #ifE
    | block #blockE
    // primary values
    | IntegerLiteral #intPE
    | String #strPE
    | (True | False) #boolPE

    | fn=path arg=structExpr #callE
    | Ident #varE
    | structExpr #structE
    ;

// ().x
// ().x()
// if {} else {}.x()
// ().x.y().z
// if {} else {}.x()

memberAccess
    : field=Ident args=structExpr?
    ;

expr
    : primaryExpr #primaryE
	| left=expr bop=( Star | Slash ) right=expr #binaryE
	| left=expr bop=( Plus | Minus ) right=expr #binaryE
	| left=expr bop=( Greater | Less ) right=expr #binaryE
    ;

// (a=b, b), (c, d)
// structs, tuples, arguments
structExpr
    : LPrn (structExprField (Cma structExprField)* Cma?)? RPrn
    ;

structExprField
    : (Ident Eq)? expr
    ;

//===
// Types
//===

// type
//     : path #bareTy
//     | LPrn RPrn #unitTy
//     | LPrn (Ident Cln type (Cma Ident Cln type)*)? RPrn #structTy
//     | LBk type Sc IntegerLiteral RBk #sliceTy
//     | LPrn type (Cma type)* RPrn #tupleTy
//     ;

//===
// Path
//===

path: initSep=ClnCln? (Ident ClnCln)* Ident;

//===
// Lexer
//===

fragment BlEnd: Sc Sc;

Comment: '//' .*? Eol -> skip;

Let: 'let';
Module: 'module';
Struct: 'struct';
Fn: 'fn';
If: 'if';
Else: 'else';
True: 'true';
False: 'false';
Return: 'return';
While: 'while';

Eq: '=';
EqEq: '==';

Cma: ',';
Dot: '.';
ClnCln: '::';
Cln: ':';
Sc: ';';
To: '->';
Qt: '\'';
String: '"' (~["\\\r\n])+ '"';

Star: '*';
Slash: '/';
Plus: '+';
Minus: '-';
Greater: '>';
Less: '<';

LPrn: '(';
RPrn: ')';
LBc: '{';
RBc: '}';
LBk: '[';
RBk: ']';

IntegerLiteral: [0-9]+;

Eol: ('\r\n' | [\n\r]) -> skip;
Whitespace: [ \t] -> skip;

Ident: [a-zA-Z_][a-zA-Z0-9_]*;

// if we get this, it means we're not handling everything
// Any: .;