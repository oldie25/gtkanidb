cmake_minimum_required(VERSION 2.6)
project(gtkanidb)

set(SRC net.c
		anidb.c
		main.c)

add_executable(gtkanidb ${SRC})
