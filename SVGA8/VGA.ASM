ideal
p386
model flat,cpp

extrn error:proc

macro delay
local a,b,c
jmp short a
a:
jmp short b
b:
jmp short c
c:
endm

macro CutLin
local a
call CutLine
or	bx,bx
jnz	a
ret
a:
endm

dataseg

vp_waitrr db 1
public vp_waitrr

scrbuf dd 0
public scrbuf

udataseg

extrn sv_bpl:dword

scra dd ?
scrx dw ?
scrw dw ?
scry dw ?
scrh dw ?

pal_tmp db 256*3 dup(?)

_xlen dw ?
_ylen dw ?
_stepy dd ?

public scra,scrx,scrw,scry,scrh
public pal_tmp


codeseg

proc V_dot uses ebx edx, @@x:dword,@@y:dword,@@c:dword
public V_dot
	mov ax,[scrx]
	cmp [word @@x],ax
	jl @@ex
	add ax,[scrw]
	cmp [word @@x],ax
	jge @@ex
	mov ax,[scry]
	cmp [word @@y],ax
	jl @@ex
	add ax,[scrh]
	cmp [word @@y],ax
	jge @@ex
	mov eax,[@@y]
	mul [sv_bpl]
	add eax,[@@x]
	mov ebx,eax
	add ebx,[scra]
	mov al,[byte @@c]
	mov [ebx],al
@@ex:	ret
endp

proc CutLine near uses si di
;ax=pos,bx=len,cx=spos,dx=slen
mov	si,cx
add	si,dx
cmp	ax,si
jl	@@a
	mov	bx,0
	ret
@@a:
cmp	ax,cx
jge	@@1
add	ax,bx
cmp	ax,cx
jg	@@b
	mov	bx,0
	ret
@@b:
mov	bx,ax
sub	bx,cx
mov	ax,cx
@@1:
mov	di,ax
add	di,bx
cmp	di,si
jng	@@1a
	mov	bx,si
	sub	bx,ax
@@1a:
ret
endp

macro prep_img
mov	esi,[@@adr]
mov	ax,[word @@x]
sub ax,[esi+4]
mov [word @@x],ax
mov	bx,[esi]
mov	cx,[scrx]
mov	dx,[scrw]
CutLin
mov	[_xlen],bx
mov	dx,[esi]
sub	dx,bx
movzx edx,dx
mov	[_stepy],edx
mov	di,ax
sub	di,[word @@x]
movzx edi,di
mov	[word @@x],ax
mov	ax,[word @@y]
sub ax,[esi+6]
mov [word @@y],ax
mov	bx,[esi+2]
mov	cx,[scry]
mov	dx,[scrh]
CutLin
mov	[_ylen],bx
mov	bx,ax
sub	ax,[word @@y]
mul	[word esi]
shl edx,16
movzx eax,ax
or eax,edx
add	edi,eax
mov	[word @@y],bx
add	esi,edi
add	esi,8
movzx eax,[word @@y]
mul [sv_bpl]
movzx edi,[word @@x]
add edi,eax
add	edi,[scra]
mov ax,ds
mov es,ax
endm

proc V_pic uses ebx edx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword
public V_pic
	prep_img
	mov edx,edi
	movzx ebx,[_xlen]
	mov eax,[_stepy]
	cld
@@1:
	mov edi,edx
	mov ecx,ebx
	rep movsb
	add esi,eax
	add edx,[sv_bpl]
	dec [_ylen]
	jnz @@1
ret
endp

proc V_spr1color uses ebx edx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword,@@c:dword
public V_spr1color
prep_img
mov ah,[byte @@c]
mov edx,edi
mov ebx,[_stepy]
cld
	@@1:
	mov edi,edx
		movzx ecx,[_xlen]
		@@2:
		lodsb
		or al,al
		jnz @@2a
		inc edi
		jmp short @@2b
		@@2a:
		mov al,ah
		stosb
		@@2b:
		loop @@2
	add esi,ebx
	add edx,[sv_bpl]
	dec [_ylen]
	jnz @@1
ret
endp

proc V_spr uses ebx edx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword
public V_spr
prep_img
mov edx,edi
mov ebx,[_stepy]
cld
	@@1:
	mov edi,edx
		movzx ecx,[_xlen]
		@@2:
		lodsb
		or al,al
		jnz @@2a
		inc edi
		jmp short @@2b
		@@2a:
		stosb
		@@2b:
		loop @@2
	add esi,ebx
	add edx,[sv_bpl]
	dec [_ylen]
	jnz @@1
ret
endp

proc V_sprf uses ebx edx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword,@@f:dword
public V_sprf
prep_img
mov edx,edi
mov ebx,[_stepy]
cld
	@@1:
	mov edi,edx
		movzx ecx,[_xlen]
		@@2:
		lodsb
		or al,al
		jnz @@2a
		inc edi
		jmp short @@2b
		@@2a:
		call [@@f]
		@@2b:
		loop @@2
	add esi,ebx
	add edx,[sv_bpl]
	dec [_ylen]
	jnz @@1
