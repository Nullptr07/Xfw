cmake_minimum_required(VERSION 3.17)
project(Project2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/nullptr07/Sources/Project1/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "────────────────────────────────────────────────────────────────────────────────────────────────────────")
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "${CMAKE_BINARY_DIR}")
message(STATUS "────────────────────────────────────────────────────────────────────────────────────────────────────────")

# Compile Flags Here

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS " \
-fconcepts \
-std=c++2a \
-fsanitize=undefined \
-fsanitize-undefined-trap-on-error \
-O3")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_llvm_library(main MODULE main.cpp PLUGIN_TOOL clang)

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))
    target_link_libraries(RenameFunctions PRIVATE
            clangAST
            clangBasic
            clangFrontend
            LLVMSupport
            )
endif()

# add_executable(Project2 main.cpp Functional.h ArgumentUtil.h XList.h XObject.h XString.h Functional/Lambda.h Functional/Func.h Utility.h Functional/DynamicBinding.h)