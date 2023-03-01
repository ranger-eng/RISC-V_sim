 .data
input:  .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 # data array

        .text
main:
        addi sp, sp, -12
        addi t1, zero, 0          # initialize i = 0, store in register t1
        addi t3, zero, 10        # t3 = size = 10
        la   t4, input           # load base address of input array into t4
loop1:
        addi a0, t1, 0            # store i into function argument register a0
        
        # must save t1, t4, t3
        sw t1, 0(sp)                #saving original values of t1,t3,t4 to stack locations
        sw t3, 4(sp)
        sw t4, 8(sp)
        
        jal ra, fibonacci       # jump to fibonacci, store return address to ra

        # restore t1, t3, t4    
        lw t1, 0(sp)            #loading values from stack locations back into registers
        lw t3, 4(sp)
        lw t4, 8(sp)

        slli t5, t1, 2          # t5 = &input[i] array address offset
        add t6, t4, t5          # t6 = &input[i]
        sw a1, 0(t6)            # input[i] = fibonacci return value

        addi t1, t1, 1          # i = i + 1

        slt t0, t1, t3         # t0 = (i < size) 1 ? 0
        bne t0, zero, loop1

        addi sp, sp, 12  # restore stack pointer to original location 
        # exit program
        jal zero, exit


fibonacci:
     

     addi t1,zero, 0        # t1 = first = 0
  
    
    addi t2,zero,1         # t2 = second = 1
   

    addi a1,zero, 0        # a1 = result = 0
    

    addi t3,zero,1         # register used for if statement comparison

 

#if (n < 1 or n == 1 )


   blt a0,t3,if_1      # want to branch to if block if n < t3 ...(t3=1) # was a2
    
 
    
    
   beq a0,t3,if_1      # want to branch to if block if n == t3 == 1     # was a2   
   
   



# if  you haven't branched to if by now, then enter else block logic

# else{
    
    addi t4,zero , 2        # t4 = i = 2
    
   
 
loop_2:
    
     add a1,t1,t2        # result = first + second
   
  
    
    add t1,zero,t2      # first = second
   
  
    
    add t2,zero,a1      # second = result
    
 
    
    
    # do all logic with current i, increment i, perform loop check, carry over valid
    #value of i into next iteration. 
    
    
    addi t4,t4,1              # add 1 to i
    
   

    
    blt t4,a0,loop_2          # blt i,n,loop_2 # was a2
   
    
    beq t4,a0,loop_2          # beq i,n,loop_2 # was a2
    
   
    
    # if you make it to this point you have exited loop
                         
    jr ra                   # return from function back to ra, return address

    
    
#}


if_1: 

add a1,a0,zero     # store value of n (a2) into result variable (a1) # changed a2 to a0


jr ra




exit: nop

