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
comp_mul:
pusha
mov ax,w mu1l
mul w mu2l
mov w varu,dx
mov w varl,ax
mov ax,w mu1l
mul w mu2u
add ax,w varu
mov w varu,ax
mov ax,w mu1u
mul w mu2l
add ax, w varu
mov w varu,ax
mov w mu1u,w varu
mov w mu1l,w varl
popa
ret
varu dw 0
varl dw 0
mu1u dw 0
mu1l dw 0
mu2u dw 0
mu2l dw 0
comp_pow:
pusha
cmp w po2l,1h
jne pow_skip
popa
ret
pow_skip:
mov bx,w po2l
mov w paddu,w po1u
mov w paddl,w po1l
mov w po1l,1h
mov w po1u,0h
pow_loop:
mov ax,bx
and ax,1h
jz pow_not_one
mov w mu1l, w po1l
mov w mu1u, w po1u
mov w mu2l, w paddl
mov w mu2u, w paddu
call comp_mul
mov  w po1l,w mu1l
mov  w po1u,w mu1u
pow_not_one:
mov w mu1l, w paddl
mov w mu1u, w paddu
mov w mu2l, w paddl
mov w mu2u, w paddu
call comp_mul
mov  w paddl,w mu1l
mov  w paddu,w mu1u
shr bx,1
jnz pow_loop
popa
ret
po1u dw 0
po1l dw 0
po2u dw 0
po2l dw 0
paddu dw 0
paddl dw 0
main:
mov w _1l,00001h
mov w _1u,00000h
mov w mu1l,w _1l
mov w mu1u,w _1u
mov w mu2l,00001h
mov w mu2u,00000h
call comp_mul
mov w _1l,w mu1l
mov w _1u,w mu1u
mov w $1l,w _1l
mov w $1u,w _1u
mov w _2l,00003h
mov w _2u,00000h
mov w po1l,w _2l
mov w po1u,w _2u
mov w po2l,00002h
mov w po2u,00000h
call comp_pow
mov w _2l,w po1l
mov w _2u,w po1u
mov w $2l,w _2l
mov w $2u,w _2u
mov w _3l,w $2l
mov w _3u,w $2u
mov w[outhex+2],_3l
mov w[outhex],_3u
call print_hex
mov ah,4ch
mov al,00
int 21h
_1u dw 0
_1l dw 0
_2u dw 0
_2l dw 0
_3u dw 0
_3l dw 0
$1u dw 0
$1l dw 0
$2u dw 0
$2l dw 0
