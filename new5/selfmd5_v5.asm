; new5: denser weave + last-block MD5 + IV[3]==0 撞bit. Aim <299.
%define v0 edx
%define v1 ebx
%define v2 ebp
%define v3 edi
%define v0q rdx
%define v1q rbx
%define v2q rbp
%define v3q rdi
%define idxs r13
%define ii eax
%define iiq rax
%define iib al
%define gg ecx
%define ggq rcx
%define ff esi
%define ffq rsi
padding_size_off equ padding_size - padding_byte
r_off equ r - padding_byte
BITS 64
org 0x9ef000
base:
    db 0x7f,"ELF",2,1
start0:
    mov esi, padding_byte-0xC
    push rsi
    lodsd
    xchg eax, v0
    lodsd
    db 0xbb
    dw 2,0x3e
start1:
    xchg eax, v1
    lodsd
    xchg eax, v2
    db 0xb8
    dq start0
    dq 0x38
start2:
    mov byte [rsi], 0x80
    mov word [rsi+padding_size_off], filesize*8
    xchg eax, ecx
    jmp start4
    dw 0x40,0x38,1,0
    db 0x7
start3:
    db 0x99,0xc7,0xc0
    dq 0
    dq $$
start4:
    lea idxs, [rsi+r_off]
    xor edi, edi
    jmp start5
    dq filesize
start5:
    cmp iib, 0xF
    jmp start
    db 0,0,0,0
start:
loop1_start:
    mov ff, v2
    cmp iib, 0xF
    ja short .r1
    xor ff, v3
    and ff, v1
    mov gg, ii
    jmp short .e4
.r1:
    xor ff, v1
    cmp iib, 0x1F
    ja short .r2
    and ff, v3
    lea gg, [iiq+iiq*4+1]
    jmp short .e3
.r2:
    lea gg, [iiq+iiq*2+5]
.e4:
    xor ff, v3
    cmp iib, 0x2F
    jbe short .e2
.r3:
    imul gg, ii, 7
    mov ff, v3
    not ff
    or ff, v1
.e3:
    xor ff, v2
.e2:
    and gg, 0xF
    add v0, dword [idxs+ggq*4+data-r]
    add v0, ff
    inc ii
    push iiq
    fild dword [rsp]
    fsin
    push 0x4f800000
    fmul dword [rsp]
    fabs
    fisttp qword [rsp]
    pop ffq
    add v0, ff
    dec ii
    mov ff, ii
    shr ff, 4
    and iib, 3
    add iiq, idxs
    xchg ii, v3
    mov cl, byte [v3q+ffq*4]
    rol v0, cl
    add v0, v1
    xchg ii, v0
    xchg ii, v1
    xchg ii, v2
    xchg ii, v3
    pop iiq
    cmp iib, 0x40
    jnz short loop1_start
    pop rsi
    add [rsi], v0
    add [rsi+4], v1
    add [rsi+8], v2
    mov [rsi+12], v3
    pop rdx
    mov cl, 0x10
    mov bl, cl
    pop rdi
    push rdi
.hex:
    lodsb
    div bl
.nib:
    add al, 0x30
    cmp al, 0x3a
    jl .d
    add al, 39
.d: stosb
    shr eax, 8
    test edi, edx           ; argv ptr parity → always both nibbles if even
    jnz .nib
    loop .hex
    pop rsi
    mov edi, edx
    xchg eax, edx
    mov dl, 32
    syscall
    mov al, 60
    pop rdi
    syscall
r:  db 7,12,17,22,5,9,14,20,4,11,16,23,6,10,15,21
iv: dd 0x6b2ff0ee, 0xc494181d, 0xca6e2e13
padding_byte:
filesize equ $-$$
align_addr equ (filesize+63) & ~63
data equ align_addr-64
padding_size equ align_addr-8
