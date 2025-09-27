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

while_loop_1:
whilecondition_1:
    lda %a
    mov B A
    lda %b
    cmp
    jnc %while_end_1
    lda %c
    out 0
    jmp %while_loop_1
while_end_1:

ifcondition_0:
    lda %c
    mov B A
    lda %b
    cmp
    jc %endif_0
    je %endif_0
startif_0:
    lda %c
    out 0
endif_0:
    lda %c
    out 0

hlt
