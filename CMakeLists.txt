cmake_minimum_required(VERSION 2.6)
project(gtkanidb)

set(SRC net.c
		control_net.c
		main.c)

add_executable(gtkanidb ${SRC})
