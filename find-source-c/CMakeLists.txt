set(LLVM_USED_LIBS clangTooling)

add_clang_executable(find-source-c find-source-c.cpp)

target_link_libraries(find-source-c
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS find-source-c
  RUNTIME DESTINATION bin)