#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf,off_t offset,size_t len);
extern size_t get_ramdisk_size();//外部声明

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());//加载用户程序到正确的位置
  return (uintptr_t)DEFAULT_ENTRY;
}

