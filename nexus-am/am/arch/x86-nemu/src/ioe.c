#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  unsigned long time=inl(RTC_PORT)-boot_time;
  return time;
  //return 0;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  //int i;
  //for (i = 0; i < _screen.width * _screen.height; i++) {
  //  fb[i] = i;
  //}
  //仿照native
  int len = (w>_screen.width-x) ? _screen.width-x : w;
  int cp_bytes = sizeof(uint32_t) * len;
  for (int j = 0; j < h && y + j < _screen.height; j ++) {
    memcpy(&fb[(y + j) * _screen.width + x], pixels, cp_bytes);
    pixels += w;
  }
}

void _draw_sync() {
}

int _read_key() {//检测按键
  if(inb(0x64)) {
    return inl(0x60);
  }
  else {
    return _KEY_NONE;
  }
}
//获取屏幕高度和宽度
void get_screen(int* width,int* height) {
  *width=_screen.width;
  *height=_screen.height;
}
