.data
a = 0
b = 0
c = 0
tmp0 = 0
tmp1 = 0

.text
start:

    ldi A 10
    sta %a

    lda %a
    sta %b

    ldi A 5
    sta %c

while_loop_0:
whilecondition_0:
    lda %c
    mov B A
    lda %b
    cmp
    jc %while_end_0
    lda %b
    out 0

    ldi A 1
    mov B A
    lda %b
    sub
    sta %b
    jmp %while_loop_0
while_end_0:

hlt
