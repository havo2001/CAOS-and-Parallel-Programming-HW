.file	"imposter.c"
	.text
	.globl	handle_sigterm
	.type	handle_sigterm, @function
handle_sigterm:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	$0, %edi
	call	exit@PLT
	.cfi_endproc
.LFE6:
	.size	handle_sigterm, .-handle_sigterm
	.section	.rodata
.LC0:
	.string	"Received unexpected signal"
	.text
	.globl	handle_sigint
	.type	handle_sigint, @function
handle_sigint:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	leaq	.LC0(%rip), %rdi
	call	puts@PLT
	movq	stdout(%rip), %rax
	movq	%rax, %rdi
	call	fflush@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	handle_sigint, .-handle_sigint
	.globl	handle_sigrtmin
	.type	handle_sigrtmin, @function
handle_sigrtmin:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	leaq	.LC0(%rip), %rdi
	call	puts@PLT
	movq	stdout(%rip), %rax
	movq	%rax, %rdi
	call	fflush@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	handle_sigrtmin, .-handle_sigrtmin
	.section	.rodata
.LC1:
	.string	"pid: %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$648, %rsp
	.cfi_offset 3, -24
	movl	%edi, -644(%rbp)
	movq	%rsi, -656(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	call	getpid@PLT
	movl	%eax, %esi
	leaq	.LC1(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-656(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, -628(%rbp)
	leaq	-496(%rbp), %rdx
	movl	$0, %eax
	movl	$19, %ecx
	movq	%rdx, %rdi
	rep stosq
	leaq	handle_sigrtmin(%rip), %rax
	movq	%rax, -496(%rbp)
	movl	$268435460, -360(%rbp)
	call	__libc_current_sigrtmin@PLT
	movl	%eax, %ecx
	leaq	-496(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rsi
	movl	%ecx, %edi
	call	sigaction@PLT
	leaq	-336(%rbp), %rdx
	movl	$0, %eax
	movl	$19, %ecx
	movq	%rdx, %rdi
	rep stosq
	leaq	handle_sigterm(%rip), %rax
	movq	%rax, -336(%rbp)
	movl	$268435456, -200(%rbp)
	leaq	-336(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rsi
	movl	$15, %edi
	call	sigaction@PLT
	leaq	-176(%rbp), %rdx
	movl	$0, %eax
	movl	$19, %ecx
	movq	%rdx, %rdi
	rep stosq
	leaq	handle_sigint(%rip), %rax
	movq	%rax, -176(%rbp)
	movl	$268435456, -40(%rbp)
	leaq	-176(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rsi
	movl	$2, %edi
	call	sigaction@PLT
	leaq	-624(%rbp), %rax
	movq	%rax, %rdi
	call	sigfillset@PLT
	call	__libc_current_sigrtmin@PLT
	movl	%eax, %edx
	leaq	-624(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	sigdelset@PLT
	leaq	-624(%rbp), %rax
	movl	$15, %esi
	movq	%rax, %rdi
	call	sigdelset@PLT
	leaq	-624(%rbp), %rax
	movl	$2, %esi
	movq	%rax, %rdi
	call	sigdelset@PLT
	leaq	-624(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rsi
	movl	$2, %edi
	call	sigprocmask@PLT
	movl	$0, %ebx
	movabsq	$-4294967296, %rax
	andq	%rbx, %rax
	orq	$1329, %rax
	movq	%rax, %rbx
	call	__libc_current_sigrtmin@PLT
	movl	%eax, %ecx
	movl	-628(%rbp), %eax
	movq	%rbx, %rdx
	movl	%ecx, %esi
	movl	%eax, %edi
	call	sigqueue@PLT
.L5:
	call	pause@PLT
	jmp	.L5
	.cfi_endproc
.LFE9:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
