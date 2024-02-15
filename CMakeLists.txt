cmake_minimum_required(VERSION 3.25)

project(Interface CXX)

add_executable(Interface
    src/DefineInterface.h
    src/Main.cpp
)

target_compile_features(Interface PRIVATE cxx_std_23)
