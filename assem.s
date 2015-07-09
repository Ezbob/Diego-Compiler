.text
.globl main
main:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	movl $heap., heap.NEXT
	subl $12, %esp
	pushl %ecx
	pushl %edx
	movl $42, %ebx
	movl %ebx, -4(%ebp)
	movl $40, %ebx
	movl %ebx, -8(%ebp)
	movl $100, %ebx
	movl %ebx, -12(%ebp)
whileStart1:
	movl -12(%ebp), %ecx
	movl -4(%ebp), %ebx
	cmpl %ebx, %ecx
	jg booOPtrue1
	pushl $0
	jmp boolOPend1
booOPtrue1:
	pushl $1
boolOPend1:
	popl %eax
	cmpl $1, %eax
	jne whileEnd1
	movl -4(%ebp), %ecx
	movl -8(%ebp), %ebx
	cmpl %ebx, %ecx
	je booOPtrue2
	pushl $0
	jmp boolOPend2
booOPtrue2:
	pushl $1
boolOPend2:
	popl %eax
	cmpl $1, %eax
	jne endIf3
	jmp whileEnd1
endIf3:
	movl -4(%ebp), %ecx
	movl $1, %ebx
	subl %ebx, %ecx
	movl %ecx, -4(%ebp)
	jmp whileStart1
whileEnd1:
	pushl %ecx
	pushl %edx
	movl -4(%ebp), %eax
	pushl %eax
	pushl $form.NUM
	calll printf
	addl $8, %esp
	popl %edx
	popl %ecx
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
error.DIVZERO: 
	.string "Error at line %i: Division by zero\n" 
error.OUTMEM: 
	.string "Error at line %i: Cannot allocate; out of memory\n" 
error.NULL: 
	.string "Error at line %i: Null pointer exception\n" 
error.OUTBOUNDS: 
	.string "Error at line %i: Index out of bounds\n" 
error.NEGSIZE: 
	.string "Error at line %i: Cannot allocate arrays of negative size\n" 
heap.:
	.space 4194304
heap.NEXT:
	.space 4
