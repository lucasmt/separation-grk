import sys

from formulas import *
from sgrk import format_sgrk

if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <#rooms>"
else:
    n_rooms = int(sys.argv[1])

    if n_rooms < 1:
        print "Number of rooms must be a positive integer, defaulting to 1"

    def make_init(room, clean):
        return ([Var(room[0])] + # room_0
                [Not(Var(room[i])) for i in range(1, n_rooms)] + # !room_i
                [Not(Var(clean[i])) for i in range(n_rooms)]) # !clean_i

    def make_trans(room, clean):
        return ([BigOr(map(Var, room))] + # (room_0 | ... | room_{n-1})
                # (X room_0 | ... | X room_1)
                [BigOr(map(Next, map(Var, room)))] + 
                # (room_i -> !room_j)
                [IfThen(Var(room[i]),
                        Not(Var(room[j])))
                 for i in range(n_rooms)
                 for j in range(i)] +
                # (X room_i -> !X room_j)
                [IfThen(Next(Var(room[i])),
                        Not(Next(Var(room[j]))))
                 for i in range(n_rooms)
                 for j in range(i)] +
                 # (room_i -> (X room_i | X room_{i+1}))
                [IfThen(Var(room[i]),
                        Or(Next(Var(room[i])),
                           Next(Var(room[i + 1]))))
                 for i in range(n_rooms - 1)] +
                # (room_{n-1} -> X room_{n-1})
                [IfThen(Var(room[n_rooms - 1]),
                        Next(Var(room[n_rooms - 1])))] +
                # ((!clean_i & X clean_i) -> (room_i & X room_i))
                [IfThen(And(Not(Var(clean[i])),
                            Next(Var(clean[i]))),
                        And(Var(room[i]),
                            Next(Var(room[i]))))
                 for i in range(n_rooms)] +
                # (clean_i -> X clean_i)
                [IfThen(Var(clean[i]),
                        Next(Var(clean[i])))
                 for i in range(n_rooms)])

    in_room = ["in:room" + str(i) for i in range(n_rooms)]
    in_clean = ["in:clean" + str(i) for i in range(n_rooms)]
    in_done = "in:done"
    out_room = ["out:room" + str(i) for i in range(n_rooms)]
    out_clean = ["out:clean" + str(i) for i in range(n_rooms)]

    in_init = (make_init(in_room, in_clean) +
               # !in:done
               [Not(Var(in_done))])
    in_trans = (make_trans(in_room, in_clean) +
                # (in:done -> X in:done)
                [IfThen(Var(in_done),
                        Next(Var(in_done)))] +
                # (X in:done -> (X in:room_i <-> in:room_i))
                [IfThen(Next(Var(in_done)),
                        Iff(Next(Var(in_room[i])),
                            Var(in_room[i])))
                 for i in range(n_rooms)] +
                # (X in:done -> (X in:clean_i <-> in:clean_i))
                [IfThen(Next(Var(in_done)),
                        Iff(Next(Var(in_clean[i])),
                            Var(in_clean[i])))
                 for i in range(n_rooms)])
    out_init = make_init(out_room, out_clean)
    out_trans = make_trans(out_room, out_clean)

    # ((GF in:done & GF !in:clean_i) -> (GF out:clean_i))
    impls_1 = [([Var(in_done),
                 Not(Var(in_clean[i]))],
                [Var(out_clean[i])])
               for i in range(n_rooms)]
    # ((GF in:done & GF in:clean_i) -> (GF !out:clean_i))
    impls_2 = [([Var(in_done),
                 Var(in_clean[i])],
                [Not(Var(out_clean[i]))])
               for i in range(n_rooms)]
    impls = impls_1 + impls_2
    
    print(format_sgrk(in_init, out_init, in_trans, out_trans, impls))
