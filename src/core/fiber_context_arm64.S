
.text
.p2align 2

.global rex_fiber_swap_context
.type rex_fiber_swap_context, %function

rex_fiber_swap_context:
    // Save callee-saved general-purpose registers.
    stp x19, x20, [x0, #0]
    stp x21, x22, [x0, #16]
    stp x23, x24, [x0, #32]
    stp x25, x26, [x0, #48]
    stp x27, x28, [x0, #64]
    stp x29, x30, [x0, #80]

    // Save stack pointer.
    mov x9, sp
    str x9, [x0, #96]

    // Save callee-saved floating-point registers.
    stp d8, d9, [x0, #104]
    stp d10, d11, [x0, #120]
    stp d12, d13, [x0, #136]
    stp d14, d15, [x0, #152]

    // Restore target floating-point registers.
    ldp d8, d9, [x1, #104]
    ldp d10, d11, [x1, #120]
    ldp d12, d13, [x1, #136]
    ldp d14, d15, [x1, #152]

    // Restore target general-purpose registers.
    ldp x19, x20, [x1, #0]
    ldp x21, x22, [x1, #16]
    ldp x23, x24, [x1, #32]
    ldp x25, x26, [x1, #48]
    ldp x27, x28, [x1, #64]
    ldp x29, x30, [x1, #80]

    // Restore target stack pointer and resume it.
    ldr x9, [x1, #96]
    mov sp, x9
    ret

.size rex_fiber_swap_context, .-rex_fiber_swap_context
