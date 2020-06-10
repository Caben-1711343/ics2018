#include "cpu/exec.h"

make_EHelper(test) {
  //TODO();
  //参照and指令，不将结果写回
  rtl_and(&t2,&id_dest->val,&id_src->val);
  //以下为设置标志寄存器
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_set_CF(&tzero);//零寄存器可以直接用
  rtl_set_OF(&tzero);

  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_and(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);//写回寄存器
  //以下为设置标志寄存器
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  rtl_xor(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);//写回寄存器
  //以下为设置标志寄存器
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  //设置EFLAGS
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_set_CF(&tzero);//零寄存器可以直接用
  rtl_set_OF(&tzero);

  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_sext(&t2,&id_dest->val,id_dest->width);//符号扩展
  rtl_sar(&t2,&t2,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  
  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  //参照shl
  rtl_shr(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  
  //print_asm_template2(shl); 

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  rtl_not(&id_dest->val);//取反
  operand_write(id_dest,&id_dest->val);

  print_asm_template1(not);
}

make_EHelper(rol) {//循环左移
  int tmp=id_src->val;
  //暂存rm
  rtl_li(&t0,id_dest->val);
  while(tmp){
    rtl_msb(&t2,&t0,id_dest->width);
    t0=t0*2+t2;
    tmp-=1;
  }
  if(id_src->val==1){
    // t2暂存高位
    rtl_msb(&t2,&t0,id_dest->width);
    rtl_eqi(&t2,&t2,cpu.eflags.CF);
    t2=t2==0?1:0;
    rtl_set_OF(&t2);
  }
  operand_write(id_dest,&t0);
  print_asm_template1(rol);
}

