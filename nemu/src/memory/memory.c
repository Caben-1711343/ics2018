#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

//获取前20位
#define PTE_ADDR(pte)    ((uint32_t)(pte) & ~0xfff)

//解析VA，三个部分对应页目录、二级页表以及物理页内偏移
#define PDX(va)    (((uint32_t)(va) >> 22) & 0x3ff)
#define PTX(va)    (((uint32_t)(va) >> 12) & 0x3ff)
#define OFF(va)    ((uint32_t)(va) & 0xfff)
uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int id=is_mmio(addr);
  if(id==-1) {//映射到I/O空间
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
  else {//否则
    return mmio_read(addr,len,id);   
  }
  //return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int id=is_mmio(addr);
  if(id!=-1) {//映射到I/O空间
    mmio_write(addr,len,data,id);
  }
  else {//否则
    memcpy(guest_to_host(addr), &data, len);
  }
  //memcpy(guest_to_host(addr), &data, len);
}

//修改对应Accessed和Dirty位
paddr_t page_translate(vaddr_t addr,bool iswrite) {
  CR0 cr0=(CR0)cpu.CR0;
  if(cr0.paging && cr0.protect_enable) {//开启分页机制和保护模式
    CR3 cr3=(CR3)cpu.CR3;
    
    //页目录表
    PDE* pgdirs=(PDE*)PTE_ADDR(cr3.val);//获取页目录表基址
    PDE pde=(PDE)paddr_read((uint32_t)(pgdirs+PDX(addr)),4);//索引偏移
    Assert(pde.present,"addr=0x%x",addr);//present位无效，报错
    
    //二级页表
    PTE* ptable=(PTE*)PTE_ADDR(pde.val);//获取二级页表首地址
    PTE pte=(PTE)paddr_read((uint32_t)(ptable+PTX(addr)),4);
    Assert(pde.present,"addr=0x%x",addr);//present位无效，报错
    
    //设置Accessed和Dirty位
    pde.accessed=1;
    pte.accessed=1;
    if(iswrite) {
      pte.dirty=1;
    }
    
    //物理页内偏移
    paddr_t paddr=PTE_ADDR(pte.val) | OFF(addr);
    //printf("vaddr=0x%x,paddr=0x%x\n",addr,paddr);
    return paddr;
  }
  
  return addr;
}    
    
uint32_t vaddr_read(vaddr_t addr, int len) {
  //return paddr_read(addr, len);
  if(PTE_ADDR(addr) != PTE_ADDR(addr+len-1)) {//检查数据是否跨页
    printf("Error:The data passes two pages!");
    assert(0);
  }
  else {
    paddr_t paddr=page_translate(addr,false);//获取物理地址，只可读
    return paddr_read(paddr,len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  //paddr_write(addr, len, data);
  if(PTE_ADDR(addr) != PTE_ADDR(addr+len-1)) {//检查数据是否跨页
    printf("Error:The data passes two pages!");
    assert(0);
  }
  else {
    paddr_t paddr=page_translate(addr,true);//获取物理地址，可写
    paddr_write(paddr,len,data);
  }
}
