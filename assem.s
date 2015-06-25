.text
func0:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	movl $1, %ebx
	pushl %edi
	movl %ebp, %edi
	movl %edi, staticLinks(,%ebx,4)
	popl %edi
	popl %ebx
	pushl %ebx
	pushl %esi
	pushl %edi
	pushl %ecx
	pushl %edx
	pushl %edi
	movl 8(%ebp), %edi
	cmp $0, %edi
	popl %edi
	je booOPtrue0
	movl $0, %ebx
	jmp boolOPend0
booOPtrue0:
	movl $1, %ebx
boolOPend0:
	pushl %edi
	movl 8(%ebp), %edi
	cmp $1, %edi
	popl %edi
	je booOPtrue1
	movl $0, %ecx
	jmp boolOPend1
booOPtrue1:
	movl $1, %ecx
boolOPend1:
	cmp $1, %ebx
	je ORtrue2
	cmp $1, %ecx
	je ORtrue2
	movl $0, %edx
	jmp ORend2
ORtrue2:
	movl $1, %edx
ORend2:
	cmp $1, %edx
	jne else3
	movl $1, %eax
	jmp endIf3
else3:
	pushl %ebx
	movl $1, %ebx
	pushl %edi
	movl %ebp, %edi
	movl %edi, staticLinks(,%ebx,4)
	popl %edi
	popl %ebx
	movl 8(%ebp), %esi
	subl $1, %esi
	pushl %esi
	call func0
	addl $4, %esp
	movl %eax, %ebx
	movl %ebx, %ecx
	imul 8(%ebp), %ecx
	movl %ecx, %eax
endIf3:
endFunc0:
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	ret
.globl main
main:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	pushl %ecx
	pushl %edx
	movl $heapNext, (staticLinks)
	movl $1, %edx
	imul $4, %edx
	addl %edx, (heapNext)
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl $0, %ebx
	pushl %edi
	movl %ebp, %edi
	movl %edi, staticLinks(,%ebx,4)
	popl %edi
	popl %ebx
	pushl $5
	call func0
	addl $4, %esp
	movl %eax, %esi
	pushl %esi
	pushl $formNUM
	call printf
	addl $8, %esp
	popl %edx
	popl %ecx
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	movl $0, %eax
	ret
.data
formNUM: 
	.string "%d\n" 
formTRUE: 
	.string "TRUE\n" 
formFALSE: 
	.string "FALSE\n" 
formNULL: 
	.string "NULL\n" 
heap:
	.space 4194304
heapNext:
	.space 4
staticLinks:
	.space 4
