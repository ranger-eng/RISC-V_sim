        .data
input:  .word 3, 0, 1, 2, 6, -2, 4, 7, 3, 7

        .text
        la   x19, input     # address of input string
        addi x20, x0, 10
clear2: addi x28, x19, 0    # p = &array[0]
        slli x29, x20, 2    # x29 = size * 4
        add  x30, x19, x29  # x30 = &array[size]
loop2:  sw    x0, 0(x28)    # Memory[p] = 0
        addi x28, x28, 4    # p = p + 4
        slt  x31, x28, x30  # x31 = 1 if (p < &array[size])
        bne  x31, x0, loop2 # if (x31 != 0) goto loop2
exit:   nop
