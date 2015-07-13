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
	movl 12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl 8(%ebp), %ecx
	movl -12(%ecx), %ebx
	popl %eax
	cmpl %ebx, %eax
	jle j_boolOPtrue.0
	movl $0, %ebx
	jmp j_boolOPend.0
j_boolOPtrue.0:
	movl $1, %ebx
j_boolOPend.0:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_endIf.1
	pushl $1
	movl y, %esi
	movl 12(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl 12(%ebp)
	movl 8(%ebp), %ecx
	movl -16(%ecx), %eax
	movl $1, %ebx
	subl %ebx, %eax
	movl %eax, %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.2
	movl $0, %ebx
	jmp j_boolOPend.2
j_boolOPtrue.2:
	movl $1, %ebx
j_boolOPend.2:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_endIf.3
	pushl 20(%ebp)
	movl w, %esi
	movl 12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	pushl 16(%ebp)
	movl p, %esi
	movl 12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl 12(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	pushl %eax
	pushl 8(%ebp)
	calll f_BKnap.0
	addl $16, %esp
	movl %eax, -4(%ebp)
j_endIf.3:
	pushl 12(%ebp)
	movl 8(%ebp), %ecx
	movl -16(%ecx), %eax
	movl $1, %ebx
	subl %ebx, %eax
	movl %eax, %ebx
	popl %eax
	cmpl %ebx, %eax
	je j_boolOPtrue.5
	movl $0, %ebx
	jmp j_boolOPend.5
j_boolOPtrue.5:
	movl $1, %ebx
j_boolOPend.5:
	cmpl $1, %ebx
	jne j_ANDfalse.4
	pushl 16(%ebp)
	movl p, %esi
	movl 12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	movl 8(%ebp), %ecx
	movl -20(%ecx), %ebx
	popl %eax
	cmpl %ebx, %eax
	jg j_boolOPtrue.6
	movl $0, %ebx
	jmp j_boolOPend.6
j_boolOPtrue.6:
	movl $1, %ebx
j_boolOPend.6:
	cmpl $1, %ebx
	jne j_ANDfalse.4
	movl $1, %ebx
	jmp j_ANDend.4
j_ANDfalse.4:
	movl $0, %ebx
j_ANDend.4:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_endIf.7
	pushl 16(%ebp)
	movl p, %esi
	movl 12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	addl %ebx, %eax
	movl %eax, %ebx
	movl 8(%ebp), %ecx
	movl %ebx, -20(%ecx)
	pushl 20(%ebp)
	movl w, %esi
	movl 12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	addl %ebx, %eax
	movl %eax, %ebx
	movl 8(%ebp), %ecx
	movl %ebx, -24(%ecx)
	movl $0, -8(%ebp)
j_whileStart.8:
	movl -8(%ebp), %eax
	movl 12(%ebp), %ebx
	cmpl %ebx, %eax
	jle j_boolOPtrue.9
	movl $0, %ebx
	jmp j_boolOPend.9
j_boolOPtrue.9:
	movl $1, %ebx
j_boolOPend.9:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_whileEnd.8
	movl y, %esi
	movl -8(%ebp), %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl x, %esi
	movl -8(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	movl -8(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -8(%ebp)
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
	movl -20(%ecx), %ebx
	popl %eax
	cmpl %ebx, %eax
	jge j_boolOPtrue.10
	movl $0, %ebx
	jmp j_boolOPend.10
j_boolOPtrue.10:
	movl $1, %ebx
j_boolOPend.10:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_endIf.11
	pushl $0
	movl y, %esi
	movl 12(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl 12(%ebp)
	movl 8(%ebp), %ecx
	movl -16(%ecx), %eax
	movl $1, %ebx
	subl %ebx, %eax
	movl %eax, %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.12
	movl $0, %ebx
	jmp j_boolOPend.12
j_boolOPtrue.12:
	movl $1, %ebx
j_boolOPend.12:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_endIf.13
	pushl 20(%ebp)
	pushl 16(%ebp)
	movl 12(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	pushl %eax
	pushl 8(%ebp)
	calll f_BKnap.0
	addl $16, %esp
	movl %eax, -4(%ebp)
j_endIf.13:
	pushl 12(%ebp)
	movl 8(%ebp), %ecx
	movl -16(%ecx), %eax
	movl $1, %ebx
	subl %ebx, %eax
	movl %eax, %ebx
	popl %eax
	cmpl %ebx, %eax
	je j_boolOPtrue.15
	movl $0, %ebx
	jmp j_boolOPend.15
j_boolOPtrue.15:
	movl $1, %ebx
j_boolOPend.15:
	cmpl $1, %ebx
	jne j_ANDfalse.14
	pushl 16(%ebp)
	movl 8(%ebp), %ecx
	movl -20(%ecx), %ebx
	popl %eax
	cmpl %ebx, %eax
	jg j_boolOPtrue.16
	movl $0, %ebx
	jmp j_boolOPend.16
j_boolOPtrue.16:
	movl $1, %ebx
j_boolOPend.16:
	cmpl $1, %ebx
	jne j_ANDfalse.14
	movl $1, %ebx
	jmp j_ANDend.14
j_ANDfalse.14:
	movl $0, %ebx
j_ANDend.14:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_endIf.17
	movl 16(%ebp), %ebx
	movl 8(%ebp), %ecx
	movl %ebx, -20(%ecx)
	movl 20(%ebp), %ebx
	movl 8(%ebp), %ecx
	movl %ebx, -24(%ecx)
	movl $0, -8(%ebp)
j_whileStart.18:
	movl -8(%ebp), %eax
	movl 12(%ebp), %ebx
	cmpl %ebx, %eax
	jle j_boolOPtrue.19
	movl $0, %ebx
	jmp j_boolOPend.19
j_boolOPtrue.19:
	movl $1, %ebx
j_boolOPend.19:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_whileEnd.18
	movl y, %esi
	movl -8(%ebp), %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl x, %esi
	movl -8(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	movl -8(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -8(%ebp)
	jmp j_whileStart.18
j_whileEnd.18:
j_endIf.17:
j_endIf.11:
	movl $0, %eax
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
	movl 12(%ebp), %ebx
	movl %ebx, -4(%ebp)
	movl 16(%ebp), %ebx
	movl %ebx, -8(%ebp)
	movl 20(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -12(%ebp)
j_whileStart.20:
	pushl -12(%ebp)
	movl 8(%ebp), %ecx
	movl -16(%ecx), %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.21
	movl $0, %ebx
	jmp j_boolOPend.21
j_boolOPtrue.21:
	movl $1, %ebx
j_boolOPend.21:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_whileEnd.20
	pushl -8(%ebp)
	movl w, %esi
	movl -12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	addl %ebx, %eax
	movl %eax, -8(%ebp)
	pushl -8(%ebp)
	movl 8(%ebp), %ecx
	movl -12(%ecx), %ebx
	popl %eax
	cmpl %ebx, %eax
	jl j_boolOPtrue.22
	movl $0, %ebx
	jmp j_boolOPend.22
j_boolOPtrue.22:
	movl $1, %ebx
j_boolOPend.22:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_else.23
	pushl -4(%ebp)
	movl p, %esi
	movl -12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	addl %ebx, %eax
	movl %eax, -4(%ebp)
	jmp j_endIf.23
j_else.23:
	pushl -4(%ebp)
	pushl $1
	pushl -8(%ebp)
	movl 8(%ebp), %ecx
	movl -12(%ecx), %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	movl w, %esi
	movl -12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	xorl %edx, %edx
	idivl %ebx
	movl %eax, %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	movl p, %esi
	movl -12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	imul %ebx, %eax
	movl %eax, %ebx
	popl %eax
	addl %ebx, %eax
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	retl
j_endIf.23:
	movl -12(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -12(%ebp)
	jmp j_whileStart.20
j_whileEnd.20:
	movl -4(%ebp), %eax
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
	movl 12(%ebp), %eax
	movl 16(%ebp), %ebx
	imul %ebx, %eax
	movl 20(%ebp), %ebx
	addl %ebx, %eax
	movl %eax, -4(%ebp)
	pushl -4(%ebp)
	movl -4(%ebp), %eax
	movl 24(%ebp), %ebx
	xorl %edx, %edx
	idivl %ebx
	movl 24(%ebp), %ebx
	imul %ebx, %eax
	movl %eax, %ebx
	popl %eax
	subl %ebx, %eax
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
	movl 12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	movl %ebx, -4(%ebp)
	movl p, %esi
	movl 16(%ebp), %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl p, %esi
	movl 12(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl -4(%ebp)
	movl p, %esi
	movl 16(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	movl w, %esi
	movl 12(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	movl %ebx, -8(%ebp)
	movl w, %esi
	movl 16(%ebp), %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl w, %esi
	movl 12(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl -8(%ebp)
	movl w, %esi
	movl 16(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	movl $1, %eax
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
	movl 12(%ebp), %eax
	movl 16(%ebp), %ebx
	cmpl %ebx, %eax
	jl j_boolOPtrue.24
	movl $0, %ebx
	jmp j_boolOPend.24
j_boolOPtrue.24:
	movl $1, %ebx
j_boolOPend.24:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_endIf.25
	pushl 16(%ebp)
	pushl 12(%ebp)
	movl 8(%ebp), %eax
	pushl %eax
	calll f_partition.5
	addl $12, %esp
	movl %eax, -4(%ebp)
	movl -4(%ebp), %eax
	movl $1, %ebx
	subl %ebx, %eax
	pushl %eax
	pushl 12(%ebp)
	pushl 8(%ebp)
	calll f_quicksort.4
	addl $12, %esp
	movl %eax, -8(%ebp)
	pushl 16(%ebp)
	movl -4(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	pushl %eax
	pushl 8(%ebp)
	calll f_quicksort.4
	addl $12, %esp
	movl %eax, -8(%ebp)
j_endIf.25:
	movl $1, %eax
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
	movl 16(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	movl %ebx, -4(%ebp)
	movl w, %esi
	movl 16(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	movl %ebx, -8(%ebp)
	movl 12(%ebp), %eax
	movl $1, %ebx
	subl %ebx, %eax
	movl %eax, -12(%ebp)
	movl 12(%ebp), %ebx
	movl %ebx, -16(%ebp)
j_whileStart.26:
	pushl -16(%ebp)
	movl 16(%ebp), %eax
	movl $1, %ebx
	subl %ebx, %eax
	movl %eax, %ebx
	popl %eax
	cmpl %ebx, %eax
	jle j_boolOPtrue.27
	movl $0, %ebx
	jmp j_boolOPend.27
j_boolOPtrue.27:
	movl $1, %ebx
j_boolOPend.27:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_whileEnd.26
	movl p, %esi
	movl -16(%ebp), %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl -8(%ebp), %ebx
	popl %eax
	imul %ebx, %eax
	pushl %eax
	pushl -4(%ebp)
	movl w, %esi
	movl -16(%ebp), %edi
	incl %edi
	movl (%esi,%edi,4), %ebx
	popl %eax
	imul %ebx, %eax
	movl %eax, %ebx
	popl %eax
	cmpl %ebx, %eax
	jge j_boolOPtrue.28
	movl $0, %ebx
	jmp j_boolOPend.28
j_boolOPtrue.28:
	movl $1, %ebx
j_boolOPend.28:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_endIf.29
	movl -12(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -12(%ebp)
	pushl -16(%ebp)
	pushl -12(%ebp)
	movl 8(%ebp), %eax
	pushl %eax
	calll f_exchange.3
	addl $12, %esp
	movl %eax, -20(%ebp)
j_endIf.29:
	movl -16(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -16(%ebp)
	jmp j_whileStart.26
j_whileEnd.26:
	pushl 16(%ebp)
	movl -12(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	pushl %eax
	movl 8(%ebp), %eax
	pushl %eax
	calll f_exchange.3
	addl $12, %esp
	movl %eax, -20(%ebp)
	movl -12(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
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
	movl $1000, %ecx
	cmpl $0, %ecx
	jge j_notNegSize.30
	pushl $125
	pushl $error.NEGSIZE
	calll printf
	addl $8, %esp
	movl $4, %ebx
	movl $1, %eax
	int $0x80
j_notNegSize.30:
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	movl heap.NEXT, %eax
	movl %eax, p
	xorl %ebx, %ebx
	movl $1000, %ecx
	cmpl $0, %ecx
	jge j_notNegSize.31
	pushl $126
	pushl $error.NEGSIZE
	calll printf
	addl $8, %esp
	movl $4, %ebx
	movl $1, %eax
	int $0x80
j_notNegSize.31:
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	movl heap.NEXT, %eax
	movl %eax, x
	xorl %ebx, %ebx
	movl $1000, %ecx
	cmpl $0, %ecx
	jge j_notNegSize.32
	pushl $127
	pushl $error.NEGSIZE
	calll printf
	addl $8, %esp
	movl $4, %ebx
	movl $1, %eax
	int $0x80
j_notNegSize.32:
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	movl heap.NEXT, %eax
	movl %eax, y
	xorl %ebx, %ebx
	movl $1000, %ecx
	cmpl $0, %ecx
	jge j_notNegSize.33
	pushl $128
	pushl $error.NEGSIZE
	calll printf
	addl $8, %esp
	movl $4, %ebx
	movl $1, %eax
	int $0x80
j_notNegSize.33:
	movl %ecx, (%eax,%ebx,4)
	incl %ecx
	imul $4, %ecx
	addl %ecx, heap.NEXT
	movl $1000, -16(%ebp)
	movl $600, -12(%ebp)
	movl $-1, -20(%ebp)
	movl $0, -24(%ebp)
	movl $0, -28(%ebp)
	movl $1, -36(%ebp)
	movl $40, -40(%ebp)
	movl $3641, -44(%ebp)
	movl $729, -48(%ebp)
	movl $0, -8(%ebp)
j_whileStart.34:
	movl -8(%ebp), %eax
	movl -16(%ebp), %ebx
	cmpl %ebx, %eax
	jl j_boolOPtrue.35
	movl $0, %ebx
	jmp j_boolOPend.35
j_boolOPtrue.35:
	movl $1, %ebx
j_boolOPend.35:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_whileEnd.34
	pushl -48(%ebp)
	pushl -44(%ebp)
	pushl -40(%ebp)
	pushl -36(%ebp)
	pushl %ebp
	calll f_NextRand.2
	addl $20, %esp
	movl %eax, -36(%ebp)
	movl -36(%ebp), %eax
	movl $20, %ebx
	xorl %edx, %edx
	idivl %ebx
	movl $10, %ebx
	addl %ebx, %eax
	pushl %eax
	movl w, %esi
	movl -8(%ebp), %edi
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
	movl %eax, -36(%ebp)
	movl -36(%ebp), %eax
	movl $90, %ebx
	xorl %edx, %edx
	idivl %ebx
	movl $1, %ebx
	addl %ebx, %eax
	pushl %eax
	movl p, %esi
	movl -8(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl $0
	movl x, %esi
	movl -8(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	pushl $0
	movl y, %esi
	movl -8(%ebp), %edi
	incl %edi
	popl %ebx
	movl %ebx, (%esi,%edi,4)
	movl -8(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -8(%ebp)
	jmp j_whileStart.34
j_whileEnd.34:
	movl -16(%ebp), %eax
	movl $1, %ebx
	subl %ebx, %eax
	pushl %eax
	pushl $0
	pushl %ebp
	calll f_quicksort.4
	addl $12, %esp
	movl %eax, -32(%ebp)
	pushl $0
	pushl $0
	pushl $0
	pushl %ebp
	calll f_BKnap.0
	addl $16, %esp
	movl %eax, -4(%ebp)
	movl $0, -8(%ebp)
j_whileStart.36:
	movl -8(%ebp), %eax
	movl -16(%ebp), %ebx
	cmpl %ebx, %eax
	jl j_boolOPtrue.37
	movl $0, %ebx
	jmp j_boolOPend.37
j_boolOPtrue.37:
	movl $1, %ebx
j_boolOPend.37:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_whileEnd.36
	movl x, %esi
	movl -8(%ebp), %edi
	incl %edi
	pushl (%esi,%edi,4)
	movl $1, %ebx
	popl %eax
	cmpl %ebx, %eax
	je j_boolOPtrue.38
	movl $0, %ebx
	jmp j_boolOPend.38
j_boolOPtrue.38:
	movl $1, %ebx
j_boolOPend.38:
	movl %ebx, %eax
	cmpl $1, %eax
	jne j_endIf.39
	movl -28(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -28(%ebp)
j_endIf.39:
	movl -8(%ebp), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -8(%ebp)
	jmp j_whileStart.36
j_whileEnd.36:
	movl -28(%ebp), %eax
	pushl %eax
	pushl $form.NUM
	calll printf
	addl $8, %esp
	movl -20(%ebp), %eax
	pushl %eax
	pushl $form.NUM
	calll printf
	addl $8, %esp
	movl -24(%ebp), %eax
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
w:
	.space 4
p:
	.space 4
x:
	.space 4
y:
	.space 4
