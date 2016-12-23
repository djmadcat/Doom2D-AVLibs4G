ideal
p386
model flat,cpp

dataseg

extrn sv_width:dword,sv_height:dword

mouse_sx dd 1
mouse_sy dd 1

mouse_buttons dw 0
public mouse_buttons,mouse_sx,mouse_sy

udataseg

mx dd ?
my dd ?

codeseg

proc I_init uses ebx es
public I_init
  mov [mx],0
  mov [my],0
  mov ax,3533h
  int 21h
  mov ax,es
  movzx eax,ax
  or eax,ebx
  jz @@no
  xor ax,ax
  int 33h
  or ax,ax
  jz @@no
  mov [mouse_buttons],bx
  ret
@@no:
  mov [mouse_buttons],0
  ret
endp

proc I_close
public I_close
  cmp [mouse_buttons],0
  je @@x
@@x:
  ret
endp

proc I_mget uses ebx ecx esi edi, @@x:dword,@@y:dword
public I_mget
  cmp [mouse_buttons],0
  je @@err
  mov ax,0Bh
  int 33h
  movsx esi,cx
  sal esi,8
  movsx edi,dx
  sal edi,8

  mov eax,esi
  mov edx,eax
  sar edx,31
  idiv [mouse_sx]
  add eax,[mx]
  or eax,eax
  jns @@a
  mov eax,0
@@a:
  mov ebx,[sv_width]
  shl ebx,8
  cmp eax,ebx
  jb @@b
  mov eax,ebx
  dec eax
@@b:
  mov [mx],eax
  shr eax,8
  mov ecx,eax

  mov eax,edi
  mov edx,eax
  sar edx,31
  idiv [mouse_sy]
  add eax,[my]
  or eax,eax
  jns @@c
  mov eax,0
@@c:
  mov ebx,[sv_height]
  shl ebx,8
  cmp eax,ebx
  jb @@d
  mov eax,ebx
  dec eax
@@d:
  mov [my],eax
  shr eax,8
  mov edx,eax

  mov ebx,[@@x]
  mov [ebx],ecx
  mov ebx,[@@y]
  mov [ebx],edx
  mov ax,3
  int 33h
  movzx eax,bx
  ret
@@err:
  mov eax,[sv_width]
  shr eax,1
  mov ebx,[@@x]
  mov [dword ebx],eax
  mov eax,[sv_height]
  shr eax,1
  mov ebx,[@@y]
  mov [dword ebx],eax
  xor eax,eax
  ret
endp

end
