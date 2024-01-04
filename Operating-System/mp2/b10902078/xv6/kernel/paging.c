#include "param.h"
#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "proc.h"

/* NTU OS 2022 */
/* Page fault handler */
int handle_pgfault() {
	/* Find the address that caused the fault */
	/* uint64 va = r_stval(); */

	/* TODO */
	uint64 va = r_stval();
	if (va >= myproc()->sz) return -1;
	uint64 va_rounded = PGROUNDDOWN(va);

	struct proc *proc = myproc();
	pte_t *pte = walk(proc->pagetable, va_rounded, 0); //find pte for the given virtual address
	if (pte == 0) pte = walk(proc->pagetable, va_rounded, 1);
	
	if (*pte & PTE_S){ //page is in disk, so we need to read the page from disk
		char *pa = kalloc();
		uint64 blkno = PTE2BLOCKNO(*pte);
		begin_op();
		read_page_from_disk(ROOTDEV, pa, blkno);
		end_op();
		bfree_page(ROOTDEV, blkno);
		*pte = BLOCKNO2PTE(blkno) | PTE_FLAGS(*pte);

		//set the PTE_V bit and unset the PTE_S bit
		*pte |= PTE_V, *pte &= ~PTE_S;
	}
	else{ //page fault and page not in disk, that means we need to allocate page and map page
		uint64 *pa = kalloc();
		mappages(myproc()->pagetable, va_rounded, PGSIZE, (uint64)pa, PTE_U | PTE_R | PTE_W | PTE_X);
		memset(pa, 0, PGSIZE);
	}
	return 1;
}
