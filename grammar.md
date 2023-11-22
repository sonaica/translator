#  Language Grammar (Backus–Naur form)

### Program structure
```
<Program> ::= {<Preprocessing>} { <Global statement> } fun main() ‘{‘ <Operator> ‘}’

<Preprocessing> ::= <Directive> 

<Directive> ::= import <Library>; | define <Definition>;

<Library> ::= <Catalog>

<Catalog> ::= <String>\<Catalog>|<String>

<Definition> ::= <String> <String>

<Global statement> ::= <Function definition> | <Struct definition> | <Variable Initialization>
```
### Functions
```
<Function definition> ::= fun <Name> “(“ <Function parameters> “)” -> <Type>  ‘{‘ <Operator> ‘}’

<Function parameters> ::= <Type> <Name> | <Type> <Name>, <Function parameters>

<Function call> ::= <Name>(<Argument list>);

<Argument list> ::= <Name> {, <Name> }
```
### Structures
```
<Struct definition> ::= struct <Name>  ‘{‘ <Function definition> | <Variable Initialization> ‘}’ { <Name>, } <Name>;

<Struct member> ::= <Name>.(<Name>|<Function call>)
```
### Types and variables
```
<String> ::= <Symbol> | <Symbol><String>

<Symbol> ::= <Letter> | <Digit>

<Name> ::= <Letter> | <Name><Letter> | <Name><Digit>

<Variable>::= <Name> | <Array Indexation>

<Letter> ::= [[a-z] | [A-Z] | <Special symbol> ]

<Special symbol> ::= @ | # | £ | $ | _

<Type> ::= void | int | double | bool

<Variable Initialization> ::= <Type> {<Name>, } <Name>;

<Sign> ::= + | -

<Signed number> ::= <Sign><Digit>{<Digit>}|<Unsigned number>

<Unsigned number> ::= <Digit> {<Digit>}
```
### Expression
```
<Expression> ::= <Arithmetic expression> | <Logical expression> | <Comparison> | <Function call> | <Assignment> | <Expression>, <Expression> | <Variable>

<Expression operator> ::= <Expression> ;
```
### Arithmetic
```

<Arithmetic term> ::= <Arithmetic literal> | <Variable>

<Arithmetic literal> ::= <Signed number> | <Signed number>.<Unsigned number>

<Or term> ::= <Arithmetic term><Or><Or term> | <Arithmetic term>
<Xor term> ::= <Or term><Xor><Xor term> | <Or term>
<And term> ::= <Xor term><And><And term> | <Xor term>
<Power term> ::= <And term><Power><Power term> | <And term>
<Sum term> ::= <Power term><Sum><Sum term> | <Power term>
<Mul term> ::= <Sum term><Mul><Mul term> | <Sum term>
<Unary term> ::= <Unary><Mul term> | <Mul term>
<Arithmetic expression> ::= <Unary term>

<Unary> ::= + | - | ++ | -- | ~ | new | delete | & | * | ~
// add , arithmetic and logic
```
### Assignment
```
<Assignment> ::= <Variable> <Assignment operator> <Expression>

<Assignment operator> ::= = | <<= | >>= | += | ->= | -= | *= | **= | /= | //= | ^= | &= | |= | %=
```
### Comparison
```
<Comparison> ::= <Expression> <Comparison operators> <Expression>

<Comparison operators> ::= > | < | >= | <= | == | != 
```
### Logic
```
<Boolean literal> ::= false | true
```
### Operator
```
<Operator>::= ‘{‘ { <Operator> } ‘}’ | <Operator>; <Operator> | <If> | <For> | <While> | <Input> | <Output> | <Expression operator> | <Variable Initialization> | <Match>
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
<Match> ::= match <Name> ‘{‘ {<Expression> => <Operator>;} ‘}’
```

