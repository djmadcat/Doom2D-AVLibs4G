ideal
p386
model flat,cpp

dataseg

btm dw 0,0
int8sav dd 0
int8sel dw 0
;rmsav dw 0,0
;rmptr dd 0
;rmsel dw 0
cnt dw 0
frq dw 0FFFFh
drv_on db 0

time_proc dd offset __timer_def_timeproc

const

;__timer_rm_proc db 50h,0B0h,20h,0E6h,20h,58h,0CFh

codeseg

proc __timer_def_timeproc
public __timer_def_timeproc
	ret
endp

proc __timer_set_timeproc uses bx, @@f:dword,@@p:dword
public __timer_set_timeproc
	mov dx,12h
	mov ax,34DCh
	div [word @@f]
	cmp ax,20
	jae @@1
	mov ax,20
@@1:    cli
	mov [frq],ax
	mov bx,ax
	mov al,36h
	out 43h,al
	mov al,bl
	out 40h,al
	mov al,bh
	out 40h,al
	mov eax,[@@p]
	mov [time_proc],eax
	sti
	ret
endp

proc __timer_init uses ebx ecx esi edi es
public __timer_init
	cmp [drv_on],0
	jne @@x
;        mov ax,100h
;        mov bx,1
;        int 31h
;        jc @@err
;        mov [rmsel],dx
;        movzx eax,ax
;        shl eax,4
;        mov [rmptr],eax
;        mov edi,eax
;        mov esi,offset __timer_rm_proc
;        push ds
;        pop es
;        cld
;        mov ecx,7
;        rep movsb
	mov ah,0
	int 1Ah
	mov [btm],dx
	mov [btm+2],cx
	cli
	mov ax,3508h
	int 21h
	mov [int8sav],ebx
	mov [int8sel],es
;        mov ax,200h
;        mov bl,8
;        int 31h
;        mov [rmsav],dx
;        mov [rmsav+2],cx
	push ds
	mov ax,2508h
	push cs
	pop ds
	mov edx,offset TimeDriver
	int 21h
	pop ds
;        mov ecx,[rmptr]
;        shr ecx,4
;        xor edx,edx
;        mov ax,201h
;        mov bl,8
;        int 31h
	mov bx,0FFFFh
	mov [frq],bx
	mov al,36h
	out 43h,al
	mov al,bl
	out 40h,al
	mov al,bh
	out 40h,al
	mov [drv_on],1
	sti
@@x:    mov eax,1
	ret
@@err:  xor eax,eax
	ret
endp

proc __timer_close uses ebx ecx
public __timer_close
	cmp [drv_on],0
	je @@x
	cli
	mov al,36h
	out 43h,al
	xor al,al
	out 40h,al
	xor al,al
	out 40h,al
;        mov ax,201h
;        mov bl,8
;        mov dx,[rmsav]
;        mov cx,[rmsav+2]
;        int 31h
	push ds
	mov ax,2508h
	lds edx,[fword int8sav]
	int 21h
	pop ds
	mov [drv_on],0
	mov ah,1
	mov dx,[btm]
	mov cx,[btm+2]
	int 1Ah
;        mov ax,101h
;        mov dx,[rmsel]
;        int 31h
	sti
@@x:    ret
endp

proc TimeDriver
	push eax ds
	cld
	mov ax,DGROUP
	mov ds,ax
	mov ax,[frq]
	add [cnt],ax
	jnc @@1
	inc [dword btm]
	cmp ax,0FFFFh
	je @@2
@@1:    call [time_proc]
	mov al,20h
	out 20h,al
	pop ds eax
	iretd
@@2:    pushfd
	call [fword int8sav]
	call [time_proc]
	pop ds eax
	iretd
endp

end
