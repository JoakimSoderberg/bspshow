cmake_minimum_required (VERSION 2.6) 

project(bspshow)

set(BSPSHOW_VERSION_MAJOR 0)
set(BSPSHOW_VERSION_MINOR 1)
set(BSPSHOW_VERSION_MICRO 0)

set(BSPSHOW_SRCS
	bsp.c
	bsphelper.c
	draw.c
	input.c
	main.c
	utils.c
	)

set(BSPSHOW_LIBS)

if (WIN32)
	set(WITH_FREEGLUT 1)
	include_directories("win32/freeglut/include")
	link_directories("win32/freeglut/lib")
	list(APPEND BSPSHOW_LIBS freeglut.lib)
endif()

if (NOT WITH_FREEGLUT)
	find_package(GLUT REQUIRED)
	find_package(OpenGL REQUIRED)

	list(APPEND BSPSHOW_LIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
	include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
endif (NOT WITH_FREEGLUT)

# Create a config header for version numbers and compile time settings.
configure_file(
	"bspshow_config.h.in" 
	"${PROJECT_BINARY_DIR}/bspshow_config.h"
	)
include_directories(${PROJECT_BINARY_DIR})

add_executable(bspshow ${BSPSHOW_SRCS})

target_link_libraries(bspshow ${BSPSHOW_LIBS})

if (WITH_FREEGLUT)
	# Copy the freeglut.dll to the output dir.
	add_custom_command(TARGET ${AGENT_NAME} 
			POST_BUILD 
			COMMAND ${CMAKE_COMMAND} -E copy freeglut.dll $<TARGET_FILE_DIR:bspshow> VERBATIM)
endif()
