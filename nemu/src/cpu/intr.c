#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
	// 将eflags、cs、eip入栈
	memcpy(&t1,&cpu.eflags,sizeof(cpu.eflags));
	rtl_li(&t0,t1);
	rtl_push(&t0);
	rtl_push(&cpu.cs);
	rtl_li(&t0,ret_addr);
	rtl_push(&t0);
	// 根据中断号找到门描述符地址
	vaddr_t gate_addr=cpu.idtr.base+NO*sizeof(GateDesc);
	// 检验合法性
	assert(gate_addr<=cpu.idtr.base+cpu.idtr.limit);
  	// 找到中断处理程序的地址
  	uint32_t low=vaddr_read(gate_addr,2);
  	uint32_t high=vaddr_read(gate_addr+6,2);
  	uint32_t target_addr=(high<<16)+low;

#ifdef DEBUG
  	Log("target_addr: 0x%x",target_addr);
#endif
  	// 设置跳转
  	decoding.is_jmp=1;
  	decoding.jmp_eip=target_addr;
  // TODO();
}

void dev_raise_intr() {
  cpu.INTR=true;
}
