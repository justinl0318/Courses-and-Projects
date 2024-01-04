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

#s0 = first operand
#s1 = operator
#s2 = second operand

#add
add s3, s0, s2
beq s1, x0, output

#subtract
sub s3, s0, s2
li t1, 1
beq s1, t1, output

#multiply
mul s3, s0, s2
li t1, 2
beq s1, t1, output

#divide
li t1, 3
beq s1, t1, divide

#minimum
li t1, 4
beq s1, t1, minimum

#power
li t1, 5
beq s1, t1, power

#factorial
li t1, 6
beq s1, t1, factorial

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

divide:
  #if s2 = 0, do division by zero
  beq s2, x0, division_by_zero_except
  bne s2, x0, do_normal_division
  
division_by_zero_except:
  li a0, 4
  la a1, division_by_zero
  ecall
  jal zero, exit
    
do_normal_division:
  div s3, s0, s2
  beq x0, x0, output

minimum:
  #if a < b
  add s3, s0, x0 #save s0 to s3 
  blt s0, s2, output
  # else (a >= b)
  add s3, s2, x0 #save s2 to s3
  beq x0, x0, output
  
power:
  li s3, 1 #set final output to be 1
  li t2, 0 #set count = 0
  beq x0, x0, power_for_loop

power_for_loop: #for (count = 0 to count < s2)
  beq t2, s2, output #if count = s2, break
  mul s3, s3, s0 #s3 *= s0
  addi t2, t2, 1 #count += 1
  beq x0, x0, power_for_loop
  
factorial:
  li s3, 1 #set final output to be 1 first
  add t2, s0, x0 #set temp = s0
  beq x0, x0, factorial_for_loop
  
factorial_for_loop:
  beq t2, x0, output #if temp = 0, break
  mul s3, s3, t2
  addi t2, t2, -1
  beq x0, x0, factorial_for_loop




