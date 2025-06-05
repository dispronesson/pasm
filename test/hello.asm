start:
    mov #stack, sp
    add #28d, sp

    mov #string, r0
    jsr pc, print
    add #2, sp

exit:
    halt


print:
    tstb @TCSR
    beq print
    movb (r0)+, @TBUF
    bne print
    rts pc

string: .byte "Hello, World!", 13d, 10d, 0
TCSR: .word 177564
TBUF: .word 177566

stack: .blkb 32d