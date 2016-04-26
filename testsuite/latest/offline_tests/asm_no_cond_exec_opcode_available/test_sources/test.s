// test 
// write opcode with condition for an opcode without condition
add r6, [#400],r0
// following command has to be rejected because no condition allowed for that command
[z] add r6, [#400],r0
// write opcode with condition for an opcode without condition
add r6, [#400],r0
// following command has to be rejected because no condition allowed for that command
[z] add r6, [#400],r0
