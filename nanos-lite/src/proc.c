#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  //_switch(&pcb[i].as);
  //current = &pcb[i];
  //((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

int current_game = 0;

void switch_current_game() {
  current_game=2-current_game;//0仙剑，1hello，2videotest
  Log("current_game=%d",current_game);
}
_RegSet* schedule(_RegSet *prev) {
  if(current!=NULL) {
    current->tf=prev;//保存现场
   }
   else {
    current=&pcb[0];//初始进程
   }
   
   static int N=0;
   static const int frequency=1000;
   if(current==&pcb[0]) {//若当前运行的是仙剑，则记录运行次数
     N++;
   }
   else {
     current=&pcb[0];
   }
   if(N==frequency) {
     current=&pcb[1];
     N=0;
   }
   //current=(current==&pcb[0]?&pcb[1]:&pcb[0]);
   _switch(&current->as);//切换地址空间
   return current->tf;//新进程上下文
}
