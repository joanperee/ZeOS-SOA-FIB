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

ENTRY(getpid)
	pushl %ebp
	movl %esp,%ebp
	pushl %edx
	pushl %ecx
	movl $20, %eax
	pushl $getpid_ret
	pushl %ebp
	movl %esp,%ebp
	sysenter
getpid_ret:
	pop %ebp
	addl $4, %esp
	pop %edx
	pop %ecx
	cmpl $0, %eax
	jge getpid_end
	negl %eax 
	movl %eax, errno
	movl -1, %eax
getpid_end:
	pop %ebp
	ret


ENTRY(fork)
	pushl %ebp
	movl %esp,%ebp
	movl $0x2, %eax
	pushl %edx
	pushl %ecx
	push $fork_ret
	pushl %ebp
	movl %esp,%ebp
	sysenter
fork_ret:
	popl %ebp
	addl $4, %esp
	popl %ecx
	popl %edx
	cmpl $0, %eax
	jge fork_no_error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fork_no_error:
	popl %ebp
	ret

 ENTRY(read)
    pushl %ebp
    movl %esp, %ebp
    pushl %ecx
    pushl %edx   
    movl $21, %eax
    movl 8(%ebp), %edx
    movl 12(%ebp), %ecx 
	pushl $read_ret
    pushl %ebp
    movl %esp, %ebp
    sysenter
read_ret:
    popl %ebp
    addl $4, %esp
    popl %edx               
    popl %ecx               
    test %eax, %eax
    jns read_end
    negl %eax
    movl %eax, errno
    movl $-1, %eax
read_end:
    popl %ebp
    ret 

ENTRY(gotoxy)
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edx
	movl $22, %eax
	movl 8(%ebp), %edx 
	movl 12(%ebp), %ecx 
	pushl $gotoxy_ret 
	pushl %ebp 
	movl %esp, %ebp
	sysenter
gotoxy_ret:
	popl %ebp
	addl $4, %esp 
	popl %edx 
	popl %ecx 
	test %eax, %eax
	jns gotoxy_end 
	negl %eax 
	movl %eax, errno
	movl $-1, %eax 
gotoxy_end:
	popl %ebp
	ret

ENTRY(set_color)
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edx 
	movl $23, %eax
	movl 8(%ebp), %edx 
	movl 12(%ebp), %ecx
	pushl $set_color_ret
	pushl %ebp 
	movl %esp, %ebp
	sysenter
set_color_ret:
	popl %ebp 
	addl $4, %esp 
	popl %edx 
	popl %ecx 
	test %eax, %eax
	jns set_color_end 
	negl %eax 
	movl %eax, errno
	movl $-1, %eax 
set_color_end:
	popl %ebp 
	ret

ENTRY(shmat)
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edx 
	movl $24, %eax         
	movl 8(%ebp), %edx      
	movl 12(%ebp), %ecx     
	pushl $shmat_ret
	pushl %ebp 
	movl %esp, %ebp
	sysenter
shmat_ret:
	popl %ebp 
	addl $4, %esp 
	popl %edx 
	popl %ecx 
	test %eax, %eax
	jns shmat_end 
	negl %eax 
	movl %eax, errno
	movl $-1, %eax 
shmat_end:
	popl %ebp 
	ret