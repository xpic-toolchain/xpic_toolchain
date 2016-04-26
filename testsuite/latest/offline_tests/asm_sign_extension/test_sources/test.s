// following standard commands without sign-extension have to by correctly handled by xpic-compiler
store [pc + #-100]w, u4
store [pc + #-99]b, u4
store [pc + #-104]d, u4
nop
store [pc + #-1000]d, u4
store [pc + #-10400]d, u4
store [pc + #253604]d, u4
nop

