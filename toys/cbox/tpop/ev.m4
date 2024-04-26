format(`Result is %d', eval(`2**15'))
format(`dude: %d %f %s', 42, 3.14, `sup')

divert(`-1')
# forloop(var, from, to, stmt) - simple version
define(`forloop', `pushdef(`$1', `$2')_forloop($@)popdef(`$1')')
define(`_forloop',
       `$4`'ifelse($1, `$3', `', `define(`$1', incr($1))$0($@)')')
divert`'dnl

define(`countdown', `$1, ifelse(eval($1 > 0), 1, `countdown(decr($1))', `Liftoff!')')dnl
dumpdef(`countdown')dnl
countdown(5)

one
define(`one', `ONE')dnl
one
define(`ONE', `two')dnl
one ONE oneONE
`one'

define(A, 100)dnl
define(B, A)dnl
define(C, `A')dnl
dumpdef(`A', `B', `C')dnl


