 set(LLVM_USED_LIBS clangTooling)

add_clang_executable(find-source find-source.cpp)

target_link_libraries(find-source
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS find-source
  RUNTIME DESTINATION bin)