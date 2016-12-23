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

dataseg

extrn vp_waitrr:byte

codeseg

proc VP_wait
public VP_wait
	cmp [vp_waitrr],0
	je @@x
	mov dx,3DAh
@@1:	in al,dx
	test al,8
	jnz @@1
@@2:	in al,dx
	test al,8
	jz @@2
@@x:	ret
endp

proc VP_getall uses ecx edi, @@p:dword
public VP_getall
	call VP_wait
	mov edi,[@@p]
	cld
	mov dx,3C7h
	xor al,al
	out dx,al
	delay
	mov dl,0C9h
	mov ecx,256*3
@@1:	in al,dx
	delay
	stosb
	loop @@1
	ret
endp

proc _VP_set uses ecx esi, @@p:dword,@@r:dword,@@n:dword
public _VP_set
	call VP_wait
	mov dx,3C8h
	mov al,[byte @@r]
	out dx,al
	delay
	inc dx
	movzx ecx,[word @@n]
	mov eax,ecx
	add ecx,ecx
	add ecx,eax
	mov esi,[@@p]
@@1:	lodsb
	out dx,al
	delay
	loop @@1
	ret
endp

proc _VP_fill uses ecx, @@r:dword,@@g:dword,@@b:dword
public _VP_fill
	call VP_wait
	mov dx,3C8h
	xor al,al
	out dx,al
	delay
	inc dx
	mov ecx,256
@@1:	mov al,[byte @@r]
	out dx,al
	delay
	mov al,[byte @@g]
	out dx,al
	delay
	mov al,[byte @@b]
	out dx,al
	delay
	loop @@1
	ret
endp

end