ret
endp

proc V_spr2 uses ebx edx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword
public V_spr2
mov	esi,[@@adr]
mov	ax,[word @@x]
sub ax,[esi]
add ax,[esi+4]
mov [word @@x],ax
mov	bx,[esi]
mov	cx,[scrx]
mov	dx,[scrw]
CutLin
mov	[_xlen],bx
mov	dx,[esi]
sub	dx,bx
movzx edx,dx
mov	[_stepy],edx
mov di,[word @@x]
add di,[esi]
sub di,ax
sub di,bx
movzx edi,di
add ax,bx
dec ax
mov	[word @@x],ax
mov	ax,[word @@y]
sub ax,[esi+6]
mov [word @@y],ax
mov	bx,[esi+2]
mov	cx,[scry]
mov	dx,[scrh]
CutLin
mov	[_ylen],bx
mov	bx,ax
sub	ax,[word @@y]
mul	[word ptr esi]
shl edx,16
movzx eax,ax
or eax,edx
add	edi,eax
mov	[word @@y],bx
add	esi,edi
add	esi,8
movzx eax,[word @@y]
mul [sv_bpl]
movzx edi,[word @@x]
add edi,eax
add	edi,[scra]
mov ax,ds
mov es,ax
mov edx,edi
mov ebx,[_stepy]
	@@1:
	mov edi,edx
		movzx ecx,[_xlen]
		@@2:
		cld
		lodsb
		or al,al
		jnz @@2a
		dec edi
		jmp short @@2b
		@@2a:
		std
		stosb
		@@2b:
		loop @@2
	add esi,ebx
	add edx,[sv_bpl]
	dec [_ylen]
	jnz @@1
cld
ret
endp

proc V_clr uses ebx edx ecx si edi, @@x:dword,@@w:dword,@@y:dword,@@h:dword,@@color:dword
public V_clr
mov ax,[word @@x]
mov bx,[word @@w]
mov cx,[scrx]
mov dx,[scrw]
CutLin
mov [word @@x],ax
mov [word @@w],bx
mov ax,[word @@y]
mov bx,[word @@h]
mov cx,[scry]
mov dx,[scrh]
CutLin
mov [word @@y],ax
mov [word @@h],bx
mov ax,ds
mov es,ax
movzx eax,[word @@y]
mul [sv_bpl]
movzx ebx,[word @@x]
add ebx,eax
add ebx,[scra]
cld
mov al,[byte @@color]
mov si,[word @@h]
mov dx,[word @@w]
	@@1:
	mov edi,ebx
	movzx ecx,dx
	rep stosb
	add ebx,[sv_bpl]
	dec si
	jnz @@1
ret
endp

proc VP_tocolor uses ecx esi edi, @@pal:dword,@@R:dword,@@G:dword,@@B:dword,@@N:dword,@@S:dword
public VP_tocolor
	mov edi,[@@pal]
	mov esi,offset pal_tmp
	mov ecx,256
@@1:	mov al,[byte @@R]
	sub al,[edi]
	imul [byte @@S]
	idiv [byte @@N]
	add al,[edi]
	mov [esi],al
	inc esi
	inc edi
	mov al,[byte @@G]
	sub al,[edi]
	imul [byte @@S]
	idiv [byte @@N]
	add al,[edi]
	mov [esi],al
	inc esi
	inc edi
	mov al,[byte @@B]
	sub al,[edi]
	imul [byte @@S]
	idiv [byte @@N]
	add al,[edi]
	mov [esi],al
	inc esi
	inc edi
	loop @@1
	ret
endp

proc VP_topal uses ecx esi edi, @@p1:dword,@@p2:dword,@@N:dword,@@S:dword
public VP_topal
	push ds
	pop es
	mov edi,offset pal_tmp
	mov esi,[@@p1]
	mov ecx,256*3/4
	cld
	rep movsd
	mov esi,offset pal_tmp
	mov edi,[@@p2]
	mov ecx,256*3
@@2:	mov al,[edi]
	sub al,[esi]
	imul [byte @@S]
	idiv [byte @@N]
	add [esi],al
	inc esi
	inc edi
	loop @@2
	ret
endp

proc V_setrect near @@x:dword,@@w:dword,@@y:dword,@@h:dword
public V_setrect
	mov ax,[word @@x]
	mov [scrx],ax
	mov ax,[word @@y]
	mov [scry],ax
	mov ax,[word @@w]
	mov [scrw],ax
	mov ax,[word @@h]
	mov [scrh],ax
	ret
endp

proc V_setscr near @@p:dword
public V_setscr
	mov eax,[@@p]
	or eax,eax
	jnz @@1
	mov eax,[scrbuf]
@@1:	mov [scra],eax
	ret
endp

end
