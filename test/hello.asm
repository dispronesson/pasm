start:
    mov #2000, sp

    mov #"g", -(sp)
    mov #5, -(sp)
    jsr pc, print

    add #4, sp

    halt


print:
    mov r0, -(sp)
    mov r1, -(sp)

    mov 10(sp), r0
    mov 6(sp), r1

    br check
    
print_loop:
    tst @#177564
    beq print_loop
    mov r0, @#177566
    inc r0
    dec r1

check:
    tst r1
    bne print_loop

last_wait:
    tst @#177564
    beq last_wait

    mov (sp)+, r1
    mov (sp)+, r0

    rts pc