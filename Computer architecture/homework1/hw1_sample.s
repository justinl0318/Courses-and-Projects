.globl __start

.rodata
    division_by_zero: .string "division by zero"

.text
__start:
    # Read first operand
    li a0, 5
    ecall
    mv s0, a0
    # Read operation
    li a0, 5
    ecall
    mv s1, a0
    # Read second operand
    li a0, 5
    ecall
    mv s2, a0

###################################
#  TODO: Develop your calculator  #
#                                 #
###################################

output:
    # Output the result
    li a0, 1
    mv a1, s3
    ecall

exit:
    # Exit program(necessary)
    li a0, 10
    ecall

division_by_zero_except:
    li a0, 4
    la a1, division_by_zero
    ecall
    jal zero, exit
    
    
li x20, 2
addi s3, x0, x0
jal x1, recurrence
  
recurrence:
  addi sp, sp, -16
  sd x1, 8(sp)
  sd x10, 0(sp)
  addi x5, x10, -1
  bge x5, x20, L1
  addi x10, x0, 1
  addi sp, sp, 16
  jalr, x0, 0(x1)
  
L1:
  addi x10, x10, -1
  jal x1, recurrence
  addi x18, x10, 0
  ld x10, 0(sp)
  addi x10, x10, -2
  jal x1, recurrence
  addi x19, x0, 0
  ld x10, 0(sp)
  ld x1, 8(sp)
  addi sp, sp, 16
  slli x18, x18, 1
  addi s3, x18, x19
  jalr x0, 0(x1)


recurrence:
  addi sp, sp, -24
  sw s0, 16(sp)
  sw x1, 8(sp)
  sw x10, 0(sp)
  addi x5, s0, -1
  bge x5, x20, L1
  addi x10, x0, 1
  addi sp, sp, 24
  jalr x0, 0(x1)
  
L1:
  addi s0, s0, -1
  jal x1, recurrence
  addi x18, x10, 0
  
  lw s0, 16(sp)
  addi s0, s0, -2
  jal x1, recurrence
  addi x19, x10, 0
  
  lw s0, 16(sp)
  lw x1, 8(sp)
  addi sp, sp, 24
  
  mul t1, x18, x20
  add x10, t1, x9
  jalr x0, 0(x1)
  #addi s3, t1, 0
  #beq x0, x0, output

  #add x10, x10, x19
  #jalr x0, 0(x1)
  