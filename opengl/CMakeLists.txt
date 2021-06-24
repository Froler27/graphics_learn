#osg入门
SET(OUT_NAME opengl_test)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
SET(APPLICATION_DIR ${CMAKE_SOURCE_DIR}/opengl)

set(GLFW_INCLUDE_DIR D:/Froler/environment/GLFW334_64/include)
set(GLFW_LIB D:/Froler/environment/GLFW334_64/lib/glfw3.lib)
set(GLAD_INCLUDE_DIR D:/Froler/environment/glad/include)

include_directories(
    ${GLFW_INCLUDE_DIR}
	${GLAD_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/include
)

file(GLOB SOURCES ${APPLICATION_DIR}/*.cpp)
file(GLOB HEADERS ${APPLICATION_DIR}/*.h)
file(GLOB SOURCES_COMMON ${CMAKE_SOURCE_DIR}/common/*.cpp)

add_executable(
	${OUT_NAME}
	${HEADERS}
	${SOURCES}
	${SOURCES_COMMON}
	${GLAD_INCLUDE_DIR}/../src/glad.c
	)
	
#说明1.此处添加依赖库
target_link_libraries(
    ${OUT_NAME}
	${OPENSCENEGRAPH_LIBRARIES}
	${GLFW_LIB}
	)