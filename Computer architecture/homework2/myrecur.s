.global __start

.text
__start:
  li a0, 5
  ecall
  mv s0, a0
  
#recur(n):
#  if n == 0:
#    return 0
#  if n == 1:
#    return 1
#  return 2 * recur(n-1) + recur(n-2)

#argument n in s0
li x20, 2
jal x1, recurrence
addi s3, x10, 0
beq x0, x0, output

recurrence:
  addi sp, sp, -24
  sw s0, 16(sp) #argument n
  sw x1, 8(sp) #return address
  sw x10, 0(sp) #return value
  #addi x5, s0, -1
  #bge x5, x20, L1
  bge s0, x20, L1 #if n >= 2: continue recursion
  addi x10, s0, 0 #else base case: return n
  #addi sp, sp, 24
  jalr x0, 0(x1)

L1:
  addi s0, s0, -1
  jal x1, recurrence #recur(n-1)
  #addi x6, x10, 0 #save return value of recur(n-1) (which is in x10) to x6
  addi sp, sp, 24
  
  lw s0, 16(sp)
  addi s0, s0, -2
  jal x1, recurrence #recur(n-2)
  lw t0, 0(sp) #t0 = recur(n-1)
  addi sp, sp, 24 #x10 = recur(n-2)
  #addi x7, x10, 0 #save return value of recur(n-2) (which is in x10) to x7
  
  #lw s0, 16(sp) #restore caller's argument n
  lw x1, 8(sp) #restore caller's address
  #lw x10, 0(sp) #restore caller's return value
  #addi sp, sp, 24
  
  slli t0, t0, 1 #recur(n-1) * 2
  #add x6, x6, x7
  add x10, x10, t0
  jalr x0, 0(x1)
  
  


output:
    # Output the result
    li a0, 1
    #we have to save the final output to s3 first
    mv a1, s3
    ecall
    beq x0 x0 exit

exit:
    # Exit program(necessary)
    li a0, 10
    ecall