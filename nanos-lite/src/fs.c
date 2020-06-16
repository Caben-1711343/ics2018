#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

//对文件记录表进行初始化
void init_fs() {
  // TODO: initialize the size of /dev/fb
  extern void get_screen(int* p_width,int* p_height);//外部引用声明
  int width=0,height=0;
  get_screen(&width,&height);//获取屏幕信息
  file_table[FD_FB].size=width*height*sizeof(uint32_t);
  Log("Set FD_FB size=%d",file_table[FD_FB].size);//辅助输出
}


//返回文件的大小
size_t fs_filesz(int fd) {
  assert(fd>=0 && fd<NR_FILES);//不能超过给定文件个数
  return file_table[fd].size;
}

off_t disk_offset(int fd) {
  assert(fd>=0 && fd<NR_FILES);//不能超过给定文件个数
  return file_table[fd].disk_offset;
}

off_t get_open_offset(int fd) {
  assert(fd>=0 && fd<NR_FILES);//不能超过给定文件个数
  return file_table[fd].open_offset;
}

//设置open_offset，每次从指定位置读取
void set_open_offset(int fd,off_t n) {
  assert(fd>=0 && fd<NR_FILES);//不能超过给定文件个数
  assert(n>=0);
  if(n>file_table[fd].size) {
    n=file_table[fd].size;
  }
  file_table[fd].open_offset=n;
}
//外部引用声明
extern void ramdisk_read(void *buf,off_t offset,size_t len);
extern void ramdisk_write(void *buf,off_t offset,size_t len);

//打开文件，忽略flags和mode
int fs_open(const char* filename,int flags,int mode) {
  for(int i=0;i<NR_FILES;i++) {
    if(strcmp(filename,file_table[i].name)==0) {
      return i;
    }
  }
  panic("This file is not exist!");
  return -1;
}


extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern void fb_write(const void *buf, off_t offset, size_t len);
extern size_t events_read(void *buf, size_t len);
//读取文件，调用ramdisk_read()函数
ssize_t fs_read(int fd,void* buf,size_t len) {
  assert(fd>=0 && fd<NR_FILES);//不能超过给定文件个数
  if(fd==FD_EVENTS) {
    return events_read(buf,len);//传入字节数为len，读取输入设备
  }
  if(fd<3||fd==FD_FB) {
    panic("Wrong fd!");
    return 0;
  }
  int n=fs_filesz(fd)-get_open_offset(fd);//最多能读取多少字节
  if(n>len) {
    n=len;
  }
  //ramdisk_read(buf,disk_offset(fd)+get_open_offset(fd),n);
  if(fd==FD_DISPINFO) {
    dispinfo_read(buf,get_open_offset(fd),n);
  }
  else {
    ramdisk_read(buf,disk_offset(fd)+get_open_offset(fd),n);
  }
  set_open_offset(fd,get_open_offset(fd)+n);//设置偏移量
  return n;
}

int fs_close(int fd) {
  assert(fd>=0 && fd<NR_FILES);//不能超过给定文件个数
  return 0;
} 

//参照fs_read
ssize_t fs_write(int fd,void* buf,size_t len) {
  assert(fd>=0 && fd<NR_FILES);//不能超过给定文件个数
  if(fd<3||fd==FD_DISPINFO) {
    panic("Wrong fd!");
    return 0;
  }
  int n=fs_filesz(fd)-get_open_offset(fd);
  if(n>len) {//大于文件长度则最多只能写文件长度个字节
    n=len;
  }
  //ramdisk_write(buf,disk_offset(fd)+get_open_offset(fd),n);
  if(fd==FD_FB) {
    fb_write(buf,get_open_offset(fd),n);
  }
  else {
    ramdisk_write(buf,disk_offset(fd)+get_open_offset(fd),n);
  }
  set_open_offset(fd,get_open_offset(fd)+n);//设置偏移量
  return n;
}

//移动位置
off_t fs_lseek(int fd,off_t offset,int whence) {
  switch(whence) {
    case SEEK_SET://文件开始
      set_open_offset(fd,offset);
      return get_open_offset(fd);
    case SEEK_CUR:
      set_open_offset(fd,get_open_offset(fd)+offset);
      return get_open_offset(fd);  
    case SEEK_END://文件末尾
      set_open_offset(fd,fs_filesz(fd)+offset);
      return get_open_offset(fd);
    default:
      panic("Unhandled whence ID = %d",whence);
      return -1;
  }
}

  
  















 

