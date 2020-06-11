#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

//dummy.c
make_EHelper(call);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(ret);

//add.c
make_EHelper(lea);
make_EHelper(and);
make_EHelper(nop);
make_EHelper(add);
make_EHelper(cmp);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(test);
make_EHelper(jcc);

//add-longlong.c
make_EHelper(adc);
make_EHelper(or);

//bit.c
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(dec);
make_EHelper(not);

//bubble-sort.c
make_EHelper(inc);

//fact.c
make_EHelper(jmp);
make_EHelper(imul2);

//fib.c

//goldbach.c
make_EHelper(cltd);
make_EHelper(idiv);

//hello-str.c
make_EHelper(movsx);
make_EHelper(jmp_rm);
make_EHelper(leave);
make_EHelper(div);

//if-else.c
//leap-year.c
//load-store.c
//matrix-mul.c
//max.c
//min3.c
//mov-c.c
//movsx.c

//mul-longlong.c
make_EHelper(imul1);

//pascal.c
//prime.c
//quick-sort.c

//recursion.c
make_EHelper(call_rm);

//select-sort.c

//shift.c
make_EHelper(shr);

//shuixianhua.c
//string.c

//sub-longlong.c
make_EHelper(sbb);

//sum.c
//switch.c
//to-lower-case.c
//unalign.c
//wanshu.c

//more besides
make_EHelper(mul);
make_EHelper(imul3);
make_EHelper(neg);

//串口
make_EHelper(in);
make_EHelper(out);

//时钟跑分
make_EHelper(cwtl);
make_EHelper(rol);

//PA3
make_EHelper(lidt);
make_EHelper(int);
