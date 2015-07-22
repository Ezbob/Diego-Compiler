.text
f_BKnap.0:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	subl $8, %esp
	pushl %ecx
	pushl %edx
	pushl 20(%ebp)
	movl w, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl 8(%ebp), %ecx
	pushl -12(%ecx)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jle j_boolOPtrue.0
	movl $0, %ebx
	jmp j_boolOPend.0
j_boolOPtrue.0:
	movl $1, %ebx
j_boolOPend.0:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_endIf.1
	pushl $1
	movl y, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl 12(%ebp)
	movl 8(%ebp), %ecx
	pushl -16(%ecx)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.2
	movl $0, %ebx
	jmp j_boolOPend.2
j_boolOPtrue.2:
	movl $1, %ebx
j_boolOPend.2:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_endIf.3
	pushl 20(%ebp)
	movl w, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	pushl 16(%ebp)
	movl p, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	pushl 12(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	pushl 8(%ebp)
	calll f_BKnap.0
	addl $16, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -4(%ebp)
j_endIf.3:
	pushl 12(%ebp)
	movl 8(%ebp), %ecx
	pushl -16(%ecx)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	je j_boolOPtrue.5
	movl $0, %ebx
	jmp j_boolOPend.5
j_boolOPtrue.5:
	movl $1, %ebx
j_boolOPend.5:
	pushl %ebx
	popl %ebx
	cmpl $1, %ebx
	jne j_ANDfalse.4
	pushl 16(%ebp)
	movl p, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl 8(%ebp), %ecx
	pushl -20(%ecx)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jg j_boolOPtrue.6
	movl $0, %ebx
	jmp j_boolOPend.6
j_boolOPtrue.6:
	movl $1, %ebx
j_boolOPend.6:
	pushl %ebx
	popl %ebx
	cmpl $1, %ebx
	jne j_ANDfalse.4
	movl $1, %ebx
	jmp j_ANDend.4
j_ANDfalse.4:
	movl $0, %ebx
j_ANDend.4:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_endIf.7
	pushl 16(%ebp)
	movl p, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl 8(%ebp), %ecx
	popl %ebx
	movl %ebx, -20(%ecx)
	pushl 20(%ebp)
	movl w, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl 8(%ebp), %ecx
	popl %ebx
	movl %ebx, -24(%ecx)
	pushl $0
	popl %ebx
	movl %ebx, -8(%ebp)
j_whileStart.8:
	pushl -8(%ebp)
	pushl 12(%ebp)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jle j_boolOPtrue.9
	movl $0, %ebx
	jmp j_boolOPend.9
j_boolOPtrue.9:
	movl $1, %ebx
j_boolOPend.9:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_whileEnd.8
	movl y, %esi
	pushl -8(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl x, %esi
	pushl -8(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl -8(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -8(%ebp)
	jmp j_whileStart.8
j_whileEnd.8:
j_endIf.7:
j_endIf.1:
	pushl 12(%ebp)
	pushl 20(%ebp)
	pushl 16(%ebp)
	movl 8(%ebp), %eax
	pushl %eax
	calll f_Bound.1
	addl $16, %esp
	pushl %eax
	movl 8(%ebp), %ecx
	pushl -20(%ecx)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jge j_boolOPtrue.10
	movl $0, %ebx
	jmp j_boolOPend.10
j_boolOPtrue.10:
	movl $1, %ebx
j_boolOPend.10:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_endIf.11
	pushl $0
	movl y, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl 12(%ebp)
	movl 8(%ebp), %ecx
	pushl -16(%ecx)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.12
	movl $0, %ebx
	jmp j_boolOPend.12
j_boolOPtrue.12:
	movl $1, %ebx
j_boolOPend.12:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_endIf.13
	pushl 20(%ebp)
	pushl 16(%ebp)
	pushl 12(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	pushl 8(%ebp)
	calll f_BKnap.0
	addl $16, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -4(%ebp)
j_endIf.13:
	pushl 12(%ebp)
	movl 8(%ebp), %ecx
	pushl -16(%ecx)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	je j_boolOPtrue.15
	movl $0, %ebx
	jmp j_boolOPend.15
j_boolOPtrue.15:
	movl $1, %ebx
j_boolOPend.15:
	pushl %ebx
	popl %ebx
	cmpl $1, %ebx
	jne j_ANDfalse.14
	pushl 16(%ebp)
	movl 8(%ebp), %ecx
	pushl -20(%ecx)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jg j_boolOPtrue.16
	movl $0, %ebx
	jmp j_boolOPend.16
j_boolOPtrue.16:
	movl $1, %ebx
j_boolOPend.16:
	pushl %ebx
	popl %ebx
	cmpl $1, %ebx
	jne j_ANDfalse.14
	movl $1, %ebx
	jmp j_ANDend.14
j_ANDfalse.14:
	movl $0, %ebx
j_ANDend.14:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_endIf.17
	pushl 16(%ebp)
	movl 8(%ebp), %ecx
	popl %ebx
	movl %ebx, -20(%ecx)
	pushl 20(%ebp)
	movl 8(%ebp), %ecx
	popl %ebx
	movl %ebx, -24(%ecx)
	pushl $0
	popl %ebx
	movl %ebx, -8(%ebp)
j_whileStart.18:
	pushl -8(%ebp)
	pushl 12(%ebp)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jle j_boolOPtrue.19
	movl $0, %ebx
	jmp j_boolOPend.19
j_boolOPtrue.19:
	movl $1, %ebx
j_boolOPend.19:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_whileEnd.18
	movl y, %esi
	pushl -8(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl x, %esi
	pushl -8(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl -8(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -8(%ebp)
	jmp j_whileStart.18
j_whileEnd.18:
j_endIf.17:
j_endIf.11:
	pushl $0
	popl %eax
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	retl
f_Bound.1:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	subl $12, %esp
	pushl %ecx
	pushl %edx
	pushl 12(%ebp)
	popl %ebx
	movl %ebx, -4(%ebp)
	pushl 16(%ebp)
	popl %ebx
	movl %ebx, -8(%ebp)
	pushl 20(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -12(%ebp)
j_whileStart.20:
	pushl -12(%ebp)
	movl 8(%ebp), %ecx
	pushl -16(%ecx)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.21
	movl $0, %ebx
	jmp j_boolOPend.21
j_boolOPtrue.21:
	movl $1, %ebx
j_boolOPend.21:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_whileEnd.20
	pushl -8(%ebp)
	movl w, %esi
	pushl -12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -8(%ebp)
	pushl -8(%ebp)
	movl 8(%ebp), %ecx
	pushl -12(%ecx)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.22
	movl $0, %ebx
	jmp j_boolOPend.22
j_boolOPtrue.22:
	movl $1, %ebx
j_boolOPend.22:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_else.23
	pushl -4(%ebp)
	movl p, %esi
	pushl -12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -4(%ebp)
	jmp j_endIf.23
j_else.23:
	pushl -4(%ebp)
	pushl $1
	pushl -8(%ebp)
	movl 8(%ebp), %ecx
	pushl -12(%ecx)
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	movl w, %esi
	pushl -12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	xorl %edx, %edx
	idivl %ebx
	pushl %eax
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	movl p, %esi
	pushl -12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	imul %ebx, %eax
	pushl %eax
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %eax
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	retl
j_endIf.23:
	pushl -12(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -12(%ebp)
	jmp j_whileStart.20
j_whileEnd.20:
	pushl -4(%ebp)
	popl %eax
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	retl
f_NextRand.2:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	subl $4, %esp
	pushl %ecx
	pushl %edx
	pushl 12(%ebp)
	pushl 16(%ebp)
	popl %ebx
	popl %eax
	imul %ebx, %eax
	pushl %eax
	pushl 20(%ebp)
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -4(%ebp)
	pushl -4(%ebp)
	pushl -4(%ebp)
	pushl 24(%ebp)
	popl %ebx
	popl %eax
	xorl %edx, %edx
	idivl %ebx
	pushl %eax
	pushl 24(%ebp)
	popl %ebx
	popl %eax
	imul %ebx, %eax
	pushl %eax
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	popl %eax
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	retl
f_exchange.3:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	subl $8, %esp
	pushl %ecx
	pushl %edx
	movl p, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	movl %ebx, -4(%ebp)
	movl p, %esi
	pushl 16(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl p, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl -4(%ebp)
	movl p, %esi
	pushl 16(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	movl w, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	movl %ebx, -8(%ebp)
	movl w, %esi
	pushl 16(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl w, %esi
	pushl 12(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl -8(%ebp)
	movl w, %esi
	pushl 16(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl $1
	popl %eax
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	retl
f_quicksort.4:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	subl $8, %esp
	pushl %ecx
	pushl %edx
	pushl 12(%ebp)
	pushl 16(%ebp)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.24
	movl $0, %ebx
	jmp j_boolOPend.24
j_boolOPtrue.24:
	movl $1, %ebx
j_boolOPend.24:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_endIf.25
	pushl 16(%ebp)
	pushl 12(%ebp)
	movl 8(%ebp), %eax
	pushl %eax
	calll f_partition.5
	addl $12, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -4(%ebp)
	pushl -4(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	pushl 12(%ebp)
	pushl 8(%ebp)
	calll f_quicksort.4
	addl $12, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -8(%ebp)
	pushl 16(%ebp)
	pushl -4(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	pushl 8(%ebp)
	calll f_quicksort.4
	addl $12, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -8(%ebp)
j_endIf.25:
	pushl $1
	popl %eax
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	retl
f_partition.5:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	subl $20, %esp
	pushl %ecx
	pushl %edx
	movl p, %esi
	pushl 16(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	movl %ebx, -4(%ebp)
	movl w, %esi
	pushl 16(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	movl %ebx, -8(%ebp)
	pushl 12(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -12(%ebp)
	pushl 12(%ebp)
	popl %ebx
	movl %ebx, -16(%ebp)
j_whileStart.26:
	pushl -16(%ebp)
	pushl 16(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jle j_boolOPtrue.27
	movl $0, %ebx
	jmp j_boolOPend.27
j_boolOPtrue.27:
	movl $1, %ebx
j_boolOPend.27:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_whileEnd.26
	movl p, %esi
	pushl -16(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	pushl -8(%ebp)
	popl %ebx
	popl %eax
	imul %ebx, %eax
	pushl %eax
	pushl -4(%ebp)
	movl w, %esi
	pushl -16(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	popl %ebx
	popl %eax
	imul %ebx, %eax
	pushl %eax
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jge j_boolOPtrue.28
	movl $0, %ebx
	jmp j_boolOPend.28
j_boolOPtrue.28:
	movl $1, %ebx
j_boolOPend.28:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_endIf.29
	pushl -12(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -12(%ebp)
	pushl -16(%ebp)
	pushl -12(%ebp)
	movl 8(%ebp), %eax
	pushl %eax
	calll f_exchange.3
	addl $12, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -20(%ebp)
j_endIf.29:
	pushl -16(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -16(%ebp)
	jmp j_whileStart.26
j_whileEnd.26:
	pushl 16(%ebp)
	pushl -12(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl 8(%ebp), %eax
	pushl %eax
	calll f_exchange.3
	addl $12, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -20(%ebp)
	pushl -12(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %eax
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	retl
.globl main
main:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	subl $48, %esp
	pushl %ecx
	pushl %edx
	movl $heap., heap.NEXT
	movl heap.NEXT, %eax
	movl %eax, w
	xorl %ebx, %ebx
	pushl $1000
	popl %ecx
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	movl heap.NEXT, %eax
	movl %eax, p
	xorl %ebx, %ebx
	pushl $1000
	popl %ecx
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	movl heap.NEXT, %eax
	movl %eax, x
	xorl %ebx, %ebx
	pushl $1000
	popl %ecx
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	movl heap.NEXT, %eax
	movl %eax, y
	xorl %ebx, %ebx
	pushl $1000
	popl %ecx
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	pushl $1000
	popl %ebx
	movl %ebx, -16(%ebp)
	pushl $600
	popl %ebx
	movl %ebx, -12(%ebp)
	pushl $-1
	popl %ebx
	movl %ebx, -20(%ebp)
	pushl $0
	popl %ebx
	movl %ebx, -24(%ebp)
	pushl $0
	popl %ebx
	movl %ebx, -28(%ebp)
	pushl $1
	popl %ebx
	movl %ebx, -36(%ebp)
	pushl $40
	popl %ebx
	movl %ebx, -40(%ebp)
	pushl $3641
	popl %ebx
	movl %ebx, -44(%ebp)
	pushl $729
	popl %ebx
	movl %ebx, -48(%ebp)
	pushl $0
	popl %ebx
	movl %ebx, -8(%ebp)
j_whileStart.30:
	pushl -8(%ebp)
	pushl -16(%ebp)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.31
	movl $0, %ebx
	jmp j_boolOPend.31
j_boolOPtrue.31:
	movl $1, %ebx
j_boolOPend.31:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_whileEnd.30
	pushl -48(%ebp)
	pushl -44(%ebp)
	pushl -40(%ebp)
	pushl -36(%ebp)
	pushl %ebp
	calll f_NextRand.2
	addl $20, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -36(%ebp)
	pushl -36(%ebp)
	pushl $20
	popl %ebx
	popl %eax
	xorl %edx, %edx
	idivl %ebx
	pushl %eax
	pushl $10
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl w, %esi
	pushl -8(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl -48(%ebp)
	pushl -44(%ebp)
	pushl -40(%ebp)
	pushl -36(%ebp)
	pushl %ebp
	calll f_NextRand.2
	addl $20, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -36(%ebp)
	pushl -36(%ebp)
	pushl $90
	popl %ebx
	popl %eax
	xorl %edx, %edx
	idivl %ebx
	pushl %eax
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl p, %esi
	pushl -8(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl $0
	movl x, %esi
	pushl -8(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl $0
	movl y, %esi
	pushl -8(%ebp)
	popl %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl -8(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -8(%ebp)
	jmp j_whileStart.30
j_whileEnd.30:
	pushl -16(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	pushl $0
	pushl %ebp
	calll f_quicksort.4
	addl $12, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -32(%ebp)
	pushl $0
	pushl $0
	pushl $0
	pushl %ebp
	calll f_BKnap.0
	addl $16, %esp
	pushl %eax
	popl %ebx
	movl %ebx, -4(%ebp)
	pushl $0
	popl %ebx
	movl %ebx, -8(%ebp)
j_whileStart.32:
	pushl -8(%ebp)
	pushl -16(%ebp)
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.33
	movl $0, %ebx
	jmp j_boolOPend.33
j_boolOPtrue.33:
	movl $1, %ebx
j_boolOPend.33:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_whileEnd.32
	movl x, %esi
	pushl -8(%ebp)
	popl %edi
	incl %edi
	pushl (%esi,%edi,4)
	pushl $1
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	je j_boolOPtrue.34
	movl $0, %ebx
	jmp j_boolOPend.34
j_boolOPtrue.34:
	movl $1, %ebx
j_boolOPend.34:
	pushl %ebx
	popl %eax
	cmpl $1, %eax
	jne j_endIf.35
	pushl -28(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -28(%ebp)
j_endIf.35:
	pushl -8(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl %ebx, -8(%ebp)
	jmp j_whileStart.32
j_whileEnd.32:
	pushl -28(%ebp)
	popl %eax
	pushl %eax
	pushl $form.NUM
	calll printf
	addl $8, %esp
	pushl -20(%ebp)
	popl %eax
	pushl %eax
	pushl $form.NUM
	calll printf
	addl $8, %esp
	pushl -24(%ebp)
	popl %eax
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
w:
	.space 4
p:
	.space 4
x:
	.space 4
y:
	.space 4
