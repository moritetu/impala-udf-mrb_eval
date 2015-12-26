cmake_minimum_required(VERSION 2.8)

set(MRUBY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vendor/mruby")
message("MRUBY_ROOT: ${MRUBY_ROOT}")
include_directories(${MRUBY_ROOT}/include)
link_directories(${MRUBY_ROOT}/build/host/lib)
add_library(mrb_eval SHARED mrb_eval.cpp)
target_link_libraries(mrb_eval ImpalaUdf -Wl,--whole-archive mruby -Wl,-no-whole-archive)

# Test
add_executable(mrb_eval-test mrb_eval-test.cpp)
target_link_libraries(mrb_eval-test mrb_eval)
