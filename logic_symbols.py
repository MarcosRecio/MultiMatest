# These strings are used in internal representations of the ops when parsed, and
# are stored in sstables when parsing the text data, and then cross-referenced
# when understanding the ops in the TreeNet code. (E.g., important to be able to
# distinguish unary and binary ops.) I.e., if these get changed, then data
# generation likely has to be done again.
#
# This isn't the same as the input operations allowed - there can be a many-to-
# one mapping in this case. E.g., both /\ and & are recognised for AND.

IDENTITY_SYMBOL = ''
NEGATION_SYMBOL = '~'
AND_SYMBOL = '&'
OR_SYMBOL = 'v'
XOR_SYMBOL = '^'
IMPLIES_SYMBOL = '->'
FOR_ALL_SYMBOL = 'A'
EXISTS_SYMBOL = 'E'
RELATION_SYMBOL = 'R{}'  # formatted for arity of relation.
FALSE_SYMBOL = 'F'
TRUE_SYMBOL = 'T'