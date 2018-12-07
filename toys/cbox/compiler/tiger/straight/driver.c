#include <stdio.h>
#include "../common/util.h"
#include "straight.h"


int main()
{
    //A_exp exp = A_NumExp(5);
    //A_stm prog = A_AssignStm("a", exp);

#if 1
    A_stm prog = 
        A_CompoundStm(A_AssignStm("a",
                        A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),
            A_CompoundStm(A_AssignStm("b",
                        A_EseqExp(A_PrintStm(A_PairExpList(A_IdExp("a"),
                                    A_LastExpList(A_OpExp(A_IdExp("a"), A_minus,
                                            A_NumExp(1))))),
                            A_OpExp(A_NumExp(10), A_times, A_IdExp("a")))),
                A_PrintStm(A_LastExpList(A_IdExp("b")))));

#endif
}
