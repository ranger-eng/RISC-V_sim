        .data
input:  .word 3, 5, 1, 2, 6, -2, 4, 7, 3, 7

        .text
        la   x19, input     # address of input string
        addi x20, x0, 10
clear1: addi x28, x0, 0     # i = 0
loop1:  slli x29, x28, 2    # x29 = i * 4
        add  x30, x19, x29  # x30 = &array[i]
        sw    x0, 0(x30)    # array[i] = 0
        addi x28, x28, 1    # i = i + 1
        slt  x31, x28, x20  # x31 = 1 if (i < size)
        bne  x31, x0, loop1 # if (x31 != 0) goto loop1
exit:   nop