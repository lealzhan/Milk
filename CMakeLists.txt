cmake_minimum_required(VERSION 3.10.2)
PROJECT(Milk)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

message(STATUS "Windows平台")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 -std=c++11")
SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")


FILE (GLOB HEADER_FILES
	include/*.h  
	include/*.hpp  
)
FILE (GLOB SOURCE_FILES
	src/*.cpp  
)

SOURCE_GROUP("Header Files" FILES ${HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${SOURCE_FILES})

ADD_EXECUTABLE(Prefilter ${HEADER_FILES} ${SOURCE_FILES})


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/dependencies/include/)

set(FREEIMAGE_LIBRARIES "${PROJECT_SOURCE_DIR}/dependencies/lib/x64/FreeImage.lib")
TARGET_LINK_LIBRARIES(Prefilter ${FREEIMAGE_LIBRARIES})
set(FREEGLUT_LIBRARIES "${PROJECT_SOURCE_DIR}/dependencies/lib/x64/freeglutd.lib")
TARGET_LINK_LIBRARIES(Prefilter ${FREEGLUT_LIBRARIES})
set(GLEW32_LIBRARIES "${PROJECT_SOURCE_DIR}/dependencies/lib/x64/glew32.lib")
TARGET_LINK_LIBRARIES(Prefilter ${GLEW32_LIBRARIES})

add_custom_command(TARGET Prefilter POST_BUILD        					# Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  					# which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/dependencies/bin/x64/freeglut.dll"      	# <--this is in-file
        $<TARGET_FILE_DIR:Prefilter>)                 					# <--this is out-file path

add_custom_command(TARGET Prefilter POST_BUILD        					# Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  					# which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/dependencies/bin/x64/glew32.dll"      	# <--this is in-file
        $<TARGET_FILE_DIR:Prefilter>)                 					# <--this is out-file path

		
add_custom_command(TARGET Prefilter POST_BUILD        					# Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  					# which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/dependencies/bin/x64/FreeImage.dll"      	# <--this is in-file
        $<TARGET_FILE_DIR:Prefilter>)                 					# <--this is out-file path