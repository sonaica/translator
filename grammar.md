#  Language Grammar (Backus–Naur form)

### Program structure
```
<Program> ::= {<Preprocessing>} { <Global statement> } fun main() ‘{‘ <Operator> ‘}’

<Preprocessing> ::= <Directive> 

<Directive> ::= import "<Library>"; | define <Definition>;

<Library> ::= <String>

<Definition> ::= <String> <String>

<Global statement> ::= <Function definition> | <Struct definition> | <Variable Creation>
```
### Functions
```
<Function definition> ::= fun <Name> “(“ <Function parameters> “)” <Type>  ‘{‘ <Operator> return <Expression>‘}’

<Function parameters> ::= <Type> <Name> | <Type> <Name>, <Function parameters>

<Function call> ::= <Name>(<Argument list>)

<Argument list> ::= <Name> {, <Name> }
```
### Structures
```
<Struct definition> ::= struct <Name>  ‘{‘ <Function definition> | <Variable Creation> ‘}’ { <Name>, } <Name>;

<Struct member> ::= <Name>.(<Name>|<Function call>)
```
### Types and variables
```
<String> ::= <Symbol> | <Symbol><String>

<Symbol> ::= <Letter> | <Digit>

<Name> ::= <Letter> | <Name><Letter> | <Name><Digit>

<Variable>::= <Name> | <Array Indexation>

<Letter> ::= [a-z] | [A-Z] | <Special symbol>

<Special symbol> ::=  _ | /

<Type> ::= void | int | double | bool

<Variable Creation> ::= <Variable Definition> | <Variable Initialization>

<Variable Definition> ::= <Type> {<Name>, } <Name>;

<Variable Initialization> ::= <Type> {<Name> ::= <Expression>, } <Name> ::= <Expression>;

<Sign> ::= + | -

<Signed number> ::= <Sign><Digit>{<Digit>}|<Unsigned number>

<Unsigned number> ::= <Digit> {<Digit>}
```
### Expression
```
<Expression> ::= <Arithmetic expression> | <Comparison> | <Function call> | <Assignment> | <Expression>, <Expression> | <Variable> | <Struct member>

<Expression operator> ::= <Expression>;
```
### Arithmetic and logic
```
<Arithmetic expression> ::= <Unary term>

<Unary> ::= + | - | ++ | -- | ~ | new | delete | & | * | ~

<Mul> ::= *

<Sum> ::= +

<Power> ::= **

<And> ::= & | and

<Xor> ::= ^

<Or> ::= '|' | or

<Shift> ::= << | >>

<Unary term> ::= <Unary><Unary term> | <Power term>

<Power term> ::= <Power term><Power><Power term> | <Mul term>

<Mul term> ::= <Mul term><Mul><Mul term> | <Sum term>

<Sum term> ::= <Sum term><Sum><Sum term> | <Shift term>

<Shift term> ::= <Shift term><Shift><Shift term> | <And term>

<And term> ::= <And term><And><And term> | <Xor term>

<Xor term> ::= <Xor term><Xor><Xor term> | <Or term>

<Or term> ::= <Or term><Or><Or term> | <Arithmetic term>

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
<Operator>::= ‘{‘ { <Operator> } ‘}’ | <Operator>; <Operator> | <If> | <For> | <While> | <Input> | <Output> | <Expression operator> | <Variable Creation> | <Match>
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
<Input>::= input { <Variable>, } <Variable>; 

<Output>::= output {<Expression>,} <Expression>;
```
### Arrays
```
<Array<Type> declaration> ::= <Type> <Name>”[“<Expression>”]” [ = “{“
<Literal<Type>>,
<Literal<Type>>,
<Literal<Type>>,
…
<Literal<Type>>”}”];

// <Literal<Type>> - repeated <Expression> times

<Array<Type> declaration> ::= <Type> <Name> “[]” = “{“
<Literal<Type>>,
<Literal<Type>>,
<Literal<Type>>,
…
<Literal<Type>>”}”;

// <Literal<Type>> - repeated any number of times

<Array indexation> ::= <Name>[<Expression>]
```
### Match
```
<Match> ::= match <Name> ‘{‘ {<Expression> => <Operator>} ‘}’
```

