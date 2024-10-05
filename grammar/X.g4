grammar X;
options { tokenVocab=XLexer; }

program
    : moddef (function | structDef | enumDef | typeDef)+ EOF
    ;

// allow a; b; 5+3
// jitLine
//     : stmt* (expr | varDef | typeDef)? (EOF | Eol)
//     ;

moddef
    : Module initSep=Dot? (Ident Dot)* Ident? Sc
    ;

//===
// statements
//===

typeDef
    : Type name=Ident Eq path Sc
    ;

enumDef
    : Enum name=Ident LBc (enumVariant (Cma enumVariant)* Cma?)? RBc
    ;

enumVariant
    : name=Ident (Cln type)?
    ;

varDef
    : Let name=Ident Cln typ=path (Eq val=expr)? Sc
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
    : Fn (class=path Dot)? name=Ident params (To ret=path)? body=block
    ;

param
    : Ident Cln path
    ;

params
    : LPrn ((self=Ident | param) (Cma param)*)? RPrn;

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
    : if_ #ifE
    | block #blockE
    // primary values
    | IntegerLiteral #intPE
    | String #strPE
    | (True | False) #boolPE

    | fn=primaryExpr args=structExpr #callE
    | Ident #varE
    | primaryExpr Dot Ident #memberE
    | structExpr #structE
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

type
    : path #bareTy
    | LPrn RPrn #unitTy
    | LPrn (Ident Cln type (Cma Ident Cln type)*)? RPrn #structTy
    // | LBk type Sc IntegerLiteral RBk #sliceTy
    | LPrn type (Cma type)* RPrn #tupleTy
    ;

//===
// Path
//===

path: initSep=Dot? (Ident Dot)* Ident;

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
Enum: 'enum';
Type: 'type';

Eq: '=';
EqEq: '==';

Cma: ',';
Dot: '.';
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