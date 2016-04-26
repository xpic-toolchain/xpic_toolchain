// test 
// following commands have to be rejected because expression inside [] is not valid
add r6, [#400 + -- r3],r0
add r6, [#400 + --r3],r0
// following command are valid
add r6,[#400],r0
// following commands have to be rejected because expression inside [] is not valid
add r6, [#400 + -- r4  ],r0
// following command are valid
add r6,[#400],r0
