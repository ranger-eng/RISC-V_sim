        .data
input:  .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 # data array

        .text
main:
        addi a2, zero, 0        # initialize i = 0, store in arg register a2
        jal ra, fibonacci       # jump to fibonacci, store return address to ra

        # exit program
        jal zero, exit

fibonacci:
        jr ra                   # return from function back to ra, return address

exit:   
        nop