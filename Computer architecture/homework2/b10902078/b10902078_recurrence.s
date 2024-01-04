.global __start

.text
__start:
  li a0, 5
  ecall
  mv s0, a0
  
#recur(n):
#  if n < 2: 
#    return n
#  return 2 * recur(n-1) + recur(n-2)

#x1 = return address
#x10 = function argument n / return value
li x20, 2 #set x20 = 2 = base case threshold
jal x1, recurrence
addi s3, x10, 0
beq x0, x0, output

recurrence:
  addi sp, sp, -32
  sw x10, 24(sp) #save left child return value
  sw x10, 16(sp) #save right child return value
  sw x1, 8(sp) #return address
  sw x10, 0(sp) #argument n / return value

  bge x10, x20, L1 #if n >= 2: continue recursion
  #else base case: return n = x10
  addi sp, sp, 32
  jalr x0, 0(x1)

L1:
  addi x10, x10, -1
  jal x1, recurrence #recur(n-1)
  sw x10, 24(sp) #save the result of recur(n-1) to left child spot
  
  lw x10, 0(sp) #get original argument n's value
  addi x10, x10, -2
  jal x1, recurrence #recur(n-2)
  sw x10, 16(sp) #save the result of recur(n-2) to right child spot

  lw x6, 24(sp) #load the result of left child from memory
  lw x7, 16(sp) #load the result of right child from memory
  lw x1, 8(sp) #restore caller's address
  addi sp, sp, 32


  slli x6, x6, 1 #2 * recur(n-1)
  add x10, x6, x7 #return value x10 = 2 * recur(n-1) + recur(n-2)
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