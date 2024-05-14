#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "riscv.h"
#include "types.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void main() {
    if (cpuid() == 0) {
        consoleinit();
        printfinit();
        printf("\n");
        printf("xv6 kernel is booting\n");
        printf("\n");
        kinit();            // physical page allocator 物理页分配器
        kvminit();          // create kernel page table 创建内核页表
        kvminithart();      // turn on paging 打开分页
        procinit();         // process table 进程表
        trapinit();         // trap vectors 陷阱向量
        trapinithart();     // install kernel trap vector 安装内核陷阱向量
        plicinit();         // set up interrupt controller 设置中断控制器
        plicinithart();     // ask PLIC for device interrupts 请求 PLIC 设备中断
        binit();            // buffer cache 缓冲区缓存
        iinit();            // inode table inode节点表
        fileinit();         // file table 文件表
        virtio_disk_init(); // emulated hard disk 硬盘
        userinit();         // first user process 第一个用户进程
        __sync_synchronize();
        started = 1;
    } else {
        while (started == 0)
            ;
        __sync_synchronize();
        printf("hart %d starting\n", cpuid());
        kvminithart();  // turn on paging
        trapinithart(); // install kernel trap vector
        plicinithart(); // ask PLIC for device interrupts
    }

    scheduler();
}
