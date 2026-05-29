#include <asm.h>

ENTRY(write)
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx
    pushl %esi
    pushl %edi
    movl $4, %eax
    movl 0x8(%ebp), %edx 
    movl 0xc(%ebp), %ecx
    movl 0x10(%ebp), %ebx
    pushl %ecx 
    pushl %edx 
    pushl $write_ret
    pushl %ebp
    movl %esp, %ebp 
    sysenter
write_ret:
    pop %ebp    
    pop %edx    
    pop %edx   
    pop %ecx    
    pop %edi
    pop %esi
    pop %ebx
    test %eax, %eax
    jns write_end
    negl %eax
    movl %eax, errno
    movl $-1, %eax
write_end:
    pop %ebp
    ret

ENTRY(gettime)
	pushl %ebp
	movl %esp,%ebp
	pushl %edx
	pushl %ecx
	movl $10, %eax
	pushl $gettime_ret
	pushl %ebp
	movl %esp,%ebp
	sysenter
gettime_ret:
	pop %ebp
	addl $4, %esp
	pop %edx
	pop %ecx
	cmpl $0, %eax
	jge gettime_end
	negl %eax 
	movl %eax, errno
	movl -1, %eax
gettime_end:
	pop %ebp
	ret

