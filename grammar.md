#  Language Grammar (Backus–Naur form)

### Program structure
```
<Program> ::= {<Directive>} {  <Function definition> | <Struct definition> | <Variable creation> } main() ‘{‘ <Operator> ‘}’

<Directive> ::= import "<String>"; | define <String> <String>;
```
### Functions
```
<Function definition> ::= fun <Name> <Function parameters> <Type>  ‘{‘ <Operator> return <Expression>‘}’

<Function parameters> ::= "(" <Type> <Name> {, <Type> <Name> } ")"

<Function call> ::= <Name><Argument list>

<Argument list> ::= "("<Name> {, <Name> }")"
```
### Structures
```
<Struct definition> ::= struct <Name>  ‘{‘ { <Function definition> | <Variable creation> } ‘}’ <Name> {, <Name>};

<Struct member> ::= <Name>.(<Name>|<Function call>)
```
### Types and variables
```
<String> ::= <Symbol>{<Symbol>}

<Symbol> ::= <Letter> | <Digit>

<Name> ::= <Letter>{<Letter>|<Digit>}

<Variable>::= <Name> | <Array Indexation>

<Letter> ::= [a-z] | [A-Z] | <Special symbol>

<Digit> ::= [0-9]

<Special symbol> ::=  _ | /

<Type> ::= void | int | double | bool

<Variable Creation> ::= <Variable Definition> | <Variable Initialization>

<Variable Definition> ::= <Type> <Name> {, <Name>};

<Variable Initialization> ::= <Type> {<Name> = <Expression>, } <Name> = <Expression>;

<Sign> ::= + | -

<Signed number> ::= <Sign><Digit>{<Digit>}|<Unsigned number>

<Unsigned number> ::= <Digit> {<Digit>}
```
### Expression
```
<Expression>::= <Expression enum>

<Expressin enum>::= <Expression term> {, <Expression term> }

<Expression terms>::= <Arithmetic expression> | <Comparison> | <Function call> | <Assignment> | <Variable> | <Struct member>

<Expression operator> ::= <Expression>;
```
### Arithmetic and logic
```
<Arithmetic expression> ::= <Unary term>

<Unary> ::= + | - | ++ | -- | ~

<Mul> ::= *

<Sum> ::= +

<Power> ::= **

<And> ::= & | and

<Xor> ::= ^

<Or> ::= '|' | or

<Shift> ::= << | >>

<Unary term> ::= <Unary><Unary term> | <Power term>

<Power term> ::= <Mul term>{<Power><Mul term>}

<Mul term> ::= <Sum term>{<Mul><Sum term>}

<Sum term> ::= <Shift term>{<Sum><Shift term>}

<Shift term> ::= <And term>{<Shift><And term>}

<And term> ::= <Xor term>{<And><Xor term>}

<Xor term> ::= <Or term>{<Xor><Or term>}

<Or term> ::= <Arithmetic term>{<Or><Arithmetic term>}

<Arithmetic term> ::= <Arithmetic literal> | <Variable> | <Boolean literal>

<Arithmetic literal> ::= <Signed number> | <Signed number>.<Unsigned number>

<Boolean literal> ::= false | true
```
### Assignment
```
<Assignment> ::= <Variable> <Assignment operator> <Expression>

<Assignment operator> ::= = | <<= | >>= | += | -= | *= | **= | /= | //= | ^= | &= | |= | %=
```
### Comparison
```
<Comparison> ::= <Expression> <Comparison operators> <Expression>

<Comparison operators> ::= > | < | >= | <= | == | != 
```
### Operator
```
<Operator>::= ‘{‘ { <Operator> } ‘}’ | <If> | <For> | <While> | <Input> | <Output> | <Expression operator> | <Variable creation> | <Match>
```
### Conditional operator
```
<If> :: if (<Expression>) <Operator> [ else <Operator> ]
```
### Cycles
```
<For>::= for (<Variable Initialization>; <Expression>; <Expression>) <Operator>

<While>::= while ‘(‘ <Expression> ‘)’ do <Operator>
```
### Input and Output
```
//<Input>::= input { <Variable>, } <Variable>;
<Input> ::= input <Variable> {, <Variable>}; 

//<Output>::= output {<Expression>,} <Expression>;
<Output> ::= output <Expression> {, <Expression>};
```
### Arrays
```
<Array declaration>::= <Array declaration exact> | <Array declaration auto>

<Array declaration exact> ::= <Type> <Name>”[“<Expression>”]” [ = “{“ <Literal> {, <Literal>} "}"];

<Array declaration auto> ::= <Type> <Name> “[]” = “{“ <Literal> {, <Literal> } "}";

<Array indexation> ::= <Name>[<Expression>]

<Literal>::= <Arithmetic literal> | <boolean literal>
```
### Match
```
<Match> ::= match <Name> ‘{‘ {<Expression> => <Operator>} ‘}’
```

