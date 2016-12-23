ideal
p386
model flat,cpp

dataseg

port dw 22Ch

extrn __snd_sampler:dword

codeseg

proc pcspeaker_timeproc uses ebx edx es
public pcspeaker_timeproc
	push ds
	pop es
	call [__snd_sampler]
	xor ah,80h
	push ax
	mov dx,[port]
	mov ah,10h
	call near sbout
	pop ax
	call near sbout
	ret
endp

proc sbout
@@1:    in al,dx
	or al,al
	js @@1
	mov al,ah
	out dx,al
	ret
endp

proc pcspeaker_init uses eax edx
public pcspeaker_init
	mov dx,[port]
	mov ah,0D1h
	call near sbout
	ret
endp

proc pcspeaker_close uses eax edx
public pcspeaker_close
	mov dx,[port]
	mov ah,0D3h
	call near sbout
	ret
endp

end
