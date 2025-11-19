cmake_minimum_required(VERSION 3.30)
project(prog3A)

set(CMAKE_CXX_STANDARD 20)

add_executable(prog3A
        tas.cpp
        bounded_buffer_hw_template.cpp
        semaphore.h
        bounded_buffer_hw.h
        bounded_buffer_os_template.cpp
        semaphore_template.cpp
        bounded_buffer_os.h
        driver.cpp)
