ideal
p386
model flat,cpp

RMUL=77
GMUL=151
BMUL=28

udataseg

pal db 256*4 dup(?)
palf dw ?

dataseg

paln dw 0

codeseg

proc V_remap uses ebx ecx edi, @@p:dword,@@m:dword
public V_remap
	mov edi,[@@p]
	mov ebx,[@@m]
	mov ax,[edi]
	mul [word edi+2]
	movzx ecx,ax
	add edi,8
	cld
@@1:	mov al,[edi]
	segds xlat
	stosb
	loop @@1
	ret
endp

proc VP_brightness uses bx, @@r:dword,@@g:dword,@@b:dword
public VP_brightness
	mov al,RMUL
	mul [byte @@r]
	mov bx,ax
	mov al,GMUL
	mul [byte @@g]
	add bx,ax
	mov al,BMUL
	mul [byte @@b]
	add ax,bx
	mov al,ah
	xor ah,ah
	ret
endp

end
