start:
    mov #stack, sp
    add #28d, sp

    mov #string, r0
    jsr pc, print
    add #2, sp

exit:
    halt


print:
    tstb @RXS
    beq print
    movb (r0)+, @RXD
    bne print
    rts pc

string: .byte "Hello, World!", 13d, 10d, 0
RXS: .word 177564
RXD: .word 177566

stack: .blkb 32d