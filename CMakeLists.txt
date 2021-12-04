cmake_minimum_required(VERSION 3.1)
project(MuCplGen)
set(CMAKE_CXX_STANDARD 17)

message("build type = ${CMAKE_BUILD_TYPE}")
option(Examples "Build Example-" on)
option(Debug on)
if(Debug)
    set(CMAKE_BUILD_TYPE "Debug")
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()
message("build type = ${CMAKE_BUILD_TYPE}")
set(gpp off)
message("System=${CMAKE_SYSTEM_NAME}")
message("Compiler=${CMAKE_CXX_COMPILER_ID}")
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(gpp on)
endif()

set(CMAKE_CXX_FLAGS "--std=c++17 -lstdc++fs")

set(CMAKE_CXX_STANDARD_REQUIRED true)

if(Examples)
    add_executable(
        Calculator
        "Examples/Calculator/main.cpp"
        "Examples/Calculator/Calculator.h"
    )
    add_executable(
        PDAConflict
        "Examples/PDAConflict/main.cpp"
    )
    add_executable(
        CalculatorSubModule
        "Examples/CalculatorSubModule/main.cpp"
        "Examples/CalculatorSubModule/Calculator.h"
    )
    add_executable(
        Sequence
        "Examples/Sequence/main.cpp"
    )
    add_executable(
        SolveSemanticError
        "Examples/SolveSemanticError/main.cpp"
        "Examples/SolveSemanticError/Calculator.h"
    )
    add_executable(
        AutoShaderVariableDetector
        "Examples/AutoShaderVariableDetector/main.cpp"
        "Examples/AutoShaderVariableDetector/VariableReader.h"
    )
    add_executable(
        ILGenerator
        "Examples/ILGenerator/main.cpp"
        "Examples/ILGenerator/ILSymbolTable.cpp"
        "Examples/ILGenerator/ILGenerator.h"
        )

    if(gpp)
        target_link_libraries(Calculator stdc++fs)
        target_link_libraries(PDAConflict stdc++fs)
        target_link_libraries(CalculatorSubModule stdc++fs)
        target_link_libraries(Sequence stdc++fs)
        target_link_libraries(SolveSemanticError stdc++fs)
        target_link_libraries(AutoShaderVariableDetector stdc++fs)
        target_link_libraries(ILGenerator stdc++fs)
        message("link: stdc++fs")
    endif()
    file(COPY "${PROJECT_SOURCE_DIR}/assignment_test.txt" DESTINATION ${PROJECT_BINARY_DIR})
    file(COPY "${PROJECT_SOURCE_DIR}/Auto.shader" DESTINATION ${PROJECT_BINARY_DIR})
endif(Examples)

include_directories(${PROJECT_SOURCE_DIR})

#message(NOTICE "COPY ${PROJECT_SOURCE_DIR}/assignment_test.txt => ${PROJECT_BINARY_DIR}")
