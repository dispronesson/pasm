start:
    mov #stack, sp
    add #28d, sp

    mov #string, r0
    jsr pc, print
    add #2, sp

    halt


print:
    tstb @#177564
    beq print
    movb (r0)+, @#177566
    bne print

    rts pc

string: .byte "Hello, World!", 13d, 10d, 0

stack: .blkb 32d