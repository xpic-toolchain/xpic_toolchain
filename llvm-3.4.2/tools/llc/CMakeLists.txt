set(LLVM_LINK_COMPONENTS ${LLVM_TARGETS_TO_BUILD} bitreader asmparser irreader)

execute_process(COMMAND git describe --always --long --abbrev=40 --dirty=+
                WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_OUTPUT
                ERROR_VARIABLE GIT_ERROR
                RESULT_VARIABLE GIT_RESULT)
if (GIT_RESULT EQUAL "0")
  add_definitions(-DGIT_HASH=${GIT_OUTPUT})
else()
  message(WARNING "Git failed with code: ${GIT_RESULT}")
  message(WARNING "Git output: ${GIT_OUTPUT}")
  message(WARNING "Git error: ${GIT_ERROR}")
endif()

add_llvm_tool(llc
  llc.cpp
  )
