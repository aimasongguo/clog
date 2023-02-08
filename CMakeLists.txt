cmake_minimum_required(VERSION 3.10)

project(clog)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(
    clog
    main.c
    clog.c
    clog.h
    test.c
    test.h
)