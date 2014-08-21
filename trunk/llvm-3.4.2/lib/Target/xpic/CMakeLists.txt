set(LLVM_TARGET_DEFINITIONS xpic.td)

tablegen(LLVM xpicGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM xpicGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM xpicGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM xpicGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM xpicGenCallingConv.inc -gen-callingconv)
tablegen(LLVM xpicGenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(xpicCommonTableGen)

add_llvm_target(xpicCodeGen
  xpicDelaySlotFiller.cpp
  xpicISelDAGToDAG.cpp
  xpicISelLowering.cpp
  xpicInstrInfo.cpp
  xpicFrameLowering.cpp
  xpicMachineFunctionInfo.cpp
  xpicRegisterInfo.cpp
  xpicSubtarget.cpp
  xpicTargetMachine.cpp
  xpicSelectionDAGInfo.cpp
  xpicAsmPrinter.cpp
  ) 

add_dependencies(LLVMxpicCodeGen xpicCommonTableGen intrinsics_gen)

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(Helper)
