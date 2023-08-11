cmake_minimum_required(VERSION 3.8)
project(id-chip-reader-example LANGUAGES C)

add_executable(id_chip_reader_example id_chip_reader_example.c)
target_link_libraries(id_chip_reader_example PRIVATE id_chip_reader)