.data
a = 0
b = 0
c = 0
d = 0
x = 0
tmp0 = 0
tmp1 = 0

.text
start:

    ldi A 10
    sta %a

    lda %a
    sta %b

    lda %b
    mov B A
    lda %a
    add
    sta %c

multi:
    ldi A 0
    sta %tmp1
    ldi A 5
    sta %tmp0
    lda %c
    mov B A
multi_loop_0:
    lda %tmp1
    add
    sta %tmp1
    lda %tmp0
    dec
    sta %tmp0
    jnz %multi_loop_0
    lda %tmp1
    sta %a
    lda %a
    out 0

div:
    ldi A 0
    sta %tmp0
    ldi A 5
    mov B A
    lda %a
div_loop_0:
    sub
    sta %tmp1
    lda %tmp0
    inc
    sta %tmp0
    lda %tmp1
    cmp
    jnc %div_loop_0
    lda %tmp0
    sta %b
    lda %b
    out 0

condition_0:
    ldi A 10
    mov B A
    ldi A 11
    cmp
    jne %endif_0
startif_0:
    ldi A 5
    sta %d
    lda %d
    out 0

condition_1:
    ldi A 20
    mov B A
    lda %c
    cmp
    jne %endif_1
startif_1:
    lda %b
    mov B A
    lda %a
    add
    sta %a

condition_2:
    ldi A 10
    mov B A
    lda %b
    cmp
    jnc %endif_2
    je %endif_2
startif_2:
    lda %a
    sta %x
    jmp %endelse_0
endif_2:

startelse_0:
    lda %b
    sta %a
endelse_0:
    jmp %endelse_1
endif_1:

startelse_1:
    lda %a
    sta %b
endelse_1:
    jmp %endelse_2
endif_0:

startelse_2:
    ldi A 5
    mov B A
    ldi A 5
    sub
    sta %b
    lda %b
    out 0

condition_3:
    ldi A 10
    mov B A
    lda %b
    cmp
    jnc %endif_3
    je %endif_3
startif_3:
    lda %x
    sta %a
    jmp %endelse_3
endif_3:

startelse_3:
    lda %a
    sta %b
endelse_3:
endelse_2:

hlt
