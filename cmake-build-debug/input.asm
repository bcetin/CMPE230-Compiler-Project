jmp main
print_hex:
pusha
mov bx, offset outhex
mov dx, w[bx]
call print_half
mov bx, offset outhex
mov dx, w[bx+2]
call print_half
MOV AH,02
MOV DL,10
INT 21h
popa
ret
print_half:
pusha
mov cx,4
char_loop:
dec cx
mov ax,dx
shr dx,4
and ax,0fh
mov bx, offset string_output
add bx, cx
mov b[bx],al
cmp ax,10
jl is_num
add b[bx],7
is_num:
add b[bx],30h
cmp cx,0
je print_half_done
jmp char_loop
print_half_done:
mov bx,offset string_output+4
mov b[bx],'$'
mov dx, offset string_output
mov ah,09
int 21h
popa
ret
string_output db 5 DUP 0
outhex dw 2 DUP 0
comp_add:
pusha
mov bx,w ad1l
add bx,w ad2l
jnc add_no_carry
inc w ad1u
add_no_carry:
mov w ad1l,bx
mov bx,w ad1u
add bx,w ad2u
mov w ad1u,bx
popa
ret
ad1u dw 0
ad1l dw 0
ad2u dw 0
ad2l dw 0
main:
mov w _1l,0005
mov w _1u,0000
mov w ad1l,w _1l
mov w ad1u,w _1u
mov w ad2l,0003
mov w ad2u,0000
call comp_add
mov w _1l,w ad1l
mov w _1u,w ad1u
mov w $1l,w _1l
mov w $1u,w _1u
mov w _2l,w $1l
mov w _2u,w $1u
mov w[outhex+2],_2l
mov w[outhex],_2u
call print_hex
mov ah,4ch
mov al,00
int 21h
_1u dw 0
_1l dw 0
_2u dw 0
_2l dw 0
$1u dw 0
$1l dw 0
