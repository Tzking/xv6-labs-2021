#include "date.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "proc.h"
#include "riscv.h"
#include "spinlock.h"
#include "types.h"

uint64 sys_exit(void) {
    int n;
    if (argint(0, &n) < 0)
        return -1;
    exit(n);
    return 0; // not reached
}

uint64 sys_getpid(void) {
    return myproc()->pid;
}

uint64 sys_fork(void) {
    return fork();
}

uint64 sys_wait(void) {
    uint64 p;
    if (argaddr(0, &p) < 0)
        return -1;
    return wait(p);
}

uint64 sys_sbrk(void) {
    int addr;
    int n;

    if (argint(0, &n) < 0)
        return -1;

    addr = myproc()->sz;
    if (growproc(n) < 0)
        return -1;
    return addr;
}

uint64 sys_sleep(void) {
    int n;
    uint ticks0;

    if (argint(0, &n) < 0)
        return -1;
    acquire(&tickslock);
    ticks0 = ticks;
    while (ticks - ticks0 < n) {
        if (myproc()->killed) {
            release(&tickslock);
            return -1;
        }
        sleep(&ticks, &tickslock);
    }
    release(&tickslock);
    return 0;
}


uint64 sys_kill(void) {
    int pid;

    if (argint(0, &pid) < 0)
        return -1;
    return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void) {
    uint xticks;

    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);
    return xticks;
}

#ifdef LAB_PGTBL
int sys_pgaccess(void) {
    uint64 addr;
    int length;
    int bitmask;
    if (argaddr(0, &addr) < 0) // 页表第一页的起始地址
        return -1;
    if (argint(1, &length) < 0) // 要考察的页表长度
        return -1;
    if (argint(2, &bitmask)) // 位掩码 第一页对应最低有效位
        return -1;

    // 接受相关参数
    if (length > 32 || length < 0) 
        return -1;

    // 判断长度限制范围
    int ret = 0;               // 中间变量
    struct proc *p = myproc(); // 获取当前进程
    for (int i = 0; i < length; i++) {
        int va = addr + i * PGSIZE;
        int bitmask = vmpgaccess(p->pagetable, va);
        ret = ret | bitmask << i;
    } // 主体代码
    if (copyout(p->pagetable, bitmask, (char *)&ret, sizeof(ret)) < 0) {
        return -1;
    }
    return 0;
}
#endif
