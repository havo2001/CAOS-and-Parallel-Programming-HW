.intel_syntax noprefix
.global sum
.text

sum:
        push ebp
        mov ebp, esp
 
        push ebx
 
        mov eax, [ebp + 8]
        mov ebx, [ebp + 12]
	mov ecx, [ebp + 16]
        add ebx, ecx
	add eax, ebx

        pop ebx 
        mov esp, ebp
        pop ebp
        ret

