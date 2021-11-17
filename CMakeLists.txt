cmake_minimum_required(VERSION 3.1)
project(MuCplGen)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED True)

option(Examples "Build Examples" ON)

if(Examples)
    add_executable(ILGenerator
        "Examples/ILGenerator/main.cpp"
        "Examples/ILGenerator/ILSymbolTable.cpp"
        )
    file(COPY "${PROJECT_SOURCE_DIR}/assignment_test.txt" DESTINATION ${PROJECT_BINARY_DIR})
endif(Examples)

include_directories(${PROJECT_SOURCE_DIR})

#message(NOTICE "COPY ${PROJECT_SOURCE_DIR}/assignment_test.txt => ${PROJECT_BINARY_DIR}")