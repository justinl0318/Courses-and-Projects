.globl	__start

.rodata
        msg: .asciiz "Empty!"
.text

push_front_list:             
        ### if(list == NULL)return; ###
        beqz    a0, LBB0_2
        ### save ra、s0 ###
        addi    sp, sp, -16
        sw      ra, 12(sp)                      
        sw      s0, 8(sp)                       
        sw      s1, 4(sp)                       
        mv      s1, a1
        mv      s0, a0
        ### node_t *new_node = (node_t*)sbrk(sizeof(*new_node)); ###
        li      a0, 8
        call    sbrk
        ### new_node->value = value; ###
        sw      s1, 0(a0)
        ### new_node->next = list->head; ###
        lw      a1, 0(s0) #get list->head, and save it to a1
        sw      a1, 4(a0) #new_node->next = list->head
        ### list->head = new_node; ###
        sw      a0, 0(s0)
LBB0_2:
        ## exit handling ###
        lw      ra, 12(sp)                      
        lw      s0, 8(sp)                       
        lw      s1, 4(sp)                       
        addi    sp, sp, 16
        ret
        
#print_list(head):
#  if head == NULL:
#    return
#  print_list(head->next)
#  print(head->value)
    
print_list:
        addi sp, sp, -8
        sw ra, 4(sp) #save return address in memory
        sw a0, 0(sp) #save current node in memory            
        bne a0, x0, recur #if a0 != 0, continue recursion
        addi sp, sp, 8 #else: base case
        jalr x0, 0(ra) #return
        
recur:
        lw t1, 4(a0) #load node->next
        mv a0 t1 #node = node->next
        jal print_list #print_list(node->next)
        
        lw a0, 0(sp) #load original node from memory
        lw t0, 0(a0) #load original node->value
        mv a0 t0
        call print_int 
        lw ra, 4(sp) #load return address in memory
        addi sp, sp, 8 #restore stack
        ret

__start:
        ### save ra、s0 ###                                   
        addi    sp, sp, -16
        sw      ra, 12(sp)                      
        sw      s0, 8(sp)                                            
        ### read the numbers of the linked list ###
        call    read_int
        ### if(nums == 0) output "Empty!" ###
        beqz    a0, LBB2_2
        ### if(nums <= 0) exit
        mv      s0, a0
        blez    a0, exit
LBB2_1:                                
        call    read_int
        ### set push_front_list argument ###
        mv      a1, a0
        mv      a0, sp
        call    push_front_list
        addi    s0, s0, -1
        bnez    s0, LBB2_1
        lw      a0, 0(sp)
        j       LBB2_3
LBB2_2:
        call    print_str
        j       exit
LBB2_3:
        call    print_list
exit:   
        ### exit handling ###
        li      a0, 0
        lw      ra, 12(sp)                      
        lw      s0, 8(sp)                       
        addi    sp, sp, 16
	li a0,	10
	ecall

read_int:
	li	a0, 5
	ecall
	jr	ra

sbrk:
	mv	a1, a0
	li	a0, 9
	ecall
	jr	ra
 
print_int:
	mv 	a1, a0
	li	a0, 1
	ecall
	li	a0, 11
	li	a1, ' '
	ecall
	jr	ra

print_str:
        li      a0, 4
        la      a1, msg
        ecall
        jr      ra