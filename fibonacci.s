        .data
input:  .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 # data array

        .text
main:
        addi t1, zero, 0          # initialize i = 0, store in register t1
        addi t3, zero, 10        # t3 = size = 10
        la   t4, input           # load base address of input array into t4
loop1:        
        addi a0, t1, 0            # store i into function argument register a0
        jal ra, fibonacci       # jump to fibonacci, store return address to ra
        
        slli t5, t1, 2          # t5 = &input[i] array address offset
        add t6, t4, t5          # t6 = &input[i]
        sw a1, 0(t6)            # input[i] = fibonacci return value
        
        addi t1, t1, 1          # i = i + 1
        slt t0, t1, t3         # t0 = (i < size) 1 ? 0
        bne t0, zero, loop1

        # exit program
        jal zero, exit

fibonacci:
        addi a1, a0, 1
        jr ra                   # return from function back to ra, return address

exit:   
        nop