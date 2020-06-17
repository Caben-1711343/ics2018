#include "common.h"

extern _RegSet* do_syscall(_RegSet *r);
extern _RegSet* schedule(_RegSet *prev);
static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
    case _EVENT_SYSCALL://调用do_syscall()函数
      return do_syscall(r);
      //return schedule(r);//进程切换
    case _EVENT_TRAP:
      printf("Receive an event trap!!!\n");
      return schedule(r);//切换进程
    case _EVENT_IRQ_TIME://时钟中断
      Log("Receive an event:IRQ_TIME");
      return schedule(r);
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
