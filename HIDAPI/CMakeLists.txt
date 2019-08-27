cmake_minimum_required(VERSION 3.0)

set(SRCS linux/hid.c)   
set(HEADERS hidapi.h)

add_library(hidapi ${SRCS} ${HEADERS})
