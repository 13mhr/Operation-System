#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64 sys_exit(void) {
  int n;
  if (argint(0, &n) < 0) return -1;
  exit(n);
  return 0;  // not reached
}

uint64 sys_getpid(void) { return myproc()->pid; }

uint64 sys_fork(void) { return fork(); }

uint64 sys_wait(void) {
  uint64 p;
  int flags;

  // 获取第一个参数（地址）
  if (argaddr(0, &p) < 0) return -1;

  // 获取第二个参数（flags）
  if (argint(1, &flags) < 0) return -1;

  // 调用修改后的 wait 函数，并传递地址和 flags 参数
  return wait(p, flags);
}

uint64 sys_sbrk(void) {
  int addr;
  int n;

  if (argint(0, &n) < 0) return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0) return -1;
  return addr;
}

uint64 sys_sleep(void) {
  int n;
  uint ticks0;

  if (argint(0, &n) < 0) return -1;
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

  if (argint(0, &pid) < 0) return -1;
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

uint64 sys_rename(void) {
  char name[16];
  int len = argstr(0, name, MAXPATH);
  if (len < 0) {
    return -1;
  }
  struct proc *p = myproc();
  memmove(p->name, name, len);
  p->name[len] = '\0';
  return 0;
}

uint64 sys_yield(void) {
  struct proc *p = myproc();
  printf("Save the context of the process to the memory region from address %p to %p\n", &p->context,
         ((char *)&p->context) + sizeof(p->context));
  printf("Current running process pid is %d and user pc is %p\n", p->pid, p->trapframe->epc);

  struct proc *np;
  int found = 0;
  for (np = p + 1; np != p; np = (np == &proc[NPROC - 1]) ? proc : np + 1) {
    acquire(&np->lock);
    if (np->state == RUNNABLE) {
      found = 1;
      printf("Next runnable process pid is %d and user pc is %p\n", np->pid, np->trapframe->epc);
      release(&np->lock);
      break;
    }
    release(&np->lock);
  }

  if (!found) {
    printf("No RUNNABLE process found, scheduling current process.\n");
  }

  // 将当前进程让出 CPU
  yield();  // 调用内核已经实现的 yield 函数进行调度

  return 0;
}
