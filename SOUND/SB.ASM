ideal
p386
model flat,cpp

BUFSZ=1024

const

dmapage db 87h,83h,81h,82h,8Fh,8Bh,89h,8Ah
dmaofs db 0,2,4,6,0C0h,0C4h,0C8h,0CCh
dmacnt db 1,3,5,7,0C2h,0C6h,0CAh,0CEh
dmamask db 0Ah,0Ah,0Ah,0Ah,0D4h,0D4h,0D4h,0D4h
dmamode db 0Bh,0Bh,0Bh,0Bh,0D6h,0D6h,0D6h,0D6h
dmaff db 0Ch,0Ch,0Ch,0Ch,0D8h,0D8h,0D8h,0D8h

dataseg

port dw 220h
dma dd 1
irq db 7
sbtype db 0
smptype db 0
label irqsav pword
  dd ?
  dw ?
mproc dd ?
dmasize dd ?
dmabuf dd ?
bufsel dw 0
dmaf db 0

udataseg

extrn __snd_sampler:dword

codeseg

proc sbout
	mov dx,[port]
	add dx,12
@@1:    in al,dx
	or al,al
	js @@1
	mov al,ah
	out dx,al
	ret
endp

proc reset
	mov dx,[port]
	add dx,6
	mov al,1
	out dx,al
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	xor al,al
	out dx,al
	add dx,8
	xor cx,cx
@@1:    dec cx
	jz @@1x
	in al,dx
	or al,al
	jns @@1
@@1x:   sub dx,4
	in al,dx
	ret
endp

proc sblaster_init uses ebx ecx, @@p:dword,@@d:dword,@@i:dword,@@t:dword,@@st:dword
public sblaster_init
	cli
	mov eax,[@@t]
	mov [sbtype],al
	mov eax,[@@st]
	mov [smptype],al
	mov eax,[@@p]
	mov [port],ax
	mov edx,[@@i]
	mov [irq],dl
	mov ecx,[@@d]
	and ecx,0Fh
	mov [dma],ecx
	call reset
	mov ah,35h
	mov al,[irq]
	add al,8
	cmp al,16
	jb @@a
	add al,60h
@@a:    int 21h
	mov [dword irqsav],ebx
	mov [word irqsav+4],es
	push ds
	mov ah,25h
	mov al,[irq]
	add al,8
	cmp al,16
	jb @@b
	add al,60h
@@b:    push cs
	pop ds
	mov edx,offset irqproc
	int 21h
	pop ds
	mov cl,[irq]
	mov ah,0FEh
	rol ah,cl
	mov dx,21h
	cmp [irq],8
	jb @@c
	mov dl,0A1h
@@c:    in al,dx
	and al,ah
	out dx,al
	mov ah,0D1h
	call sbout

@@x:    sti
	ret
endp

proc sblaster_shutdown uses ebx ecx
public sblaster_shutdown
	cli
	mov dx,[port]
	add dx,14
	in al,dx
	call reset
	mov ah,0D3h
	call sbout
	cmp [bufsel],0
	je @@nobuf
	mov ax,101h
	mov dx,[bufsel]
	int 31h
@@nobuf:
	push ds
	mov ah,25h
	mov al,[irq]
	add al,8
	cmp al,16
	jb @@a
	add al,60h
@@a:    lds edx,[irqsav]
	int 21h
	pop ds
	mov cl,[irq]
	mov ah,1
	rol ah,cl
	mov dx,21h
	cmp [irq],8
	jb @@c
	mov dl,0A1h
@@c:    in al,dx
	or al,ah
	out dx,al

@@x:    sti
	ret
endp

proc irqproc
	push eax ebx ecx edx esi edi ebp ds es
	mov ax,DGROUP
	mov ds,ax

	mov edi,[dmabuf]
	mov ecx,[dmasize]
	shr ecx,1
	cmp [dmaf],0
	je @@1
	add edi,ecx
@@1:    push ds
	pop es
	cld
	call [mproc]
	xor [dmaf],1

	mov dx,[port]
	add dx,10
	in al,dx
	add dx,4
	in al,dx
	cmp [sbtype],2
	jb @@1a
	inc dx
	in al,dx
	mov ah,045h
	test [smptype],2
	jnz @@1b
	mov ah,047h
@@1b:   call sbout
@@1a:
	mov al,20h
	cmp [irq],8
	jb @@2
	out 0A0h,al
@@2:    out 20h,al
	pop es ds ebp edi esi edx ecx ebx eax
	iretd
endp

proc sb0mproc
@@1:    call [__snd_sampler]
	mov al,ah
	xor al,80h
	stosb
	loop @@1
	ret
endp

proc sb1mproc
	shr ecx,1
