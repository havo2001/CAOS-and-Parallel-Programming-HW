	.file	"pong.c"
	.text
	.globl	sigterm_received
	.bss
	.align 4
	.type	sigterm_received, @object
	.size	sigterm_received, 4
sigterm_received:
	.zero	4
	.globl	expected_value
	.data
	.align 4
	.type	expected_value, @object
	.size	expected_value, 4
expected_value:
	.long	1
	.globl	seed
	.bss
	.align 4
	.type	seed, @object
	.size	seed, 4
seed:
	.zero	4
	.text
	.globl	handle_sigterm_sigint
	.type	handle_sigterm_sigint, @function
handle_sigterm_sigint:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	$1, sigterm_received(%rip)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	handle_sigterm_sigint, .-handle_sigterm_sigint
	.section	.rodata
.LC0:
	.string	"pong %d\n"
	.align 8
.LC1:
	.string	"wrong value from ping: %d, expected %d\n"
	.text
	.globl	handle_sigrtmin
	.type	handle_sigrtmin, @function
handle_sigrtmin:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-48(%rbp), %rax
	movl	16(%rax), %eax
	movl	%eax, -28(%rbp)
	movq	-48(%rbp), %rax
	movl	24(%rax), %eax
	movl	%eax, -24(%rbp)
	movl	expected_value(%rip), %edx
	movl	-24(%rbp), %eax
	cmpl	%eax, %edx
	jne	.L3
	movl	-24(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	seed(%rip), %eax
	movl	%eax, %edx
	movl	-24(%rbp), %eax
	addl	%eax, %edx
	movl	seed(%rip), %eax
	movl	%eax, %ecx
	movl	-24(%rbp), %eax
	addl	%ecx, %eax
	imull	%edx, %eax
	shrl	%eax
	movl	%eax, %edx
	movl	%edx, %eax
	imulq	$274877907, %rax, %rax
	shrq	$32, %rax
	shrl	$6, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	imull	$1000, %eax, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	movl	%eax, -20(%rbp)
	movl	$1, %edi
	call	sleep@PLT
	movl	$0, %ebx
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	movabsq	$-4294967296, %rax
	andq	%rbx, %rax
	orq	%rdx, %rax
	movq	%rax, %rbx
	call	__libc_current_sigrtmin@PLT
	movl	%eax, %ecx
	movl	-28(%rbp), %eax
	movq	%rbx, %rdx
	movl	%ecx, %esi
	movl	%eax, %edi
	call	sigqueue@PLT
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movl	%eax, expected_value(%rip)
	jmp	.L5
.L3:
	movl	expected_value(%rip), %edx
	movl	-24(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
.L5:
	nop
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	handle_sigrtmin, .-handle_sigrtmin
	.section	.rodata
.LC2:
	.string	"pid: %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$624, %rsp
	movl	%edi, -612(%rbp)
	movq	%rsi, -624(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	call	getpid@PLT
	movl	%eax, %esi
	leaq	.LC2(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	cmpl	$1, -612(%rbp)
	jle	.L7
	movq	-624(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, seed(%rip)
	jmp	.L8
.L7:
	movl	$3, seed(%rip)
.L8:
	leaq	-480(%rbp), %rdx
	movl	$0, %eax
	movl	$19, %ecx
	movq	%rdx, %rdi
	rep stosq
	leaq	handle_sigrtmin(%rip), %rax
	movq	%rax, -480(%rbp)
	movl	$268435460, -344(%rbp)
	call	__libc_current_sigrtmin@PLT
	movl	%eax, %ecx
	leaq	-480(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rsi
	movl	%ecx, %edi
	call	sigaction@PLT
	leaq	-320(%rbp), %rdx
	movl	$0, %eax
	movl	$19, %ecx
	movq	%rdx, %rdi
	rep stosq
	leaq	handle_sigterm_sigint(%rip), %rax
	movq	%rax, -320(%rbp)
	movl	$268435456, -184(%rbp)
	leaq	-320(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rsi
	movl	$15, %edi
	call	sigaction@PLT
	leaq	-160(%rbp), %rdx
	movl	$0, %eax
	movl	$19, %ecx
	movq	%rdx, %rdi
	rep stosq
	leaq	handle_sigterm_sigint(%rip), %rax
	movq	%rax, -160(%rbp)
	movl	$268435456, -24(%rbp)
	leaq	-160(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rsi
	movl	$2, %edi
	call	sigaction@PLT
	leaq	-608(%rbp), %rax
	movq	%rax, %rdi
	call	sigfillset@PLT
	call	__libc_current_sigrtmin@PLT
	movl	%eax, %edx
	leaq	-608(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	sigdelset@PLT
	leaq	-608(%rbp), %rax
	movl	$15, %esi
	movq	%rax, %rdi
	call	sigdelset@PLT
	leaq	-608(%rbp), %rax
	movl	$2, %esi
	movq	%rax, %rdi
	call	sigdelset@PLT
	leaq	-608(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rsi
	movl	$2, %edi
	call	sigprocmask@PLT
	jmp	.L9
.L10:
	call	pause@PLT
	movl	sigterm_received(%rip), %eax
	testl	%eax, %eax
	je	.L9
	movl	$0, %edi
	call	exit@PLT
.L9:
	movl	sigterm_received(%rip), %eax
	testl	%eax, %eax
	je	.L10
	movl	$0, %eax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L12
	call	__stack_chk_fail@PLT
.L12:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
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

