import sys
from itertools import *

l = [False, True]

n = int(sys.argv[1])

in_prefix = 'in:t'
out_prefix = 'out:a'

##################################################################

def var(i, b, prefix, X=''):
    return ('' if b else '!') + X + '"' + prefix + str(i) + '"'


def assume():
    conf_vars = [[var(i, b, in_prefix) for i, b in enumerate(ass)]
                 for ass in product([False, True], repeat=n)]

    conf_vars_next = [[var(i, b, in_prefix, 'X') for i, b in enumerate(ass)]
                      for ass in product([False, True], repeat=n)]
    
    conf = ['(' + ' & '.join(var_list) + ')' for var_list in conf_vars]
    conf_next = ['(' + ' & '.join(next_var_list) + ')'
                 for next_var_list in conf_vars_next]

    init = conf[0]
    trans = '(' + conf[0] + ' -> (' + ' | '.join(conf_next) + '))'

    for i in range(1, len(conf)):
        trans += ' &\n(' + conf[i] + ' -> ' + conf_next[i] + ')'

    return (init, trans)

##################################################################

def guarantee():
    conft = ['(' + ' & '.join([var(i, b, in_prefix)
                               for i, b in enumerate(ass)]) + ')'
             for ass in product([False, True], repeat=n)]

    conf_vars = [[var(i, b, out_prefix) for i, b in enumerate(ass)]
                 for ass in product([False, True], repeat=n)]

    conf_vars_next = [[var(i, b, out_prefix, 'X') for i, b in enumerate(ass)]
                 for ass in product([False, True], repeat=n)]
    
    conf = ['(' + ' & '.join(var_list) + ')' for var_list in conf_vars]
    conf_next = ['(' + ' & '.join(next_var_list) + ')'
                 for next_var_list in conf_vars_next]

    init = conf[0]
    trans = '((' + conf[0] + ') -> ' + "(" + ' | '.join(conf_next[1::2]) + "))"

    for i in range(2, 2**n, 2):
        trans += ' &\n(' + conf[i] + ' -> ' + conf_next[i+1] + ')'
        trans += ' &\n(' + conf[i+1] + ' -> ' + conf_next[i] + ')'

    trans += ' &\n(' + conf[1] + ' -> ' + conf_next[0] + ')'

    grk = ' &\n'.join(['((G F ' + conft[i] + ') -> (G F ' + conf[i] + '))'
                       for i in range(2**n)])

    return (init, trans, grk)


inp = ', '.join([in_prefix + str(i) + '' for i in range(n)])
outp = ', '.join([out_prefix + str(i) + '' for i in range(n)])

(in_init, in_trans) = assume()
(out_init, out_trans, grk) = guarantee()

print(' ;\n'.join([in_init, out_init, in_trans, out_trans, grk]))

