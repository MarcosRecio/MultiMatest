```
Parses boolean expressions (as strings) to produce "calculation graph".
## Propositional logic
Given an expression like "a|((~a)&(b|c))", the function `parse` will output a
list of "op types" (which is variable names, or logical operands), and the list
of indices (relative to current op) that feed in to each operand. This is done
in an order that allows computation of the boolean expression. For example, the
above expression becomes:
(position):   OPS:   INPUTS:
0             a      []
1             a      []
2             ~      [-1]
3             b      []
4             c      []
5             |      [-2, -1]
6             &      [-4, -1]
7             |      [-7, -1]
## First-order logic
The above is also extended to first-order logic, with relations, "for all", and
"exists". For example, 'f(x, y)' is a relation, and 'all x . (f(x) -> g(x))' is
a "for all" formula.
Unary and binary relations are currently supported in this model. A binary
relation f(x, y) is parsed as a ternary op, with op-type "R2", and arguments
[f, x, y].
For all "all x . k" is parsed as a binary op, with op-type "A" and arguments
[x, k]. Similarly for "exists x . k"
```
