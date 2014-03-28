include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMXpecInfo
  xpicTargetInfo.cpp
  )

add_dependencies(LLVMXpicInfo xpicTable_gen)
