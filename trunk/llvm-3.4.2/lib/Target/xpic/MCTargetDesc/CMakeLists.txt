add_llvm_library(LLVMxpicDesc
  xpicMCTargetDesc.cpp
  xpicMCAsmInfo.cpp
  )

add_dependencies(LLVMxpicDesc xpicCommonTableGen)
