#include "common.h"
#include "syscall.h"

int sys_none() {
  return 1;
}

void sys_exit(int a) {
  _halt(a);
}

int sys_write(int fd,void* buf,size_t len) {
  if(fd==1||fd==2) {//fd为1或2则执行
    char c;
    Log("buffer:%s",(char*)buf);//用于比较
    for(int i=0;i<len;i++) {
      memcpy(&c,buf+i,1);
      _putc(c);
    }
    return len;
  }
  else {//否则返回错误
    panic("Unhandled fd=%d in sys_write",fd);
  }
  return -1;
}
  
_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  //获取系统调用参数
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {//判断系统调用号
    case SYS_none: SYSCALL_ARG1(r)=sys_none();break;
    case SYS_exit: sys_exit(a[1]); break;
    case SYS_write: SYSCALL_ARG1(r)=sys_write(a[1],(void*)a[2],a[3]);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}


