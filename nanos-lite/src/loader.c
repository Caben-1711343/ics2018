#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern void ramdisk_read(void *buf,off_t offset,size_t len);
extern size_t get_ramdisk_size();//外部声明
extern void* new_page(void);

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());//加载用户程序到正确的位置
  int fd=fs_open(filename,0,0);//打开文件
  //Log("filename=%s,fd=%d",filename,fd);
  //fs_read(fd,DEFAULT_ENTRY,fs_filesz(fd));//读文件
  size_t len=fs_filesz(fd);//获取文件大小
  void* pa;
  void* va;
  Log("filename=%s,fd=%d",filename,fd);
  void* end=DEFAULT_ENTRY+len;
  //从文件中按页读取，不足一页单独处理
  for(va=DEFAULT_ENTRY;va<end;va+=PGSIZE) {
    pa=new_page();
    _map(as,va,pa);
    fs_read(fd,pa,(pa-va)<PGSIZE ? (end-va) : PGSIZE);
  }
  fs_close(fd);//关闭文件
  return (uintptr_t)DEFAULT_ENTRY;
}

