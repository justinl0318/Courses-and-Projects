#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

int increase_mem(struct proc *proc, int newaddr){
	if (newaddr >= 0 && newaddr < MAXVA){//check newaddr in range of process's va
		proc->sz = newaddr;
		return 1;
	}
	return -1;
}

int decrease_mem(struct proc *proc, int oldaddr, int newaddr){
	if (uvmdealloc(proc->pagetable, oldaddr, newaddr) == newaddr){ //dealloc succeed
		proc->sz = newaddr;
		return 1;
	}
	return -1;
}

uint64
sys_sbrk(void)
{
	uint64 oldaddr, newaddr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
	oldaddr = myproc()->sz;
	newaddr = oldaddr + n;
	if (n >= 0){
		if (increase_mem(myproc(), newaddr) == -1) return -1;
	}
	else{
		if (decrease_mem(myproc(), oldaddr, newaddr) == -1) return -1;;
	}
  return oldaddr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
