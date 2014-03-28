set(LLVM_TARGET_DEFINITIONS xpic.td)

tablegen(xpicGenRegisterInfo.h.inc -gen-register-desc-header)
tablegen(xpicGenRegisterNames.inc -gen-register-enums)
tablegen(xpicGenRegisterInfo.inc -gen-register-desc)
tablegen(xpicGenInstrNames.inc -gen-instr-enums)
tablegen(xpicGenInstrInfo.inc -gen-instr-desc)
tablegen(xpicGenAsmWriter.inc -gen-asm-writer)
tablegen(xpicGenDAGISel.inc -gen-dag-isel)
tablegen(xpicGenSubtarget.inc -gen-subtarget)
tablegen(xpicGenCallingConv.inc -gen-callingconv)

add_llvm_target(xpic
  xpicDelaySlotFiller.cpp
  xpicInstrInfo.cpp
  xpicISelDAGToDAG.cpp
  xpicISelLowering.cpp
  xpicRegisterInfo.cpp
  xpicSubtarget.cpp
  xpicMCAsmInfo.cpp
  xpicTargetMachine.cpp
  ) 
