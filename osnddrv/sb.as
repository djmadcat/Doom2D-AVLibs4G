ideal
p386
model flat,cpp

BUFSZ=1024

const

drv_ftab dd get_info,detect,init,shutdown,set_dma
drv_name db 'Sound Blaster',0
drv_par dw 7, 220h,210h,230h,240h,250h,260h,280h
	dw 0
	dw 9, 7,5,3,4,9,10,11,12,15

dataseg

port dw 220h
irq db 7
label irqsav pword
  dd ?
  dw ?
mproc dd ?
dmabuf dd ?
bufsel dw 0
dmaf db 0
sbfreq db ?

proc sbout near
	mov dx,[port]
	add dx,12
@@1:	in al,dx
	or al,al
	js @@1
	mov al,ah
	out dx,al
	ret
endp

proc irqproc
	push ax cx dx
	mov dx,[port]
	add dx,14
	in al,dx
	mov ah,14h
	call near sbout
	mov cx,BUFSZ-1
	mov ah,cl
	call near sbout
	mov ah,ch
	call near sbout
	mov al,20h
	cmp [cs:irq],8
	jb @@a
	out 0A0h,al
@@a:	out 20h,al
	pop dx cx ax
	iretd
endp

proc __sb_drv
public __sb_drv
	cmp ebx,4
	ja @@err
	shl ebx,2
	jmp [drv_ftab+ebx]
@@err:	xor ax,ax
	ret
endp

proc get_info
	mov ax,1
	lea ebx,[drv_name]
	lea ecx,[drv_par]
	ret
endp

proc detect
	mov dx,ax
	add dx,12
	mov cx,500
@@1:	in al,dx
	or al,al
	jns @@ok
	loop @@1
	xor ax,ax
	ret
@@ok:	mov ax,1
	ret
endp

proc init
	cli
	mov [port],ax
	mov [irq],dl
	mov ah,35h
	mov al,[irq]
	add al,8
	cmp al,16
	jb @@a
	add al,60h
@@a:	int 21h
	mov [dword irqsav],ebx
	mov [word irqsav+4],es
	push ds
	mov ah,25h
	mov al,[irq]
	add al,8
	cmp al,16
	jb @@b
	add al,60h
@@b:	push cs
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
@@c:	in al,dx
	and al,ah
	out dx,al
	mov ah,0D1h
	call near sbout
	sti
	ret
endp

proc shutdown
	cmp [bufsel],0
	je @@nobuf
	mov ax,101h
	mov dx,[bufsel]
	int 31h
@@nobuf:
	cli
	mov ah,0D0h
	call near sbout
	mov dx,[port]
	add dx,14
	in al,dx
	mov ah,0D3h
	call near sbout
	push ds
	mov ah,25h
	mov al,[irq]
	add al,8
	cmp al,16
	jb @@a
	add al,60h
@@a:	lds edx,[irqsav]
	int 21h
	pop ds
	mov cl,[irq]
	mov ah,1
	rol ah,cl
	mov dx,21h
	cmp [irq],8
	jb @@c
	mov dl,0A1h
@@c:	in al,dx
	or al,ah
	out dx,al
	sti
	ret
endp

proc sbpoll near
	push eax ebx ecx edx esi edi ebp ds es
	in al,3
	mov cl,al
	in al,3
	mov ch,al
	cmp [dmaf],0
	jne @@2
	cmp cx,BUFSZ/2
	jae @@x
	mov edi,[dmabuf]
	mov ecx,BUFSZ/2
@@1:	push ds
	pop es
	call [mproc]
	xor [dmaf],1
	jmp @@x
@@2:	cmp cx,BUFSZ/2
	jb @@x
	mov edi,[dmabuf]
	mov ecx,BUFSZ/2
	add edi,ecx
	jmp @@1
@@x:	pop es ds ebp edi esi edx ecx ebx eax
	ret
endp

proc set_dma
	cli
	mov [dmaf],0
	mov [mproc],eax
	push cx
	mov dx,0Fh
	mov ax,4240h
	div cx
	neg al
	mov [sbfreq],al
	mov ax,100h
	mov bx,(BUFSZ*2+15)/16
	int 31h
	jc @@x
	movzx eax,ax
	shl eax,4
	mov [bufsel],dx
	mov [dmabuf],eax
	movzx ecx,ax
	neg cx
	cmp cx,BUFSZ
	ja @@ok
	add [dmabuf],ecx
	add eax,ecx
@@ok:	push eax
	pop bx dx
	mov al,5
	out 0Ah,al
	xor al,al
	out 0Ch,al
	mov al,bl
	out 2,al
	mov al,bh
	out 2,al
	mov al,dl
	out 83h,al
	mov ax,BUFSZ-1
	out 3,al
	mov al,ah
	out 3,al
	mov al,48h+1
	out 0Bh,al
	mov al,1
	out 0Ah,al
	mov edi,[dmabuf]
	mov ecx,BUFSZ
	push ds
	pop es
	mov al,80h
	cld
	rep stosb
	mov ah,40h
	call near sbout
	mov ah,[sbfreq]
	call near sbout
	mov ah,14h
	call near sbout
	mov cx,BUFSZ-1
	mov ah,cl
	call near sbout
	mov ah,ch
	call near sbout
	pop cx
	shr cx,8
	cmp cx,20
	jae @@1
	mov cx,20
@@1:	mov eax,offset sbpoll
	sti
	ret
@@x:	pop cx
	xor cx,cx
	sti
	ret
endp

end
