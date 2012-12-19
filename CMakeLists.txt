cmake_minimum_required (VERSION 2.6) 

project(bspshow)

set(BSPSHOW_SRCS
	bsp.c
	bsphelper.c
	draw.c
	input.c
	main.c
	utils.c
	)

set(BSPSHOW_LIBS)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

list(APPEND BSPSHOW_LIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

add_executable(bspshow ${BSPSHOW_SRCS})

target_link_libraries(bspshow ${BSPSHOW_LIBS})
