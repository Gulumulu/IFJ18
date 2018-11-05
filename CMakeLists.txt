cmake_minimum_required(VERSION 3.12)
project(IFJ C)

set(CMAKE_C_STANDARD 99)

add_executable(IFJ main.c errors.h errors.c symtable.h symtable.c sematnic.h sematnic.c prectable.h prectable.c predict.c predict.h)