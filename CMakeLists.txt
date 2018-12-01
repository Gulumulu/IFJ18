cmake_minimum_required(VERSION 3.12)
project(IFJ C)

set(CMAKE_C_STANDARD 99)

add_executable(IFJ main.c generate.c generate.h list.h queue.c queue.h errors.h errors.c symtable.h symtable.c semantic.h semantic.c prectable.h prectable.c predict.c predict.h scanner.c scanner.h syntax.h syntax.c if-generate.c if-generate.h)