@@1:    call [__snd_sampler]
	mov al,ah
	mov ah,bh
	xor ax,8080h
	stosw
	loop @@1
	ret
endp

proc sb2mproc
	shr ecx,1
@@1:    call [__snd_sampler]
	stosw
	loop @@1
	ret
endp

proc sb3mproc
	shr ecx,2
@@1:    call [__snd_sampler]
	stosw
	mov ax,bx
	stosw
	loop @@1
	ret
endp

proc sblaster_setrate uses ebx, @@f:dword
public sblaster_setrate
	mov ah,40h
	call sbout
	mov eax,[@@f]
	shr eax,1
	add eax,1000000
	xor edx,edx
	div [@@f]
	push eax
	neg al
	mov ah,al
	call sbout
	pop ebx
	mov eax,1000000
	xor edx,edx
	div ebx
	ret
endp

mptab dd offset sb0mproc,offset sb2mproc,offset sb1mproc,offset sb3mproc

proc sblaster_startdma uses ebx ecx edi
public sblaster_startdma
	cli
	mov [dmaf],0
	movzx ebx,[smptype]
	mov eax,[mptab+ebx*4]
	mov [mproc],eax
	mov ebx,BUFSZ
	test [smptype],1
	jz @@a
	shl ebx,1
@@a:    test [smptype],2
	jz @@b
	shl ebx,1
@@b:    mov [dmasize],ebx
	shl ebx,1
	add ebx,15
	shr ebx,4
	mov ax,100h
	int 31h
	jc @@x
	movzx eax,ax
	shl eax,4
	mov [bufsel],dx
	mov [dmabuf],eax
	movzx ecx,ax
	neg cx
	cmp cx,[word dmasize]
	ja @@ok
	add [dmabuf],ecx
@@ok:
	mov edi,[dmabuf]
	mov ecx,[dmasize]
	push ds
	pop es
	cld
	test [smptype],1
	jnz @@clr2
	mov al,80h
	rep stosb
	jmp @@ok1
@@clr2: shr ecx,1
	mov ax,0
	rep stosw
@@ok1:
	mov eax,[dmabuf]
	cmp [dma],4
	jb @@1
	shr eax,1
	push eax
	pop bx cx
	shl cl,1
	push cx bx
	pop eax
@@1:    push eax
	pop bx cx

	xor edx,edx
	mov eax,[dma]
	mov dl,[dmamask+eax]
	and al,3
	or al,4
	out dx,al

	xor al,al
	mov edx,[dma]
	mov dl,[dmaff+edx]
	out dx,al

	mov al,cl
	mov edx,[dma]
	mov dl,[dmapage+edx]
	out dx,al

	mov al,bl
	mov edx,[dma]
	mov dl,[dmaofs+edx]
	out dx,al
	mov al,bh
	out dx,al

	mov eax,[dmasize]
	cmp [dma],4
	jb @@2
	shr ax,1
@@2:    dec ax
	mov edx,[dma]
	mov dl,[dmacnt+edx]
	out dx,al
	mov al,ah
	out dx,al

	mov eax,[dma]
	and al,3
	or al,58h
	mov edx,[dma]
	mov dl,[dmamode+edx]
	out dx,al

	mov eax,[dma]
	and al,3
	mov edx,[dma]
	mov dl,[dmamask+edx]
	out dx,al

	cmp [sbtype],0
	je @@sb0
	cmp [sbtype],1
	je @@sb1

	mov ah,0C6h
	test [smptype],1
	jz @@16a
	mov ah,0B6h
@@16a:  call sbout
	mov ah,[smptype]
	shl ah,4
	and ah,30h
	call sbout
	mov ecx,[dmasize]
	shr ecx,1
	test [smptype],1
	jz @@16b
	shr ecx,1
@@16b:  dec ecx
	mov ah,cl
	call sbout
	mov ah,ch
	call sbout
	mov ah,47h
	test [smptype],2
	jz @@16c
	mov ah,45h
@@16c:  call sbout
	jmp @@end
@@sb1:
	test [smptype],2
	jz @@sb0
	mov ah,48h
	call sbout
	mov ecx,[dmasize]
	shr ecx,1
	dec ecx
	mov ah,cl
	call sbout
	mov ah,ch
	call sbout
	mov ah,91h
	call sbout
	jmp @@end
@@sb0:
	mov ah,48h
	call sbout
	mov ecx,[dmasize]
	shr ecx,1
	dec ecx
	mov ah,cl
	call sbout
	mov ah,ch
	call sbout
	mov ah,1Ch
	call sbout
	jmp @@end
@@end:
	mov eax,1
	sti
	ret
@@x:    xor eax,eax
	sti
	ret
endp

end
