def TrueConst():
    return "true"

def FalseConst():
    return "false"

def Var(name):
    return '"' + name + '"'

def Not(f):
    return "!" + f

def And(f1, f2):
    return "(" + f1 + " & " + f2 + ")"

def Or(f1, f2):
    return "(" + f1 + " | " + f2 + ")"

def IfThen(f1, f2):
    return "(" + f1 + " -> " + f2 + ")"

def Iff(f1, f2):
    return "(" + f1 + " <-> " + f2 + ")"

def Xor(f1, f2):
    return "(" + f1 + " ^ " + f2 + ")"

def BigAnd(fs):
    return "(" + " & ".join(fs) + ")"

def BigOr(fs):
    return "(" + " | ".join(fs) + ")"

def Next(f):
    return "X " + f

def Always(f):
    return "G " + f

def Event(f):
    return "F " + f

def Until(f1, f2):
    return "(" + f1 + " U " + f2 + ")"

def WeakUntil(f1, f2):
    return "(" + f1 + " W " + f2 + ")"
