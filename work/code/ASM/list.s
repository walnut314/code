    .file   "list.c"
    .text
    .globl  reverse
    .def    reverse;    .scl    2;  .type   32; .endef
    .seh_proc   reverse
reverse:
    pushq   %rbp
    .seh_pushreg    %rbp
    movq    %rsp, %rbp
    .seh_setframe   %rbp, 0
    subq    $32, %rsp
    .seh_stackalloc 32
    .seh_endprologue
    movq    %rcx, 16(%rbp)
    movq    16(%rbp), %rax
    movq    (%rax), %rax
    movq    %rax, -8(%rbp)
    movq    $0, -16(%rbp)
    jmp .L2
.L3:
    movq    -8(%rbp), %rax
    movq    (%rax), %rax
    movq    %rax, -24(%rbp)
    movq    -8(%rbp), %rax
    movq    -16(%rbp), %rdx
    movq    %rdx, (%rax)
    movq    -8(%rbp), %rax
    movq    %rax, -16(%rbp)
    movq    -24(%rbp), %rax
    movq    %rax, -8(%rbp)
.L2:
    cmpq    $0, -8(%rbp)
    jne .L3
    movq    -16(%rbp), %rax
    addq    $32, %rsp
    popq    %rbp
    ret
    .seh_endproc
    .section .rdata,"dr"
.LC0:
    .ascii "add: %d\12\0"
    .text
    .globl  add
    .def    add;    .scl    2;  .type   32; .endef
    .seh_proc   add
add:
    pushq   %rbp
    .seh_pushreg    %rbp
    movq    %rsp, %rbp
    .seh_setframe   %rbp, 0
    subq    $48, %rsp
    .seh_stackalloc 48
    .seh_endprologue
    movq    %rcx, 16(%rbp)
    movl    %edx, 24(%rbp)
    movl    $16, %ecx
    call    malloc
    movq    %rax, -8(%rbp)
    movl    24(%rbp), %eax
    movl    %eax, %edx
    leaq    .LC0(%rip), %rcx
    call    printf
    movq    -8(%rbp), %rax
    movl    24(%rbp), %edx
    movl    %edx, 8(%rax)
    movq    16(%rbp), %rax
    movq    (%rax), %rax
    movq    %rax, -16(%rbp)
    movq    16(%rbp), %rax
    movq    -8(%rbp), %rdx
    movq    %rdx, (%rax)
    movq    -8(%rbp), %rax
    movq    -16(%rbp), %rdx
    movq    %rdx, (%rax)
    nop
    addq    $48, %rsp
    popq    %rbp
    ret
    .seh_endproc
    .section .rdata,"dr"
.LC1:
    .ascii "delete: %d\12\0"
    .text
    .globl  delete
    .def    delete; .scl    2;  .type   32; .endef
    .seh_proc   delete
delete:
    pushq   %rbp
    .seh_pushreg    %rbp
    movq    %rsp, %rbp
    .seh_setframe   %rbp, 0
    subq    $48, %rsp
    .seh_stackalloc 48
    .seh_endprologue
    movq    %rcx, 16(%rbp)
    movl    %edx, 24(%rbp)
    movq    16(%rbp), %rax
    movq    (%rax), %rax
    movq    %rax, -16(%rbp)
    movq    16(%rbp), %rax
    movq    %rax, -8(%rbp)
    jmp .L7
.L10:
    movq    -16(%rbp), %rax
    movl    8(%rax), %eax
    cmpl    %eax, 24(%rbp)
    jne .L8
    movl    24(%rbp), %eax
    movl    %eax, %edx
    leaq    .LC1(%rip), %rcx
    call    printf
    movq    -16(%rbp), %rax
    movq    (%rax), %rdx
    movq    -8(%rbp), %rax
    movq    %rdx, (%rax)
    jmp .L6
.L8:
    movq    -16(%rbp), %rax
    movq    %rax, -8(%rbp)
    movq    -16(%rbp), %rax
    movq    (%rax), %rax
    movq    %rax, -16(%rbp)
.L7:
    cmpq    $0, -16(%rbp)
    jne .L10
.L6:
    addq    $48, %rsp
    popq    %rbp
    ret
    .seh_endproc
    .section .rdata,"dr"
.LC2:
    .ascii "P: %d\12\0"
    .text
    .globl  dump
    .def    dump;   .scl    2;  .type   32; .endef
    .seh_proc   dump
dump:
    pushq   %rbp
    .seh_pushreg    %rbp
    movq    %rsp, %rbp
    .seh_setframe   %rbp, 0
    subq    $48, %rsp
    .seh_stackalloc 48
    .seh_endprologue
    movq    %rcx, 16(%rbp)
    movq    16(%rbp), %rax
    movq    (%rax), %rax
    movq    %rax, -8(%rbp)
    jmp .L12
.L13:
    movq    -8(%rbp), %rax
    movl    8(%rax), %eax
    movl    %eax, %edx
    leaq    .LC2(%rip), %rcx
    call    printf
    movq    -8(%rbp), %rax
    movq    (%rax), %rax
    movq    %rax, -8(%rbp)
.L12:
    cmpq    $0, -8(%rbp)
    jne .L13
    nop
    addq    $48, %rsp
    popq    %rbp
    ret
    .seh_endproc
    .def    __main; .scl    2;  .type   32; .endef
    .globl  main
    .def    main;   .scl    2;  .type   32; .endef
    .seh_proc   main
main:
    pushq   %rbp
    .seh_pushreg    %rbp
    movq    %rsp, %rbp
    .seh_setframe   %rbp, 0
    subq    $64, %rsp
    .seh_stackalloc 64
    .seh_endprologue
    call    __main
    movq    $0, -32(%rbp)
    movl    $0, -4(%rbp)
    jmp .L15
.L16:
    movl    -4(%rbp), %edx
    leaq    -32(%rbp), %rax
    movq    %rax, %rcx
    call    add
    addl    $1, -4(%rbp)
.L15:
    cmpl    $7, -4(%rbp)
    jle .L16
    leaq    -32(%rbp), %rax
    movq    %rax, %rcx
    call    dump
    leaq    -32(%rbp), %rax
    movq    %rax, %rcx
    call    reverse
    movq    %rax, %rdx
    movq    -16(%rbp), %rax
    movq    %rdx, (%rax)
    movq    -16(%rbp), %rax
    movl    $0, %edx
    movq    %rax, %rcx
    call    delete
    movq    -16(%rbp), %rax
    movl    $4, %edx
    movq    %rax, %rcx
    call    delete
    movq    -16(%rbp), %rax
    movl    $7, %edx
    movq    %rax, %rcx
    call    delete
    movq    -16(%rbp), %rax
    movq    %rax, %rcx
    call    dump
    movl    $0, %eax
    addq    $64, %rsp
    popq    %rbp
    ret
    .seh_endproc
    .ident  "GCC: (x86_64-posix-seh, Built by strawberryperl.com project) 8.3.0"
    .def    malloc; .scl    2;  .type   32; .endef
    .def    printf; .scl    2;  .type   32; .endef
