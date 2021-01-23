from formulas import *

def format_justices(justices):
    return BigAnd(map(lambda justice: "GF " + justice, justices))

def format_implication((lhs, rhs)):
    return IfThen(format_justices(lhs),
                  format_justices(rhs))

def format_sgrk(in_init, out_init, in_trans, out_trans, impls):
    internal_separator = " &\n"
    external_separator = " ;\n"

    components = [" &\n".join(component)
                  for component in [in_init, out_init, in_trans, out_trans,
                                    map(format_implication, impls)]]

    return " ;\n".join(components)
