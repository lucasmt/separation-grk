from formulas import *

def format_justices(justices):
    return BigAnd(map(lambda justice: "GF " + justice, justices))

def format_implication((lhs, rhs)):
    return IfThen(format_justices(lhs),
                  format_justices(rhs))

def format_sgrk(in_init, in_trans, out_init, out_trans, impls):
    formatted_string = "("
    formatted_string += BigAnd(in_init) + " &\n "
    formatted_string += "G (" + " &\n   ".join(in_trans) + ")"
    formatted_string += ") ->\n"
    formatted_string += "("
    formatted_string += BigAnd(out_init) + " &\n "
    formatted_string += "G (" + " &\n    ".join(out_trans) + ") &\n "
    formatted_string += " &\n ".join(map(format_implication, impls))
    formatted_string += ")"

    return formatted_string
