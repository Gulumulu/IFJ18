cmake_minimum_required(VERSION 3.12)
project(IFJ C)

set(CMAKE_C_STANDARD 99)

add_executable(IFJ main.c symtable.h symtable.c prectable.c prectable.h errors.h errors.c sematnic.c sematnic.h)