	.file	"fib.c"
	.text
.globl fib
	.type	fib, @function
fib:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$0, -8(%ebp)
	movl	$1, -4(%ebp)
	cmpl	$1, 8(%ebp)
	jg	.L2
	movl	8(%ebp), %eax
	movl	%eax, -20(%ebp)
	jmp	.L4
.L2:
	movl	$0, -12(%ebp)
	jmp	.L5
.L6:
	movl	-4(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-8(%ebp), %eax
	movl	%eax, (%esp)
	call	addTwo
	movl	%eax, -16(%ebp)
	cmpl	$-1, -16(%ebp)
	jne	.L7
	movl	$-1, -20(%ebp)
	jmp	.L4
.L7:
	movl	-4(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, -4(%ebp)
	addl	$1, -12(%ebp)
.L5:
	movl	8(%ebp), %eax
	subl	$2, %eax
	cmpl	-12(%ebp), %eax
	jge	.L6
	movl	-16(%ebp), %eax
	movl	%eax, -20(%ebp)
.L4:
	movl	-20(%ebp), %eax
	leave
	ret
	.size	fib, .-fib
.globl addTwo
	.type	addTwo, @function
addTwo:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	movl	12(%ebp), %eax
	addl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	-4(%ebp), %eax
	cmpl	8(%ebp), %eax
	jge	.L12
	movl	$-1, -20(%ebp)
	jmp	.L14
.L12:
	movl	-4(%ebp), %eax
	cmpl	12(%ebp), %eax
	jge	.L15
	movl	$-1, -20(%ebp)
	jmp	.L14
.L15:
	movl	-4(%ebp), %eax
	movl	%eax, -20(%ebp)
.L14:
	movl	-20(%ebp), %eax
	leave
	ret
	.size	addTwo, .-addTwo
	.ident	"GCC: (GNU) 4.1.2 20080704 (Red Hat 4.1.2-48)"
	.section	.note.GNU-stack,"",@progbits
