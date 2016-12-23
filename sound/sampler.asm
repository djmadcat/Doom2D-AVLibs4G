ideal
p386
model flat,cpp

codeseg

proc snd_silent_sampler
	xor eax,eax
	mov ebx,eax
	ret
endp

proc store_samples_8_1 uses ecx edx edi, @@p:dword,@@l:dword,@@s:dword
	mov edx,[@@s]
	mov edi,[@@p]
	mov ecx,[@@l]
	cld
@@1:    call edx
	add eax,ebx
	sar eax,1
	mov al,ah
	xor al,80h
	stosb
	loop @@1
	ret
endp

proc store_samples_8_2 uses ecx edx edi, @@p:dword,@@l:dword,@@s:dword
	mov edx,[@@s]
	mov edi,[@@p]
	mov ecx,[@@l]
	shr ecx,1
	cld
@@1:    call edx
	mov al,ah
	mov ah,bh
	xor ax,8080h
	stosw
	loop @@1
	ret
endp

proc store_samples_16_1 uses ecx edx edi, @@p:dword,@@l:dword,@@s:dword
	mov edx,[@@s]
	mov edi,[@@p]
	mov ecx,[@@l]
	shr ecx,1
	cld
@@1:    call edx
	add eax,ebx
	sar eax,1
	stosw
	loop @@1
	ret
endp

proc store_samples_16_2 uses ecx edx edi, @@p:dword,@@l:dword,@@s:dword
	mov edx,[@@s]
	mov edi,[@@p]
	mov ecx,[@@l]
	shr ecx,2
	cld
@@1:    call edx
	stosw
	mov ax,bx
	stosw
	loop @@1
	ret
endp

ifdef __DLL__
publicdll snd_silent_sampler
publicdll store_samples_8_1,store_samples_8_2
publicdll store_samples_16_1,store_samples_16_2
else
public    snd_silent_sampler
public    store_samples_8_1,store_samples_8_2
public    store_samples_16_1,store_samples_16_2
endif

end
