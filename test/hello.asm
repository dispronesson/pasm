    mov #2000, sp
    mov #5, r0
    mov #"g", r1
check:
    tst @#177564
    beq check
print:
    mov r1, @#177566
wait:
    tst @#177564
    beq wait
    inc r1
    dec r0
    bne print
exit:
    halt
