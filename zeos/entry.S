/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>


/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %edx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %ebx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %ebx; \
      pushl %ecx; \
      pushl %edx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es

#define RESTORE_ALL \
      pop %edx; \
      pop %ecx; \
      pop %ebx; \
      pop %esi; \
      pop %edi; \
      pop %ebp; \
      pop %eax; \
      pop %ds;  \
      pop %es;  \
      pop %fs;  \
      pop %gs

#define EOI \
      movb $0x20, %al ; \
      outb %al, $0x20 ;


ENTRY(syscall_handler_sysenter)
      push $__USER_DS
      push %ebp
      pushfl
      push $__USER_CS
      push 4(%ebp)
      SAVE_ALL
      cmpl $0, %eax
      jl sysenter_err
      cmpl $MAX_SYSCALL, %eax
      jg sysenter_err
      call *sys_call_table(, %eax, 0x04)
      jmp sysenter_fin
sysenter_err:
      movl $-38, %eax
sysenter_fin:
      movl %eax, 0x18(%esp)
      RESTORE_ALL
      movl (%esp), %edx 
      movl 12(%esp), %ecx
      sti
      sysexit

ENTRY(writeMSR)
      pushl %ebp
      movl %esp, %ebp
      movl 0x8(%ebp), %ecx
      movl $0, %edx      
      movl 0xc(%ebp), %eax 
      wrmsr
      movl %ebp, %esp
      pop %ebp
      ret

ENTRY(keyboard_handler)
      SAVE_ALL
      EOI
      call keyboard_routine
      RESTORE_ALL
      /* Check if need_sched is set, and if so call sched_next before iret */
      cmpl $0, need_sched
      je keyboard_no_sched
      movl $0, need_sched
      call sched_next
keyboard_no_sched:
      iret

ENTRY(clock_handler)
      SAVE_ALL
      EOI
      call clock_routine
      RESTORE_ALL
      /* Check if need_sched is set, and if so call sched_next before iret */
      cmpl $0, need_sched
      je clock_no_sched
      movl $0, need_sched
      call sched_next
clock_no_sched:
      iret

ENTRY(my_page_fault_handler)
      call my_page_fault_routine

ENTRY(task_switch)
      // dynamic link
      pushl %ebp
      movl %esp, %ebp

      // salvar els 3 registres
      pushl %esi
      pushl %edi
      pushl %ebx

      // cridar a la funció inner amb el paràmetre *newproc
      pushl 8(%ebp)
      call inner_task_switch

      // buidar de la pila el paràmetre
      addl $4, %esp

      // restaurar els 3 registres
      pop %ebx
      pop %edi
      pop %esi

      // desfer el dynamic link i retornar
      pop %ebp
      ret

ENTRY(update_ebp_esp)
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax
    movl %esp, (%eax)
    movl 12(%ebp), %esp
    popl %ebp
    ret

ENTRY(ret_from_fork)
      movl $0, %eax
      popl %ebp
      ret

ENTRY(get_ebp)
      movl %ebp, %eax
      ret