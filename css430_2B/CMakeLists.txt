cmake_minimum_required(VERSION 3.30)
project(css430_2B)

set(CMAKE_CXX_STANDARD 20)

add_executable(css430_2B driver.cpp
        scheduler.h
        scheduler_template.cpp
        task.cpp)
