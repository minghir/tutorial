section	.text
	global _start       ;must be declared for using gcc
_start:                     ;tell linker entry point
	mov	edx, len 
	mov ecx, msg;message length
	mov	ebx, 1	    ;file descriptor (stdout)
	mov	eax, 4	    ;system call number (sys_write)
	int	0x80        ;call kernel
	
	mov	edx, len2
	mov	ecx, msg2    ;message to write
	mov	ebx, 1	    ;file descriptor (stdout)
	mov	eax, 4	    ;system call number (sys_write)
	int	0x80        ;call kernel
	mov	eax, 1	    ;system call number (sys_exit)
	int	0x80        ;call kernel
	

section	.data
msg	db	'Hello, world!',0xa	;our dear string
len	equ	$ - msg			;length of our dear string
msg2 db 'MIMI',0xa
len2	equ	$ - msg2		;length of our dear string
