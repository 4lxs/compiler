grammar X;
options { tokenVocab=XLexer; }

program
    : moddef (function | staticDef | typeDef)+ EOF
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
    : Let name=Ident Cln type=path? (Eq val=expr)?
    ;

typeDef
    : Type name=Ident Eq val=path Sc
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
    | expr Sc
    | varDef Sc
    | typeDef Sc
    | return
    // | function
    | Sc
    ;

return
    : Return expr? Sc
    ;

if_
    : If expr then=block (Else else_=block)?
    ;

expr
    : if_ #ifE
    | path #varE
    // primary expressions
    | IntegerLiteral #intPE
    | String #strPE
    | (True | False) #boolPE

    | fn=path arg=anonStruct #callE
    | name=path #varE
    | anonStruct #structE
	| left=expr bop=( Star | Slash ) right=expr #binaryE
	| left=expr bop=( Plus | Minus ) right=expr #binaryE
	| left=expr bop=( Greater | Less ) right=expr #binaryE
    ;

anonStruct
    : LPrn (Ident Eq expr (Cma Ident Eq expr)*)? RPrn
    ;

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
Type: 'type';
Fn: 'fn';
If: 'if';
Else: 'else';
True: 'true';
False: 'false';
Return: 'return';

Eq: '=';
EqEq: '==';

Cma: ',';
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