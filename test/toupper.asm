start:
    mov #stack, sp
    add #30d, sp

    mov #msg_input_str, -(sp)
    jsr pc, print_msg
    add #2, sp

    mov #string, -(sp)
    jsr pc, input_str

    jsr pc, to_upper
    add #2, sp

    mov #msg_output_str, -(sp)
    jsr pc, print_msg
    add #2, sp

    mov #string, -(sp)
    jsr pc, print_msg
    add #2, sp

    halt


print_msg:
    mov 2(sp), r0

msg_loop:
    tstb @TCSR
    beq msg_loop

    movb (r0)+, @TBUF
    bne msg_loop

    rts pc


print_char:
    movb 2(sp), r2

char_loop:
    tstb @TCSR
    beq char_loop

    movb r2, @TBUF
    rts pc



input_str:
    mov 2(sp), r0

str_loop:
    tstb @RCSR
    beq str_loop

    movb @RBUF, r1
    cmpb #13d, r1
    beq str_end

    cmpb #127d, r1
    beq handle_backspace

    movb r1, -(sp)
    jsr pc, print_char
    add #2, sp

    movb r1, (r0)+
    br str_loop

handle_backspace:
    cmp r0, 2(sp)
    beq str_loop

    movb #8d, -(sp)
    jsr pc, print_char
    add #2, sp

    movb #' ', -(sp)
    jsr pc, print_char
    add #2, sp

    movb #8d, -(sp)
    jsr pc, print_char
    add #2, sp

    dec r0
    br str_loop

str_end:
    movb #13d, (r0)+
    movb #10d, (r0)+
    movb #0, (r0)
    rts pc


to_upper:
    mov 2(sp), r0

upper_loop:
    movb (r0), r1
    beq upper_done
    
    cmpb r1, #'a'
    blt next_char
    cmpb r1, #'z'
    bgt next_char
    bicb #40, r1
    movb r1, (r0)

next_char:
    inc r0
    br upper_loop

upper_done:
    rts pc


string: .blkb 32d
msg_input_str: .byte "Enter a string: ", 0
msg_output_str: .byte 13d, 10d, "New string: ", 0

RCSR: .word 177560
RBUF: .word 177562
TCSR: .word 177564
TBUF: .word 177566

stack: .blkb 32d