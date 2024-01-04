#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

// for mp3
uint64
sys_thrdstop(void)
{
  int delay;
  uint64 context_id_ptr;
  uint64 handler, handler_arg;
  if (argint(0, &delay) < 0)
    return -1;
  if (argaddr(1, &context_id_ptr) < 0)
    return -1;
  if (argaddr(2, &handler) < 0)
    return -1;
  if (argaddr(3, &handler_arg) < 0)
    return -1;

  //struct proc *proc = myproc();

  //TODO: mp3
  int real_context_id;
  copyin(myproc()->pagetable, (char *)&real_context_id, context_id_ptr, sizeof(int));
  if (real_context_id >= MAX_THRD_NUM) return -1;

  struct proc *proc = myproc();
  proc->delay = delay;
  proc->tickscount = 0;
  proc->handler = handler;
  proc->handlerarg = handler_arg;
  if (real_context_id == -1){
    for (int i = 0; i < MAX_THRD_NUM; i++){
      if (proc->idused[i] == 0){
        proc->idused[i] = 1;
        proc->contextid = i;
        copyout(myproc()->pagetable, context_id_ptr, (char *)&proc->contextid, sizeof(int));
        return 0;
      }
    }
  }
  else if (real_context_id >= 0){
    proc->contextid = real_context_id;
    proc->idused[proc->contextid] = 1;
    return 0;
  }

  return -1;
}

// for mp3
uint64
sys_cancelthrdstop(void)
{
  int context_id, is_exit;
  if (argint(0, &context_id) < 0)
    return -1;
  if (argint(1, &is_exit) < 0)
    return -1;

  if (context_id < 0 || context_id >= MAX_THRD_NUM) {
    return -1;
  }

  //struct proc *proc = myproc();

  //TODO: mp3
  struct proc *proc = myproc();
  proc->delay = 0; //cancel the thrdstop() call
  proc->contextid = context_id;
  proc->isexit = is_exit;
  proc->mystate = 3;
  return proc->tickscount;
}

// for mp3
uint64
sys_thrdresume(void)
{
  int context_id;
  if (argint(0, &context_id) < 0)
    return -1;

  //struct proc *proc = myproc();

  //TODO: mp3
  if (context_id < 0 || context_id >= MAX_THRD_NUM) return -1;

  struct proc *proc = myproc();
  proc->contextid = context_id;
  proc->mystate = 2;
  return 0;
}
