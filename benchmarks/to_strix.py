import sys

from formulas import *

if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <filename>"
else:
    filename = sys.argv[1]
    
    with open(filename, "r") as in_file:
        components = in_file.read().replace("\n", " ").split(";")

        if len(components) != 5:
            print "Error: Expected 5 formulas, found " + len(components)
        else:
            parenthesize = lambda s : "(" + s.strip(" ") + ")"
            
            in_init = parenthesize(components[0])
            out_init = parenthesize(components[1])
            in_trans = parenthesize(components[2])
            out_trans = parenthesize(components[3])
            grk = parenthesize(components[4])

            print IfThen(in_init,
                         BigAnd([out_init,
                                 WeakUntil(out_trans, Not(in_trans)),
                                 IfThen(Always(in_trans), grk)]))

