#include "common.h"
#include "fs.h"
#include "memory.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern void ramdisk_read(void *buf,off_t offset,size_t len);
extern size_t get_ramdisk_size();//外部声明

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());//加载用户程序到正确的位置
  int fd=fs_open(filename,0,0);//打开文件
  Log("filename=%s,fd=%d",filename,fd);
  //fs_read(fd,DEFAULT_ENTRY,fs_filesz(fd));//读文件
  int size=fs_filesz(fd);
  int pagenum=size/PGSIZE;
  if(size%PGSIZE!=0) {
    pagenum++;
  }
  void *pa=NULL;
  void *va=DEFAULT_ENTRY;
  for(int i=0;i<pagenum;i++) {
    pa=new_page();
    _map(as,va,pa);
    fs_read(fd,pa,PGSIZE);
    va+=PGSIZE;
  }
  fs_close(fd);//关闭文件
  return (uintptr_t)DEFAULT_ENTRY;
}

