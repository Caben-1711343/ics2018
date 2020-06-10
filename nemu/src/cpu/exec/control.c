#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  //TODO();
  
  rtl_li(&t2,decoding.seq_eip);
  rtl_push(&t2);//eip入桟，跳到调用函数地址
  decoding.is_jmp=1;//设置跳转标志位
  

  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  //TODO();
  rtl_pop(&decoding.jmp_eip);//eip出桟
  decoding.is_jmp=1;//设置跳转标志位

  print_asm("ret");
}

make_EHelper(call_rm) {
  //TODO();
  //模仿call指令
  rtl_li(&t2,decoding.seq_eip);
  rtl_push(&t2);//eip入桟，跳到调用函数地址
  decoding.jmp_eip = id_dest->val;//无偏移，直接跳转
  decoding.is_jmp=1;//设置跳转标志位

  print_asm("call *%s", id_dest->str);
}
