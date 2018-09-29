include(CMakeToolsHelpers OPTIONAL)

cmake_minimum_required(VERSION 3.0.0)
project(libnatasha2 VERSION 0.1.0)

aux_source_directory(. DIR_LIB_SRCS)

add_library(libnatasha2 ${DIR_LIB_SRCS})