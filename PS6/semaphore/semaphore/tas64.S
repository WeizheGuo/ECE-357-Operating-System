	.text
.globl _tas
_tas:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$1, %rax
#APP
	lock;xchgb	%al,(%rdi)
#NO_APP
	movsbq	%al,%rax
	pop	%rbp
	ret
Lfe1:
