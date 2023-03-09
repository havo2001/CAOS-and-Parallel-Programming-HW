.intel_syntax noprefix
.global apply_operation
.text

apply_operation:
	push ebp
	mov ebp, esp
        push ebx 
 
	mov ebx, [ebp + 16]
	push ebx
	mov ebx, [ebp + 12]
	push ebx
	call [ebp + 8]
	add esp, 8
        
	pop ebx
	mov esp, ebp
	pop ebp
	ret
