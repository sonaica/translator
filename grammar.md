#  Language Grammar (Backus–Naur form)

### Program structure
```
<Program> ::= {<Directive>} {  <Function definition> | <Struct definition> | <Variable creation> } main() ‘{‘ <Operator> ‘}’

<Directive> ::= import "<String>"; | define <String> <String>;
```
### Functions
```
<Function definition> ::= fun <Name> <Function parameters> (<Type>|<Name>)  ‘{‘ <Operator> return <Expression>‘}’

<Function parameters> ::= "(" (<Name>|<Type>) <Name> {, (<Name>|<Type>) <Name> } ")"
```
### Structures
```
<Struct definition> ::= struct <Name>  ‘{‘ { <Function definition> | <Variable creation> } ‘}’ <Name> {, <Name>};

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

<Entity Creation> ::= <Type> <Name> [= <Expression>] | <Array declaration>

<Variable Creation> ::= <Entity Creation> {, <Entity Creation>};

//<Variable Definition> ::= <Type> <Name> {, <Name>};

//<Variable Initialization> ::= <Type> {<Name> = <Expression>, } <Name> = <Expression>;

<Sign> ::= + | -

<Signed number> ::= <Sign><Unsigned number>|<Unsigned number>

<Unsigned number> ::= <Digit> {<Digit>}
```
### Expression
```
<Expression> ::= <Expression term> {, <Expression term> }

<Expression term>::= <Arithmetic logic expression>
```
### Arithmetic, logic, comparison and assignment
```
<Arithmetic logic expression> ::= <Assignment term>

<FunctionCall> ::= (<Expression>) | ()

<ArrayAccess> ::= [<ExpressionTerm>]

<MemberAccess> ::= .

<Unary> ::= + | - | ++ | -- | ~

<Mul> ::= * | / | %

<Sum> ::= + | -

<Power> ::= **

<And> ::= & | and

<Xor> ::= ^

<Or> ::= '|' | or

<Shift> ::= << | >>

<Equality> ::= == | !=

<NonEquality> ::= > | < | >= | <=

<Assignment> ::= = | <<= | >>= | += | -= | *= | **= | /= | //= | ^= | &= | |= | %=м

<ArrayFuncMember term> ::= <Arithmetic term>{(<MemberAccess><Arithmetic term>)|<ArrayAccess>|<FunctionCall>}

<Unary term> ::= <Unary><Unary term> | <ArrayFuncMember term>

<Mul term> ::= <Unary term>{<Mul><Unary term>}

<Sum term> ::= <Mul term>{<Sum><Mul term>}

<Shift term> ::= <Sum term>{<Shift><Sum term>}

<NonEquality term> ::= <Shift term>{<NonEquality><Shift term>}

<Equality term> ::= <NonEquality term>{<Equality><NonEquality term>}

<And term> ::= <Equality term>{<And><Equality term>}

<Xor term> ::= <And term>{<Xor><And term>}

<Or term> ::= <Xor term>{<Or><Xor term>}

<Assignment term> ::= <Or term>{<Assignment><Or term>}

<Arithmetic term> ::= <Boolean literal> | <Arithmetic literal> | "("<Arithmetic logic expression>")" | <Identifier>

<Identifier> ::= <Name>

<Arithmetic literal> ::= <Signed number> | <Signed number>.<Unsigned number>

<Boolean literal> ::= false | true
```
### Operator
```
<Operator>::= "{" { <Operator> } "}" | <If> | <For> | <While> | <Input>; | <Output>; | <Expression>; | <Variable creation>; | <Match>
```
### Conditional operator
```
<If> :: if "("<Expression>")" <Operator> [ else <Operator> ]
```
### Cycles
```
<For>::= for "("(<Variable Initialization>); <Expression>; <Expression>")" <Operator>

<While>::= while "(" <Expression> ")" <Operator>
```
### Input and Output
```
<Input> ::= input <Variable> {, <Variable>}

<Output> ::= output <Output Item> {, <Output Item>}

<OutputItem> ::= "<String>"|<Expression>
```
### Arrays
```
<Array declaration>::= <Array declaration exact> | <Array declaration auto>

<Array declaration exact> ::= <Type> <Name>”[“<Expression>”]” [ = “{“ <Expression> "}"];

<Array declaration auto> ::= <Type> <Name> “[]” = “{“ <Expression> "}";

<Array indexation> ::= <Name>"["<Expression>"]"

<Literal>::= <Arithmetic literal> | <boolean literal>
```
### Match
```
<Match> ::= match <Name> ‘{‘ {<Literal> => <Operator>} ‘}’
```

