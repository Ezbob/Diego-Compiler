.text
.globl main
main:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	pushl %ecx
	pushl %edx
	movl $heap., heap.NEXT
	movl heap.NEXT, %eax
	movl %eax, a
	xorl %ebx, %ebx
	movl $2, %ecx
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	movl heap.NEXT, %eax
	movl %eax, b
	xorl %ebx, %ebx
	movl $2, %ecx
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	pushl $1
	movl a, %esi
	movl $0, %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl $1
	movl b, %esi
	movl $0, %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	movl $1, %eax
	pushl %eax
	pushl $form.NUM
	calll printf
	addl $8, %esp
	movl $0, %eax
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	retl
.data
form.NUM: 
	.string "%d\n" 
form.TRUE: 
	.string "TRUE\n" 
form.FALSE: 
	.string "FALSE\n" 
form.NULL: 
	.string "NULL\n" 
heap.:
	.space 4194304
heap.NEXT:
	.space 4
a:
	.space 4
b:
	.space 4
