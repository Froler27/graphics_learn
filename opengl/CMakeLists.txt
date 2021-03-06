#osg入门
SET(OUT_NAME opengl_test)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
SET(APPLICATION_DIR ${CMAKE_SOURCE_DIR}/opengl)

set(GLFW_INCLUDE_DIR D:/Froler/environment/GLFW334_64/include)
set(GLFW_LIB D:/Froler/environment/GLFW334_64/lib/glfw3.lib)
set(GLAD_INCLUDE_DIR D:/Froler/environment/glad/include)
set(GLW_INCLUDE_DIR D:/Froler/environment/glm)

set(ASSIMP_DIR D:/Froler/environment/Assimp)

include_directories(
    ${GLFW_INCLUDE_DIR}
	${GLAD_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/include
	${GLW_INCLUDE_DIR}
	${ASSIMP_DIR}/include
)

file(GLOB SOURCES ${APPLICATION_DIR}/*.cpp)
file(GLOB HEADERS ${APPLICATION_DIR}/*.h)
file(GLOB SOURCES_COMMON ${CMAKE_SOURCE_DIR}/common/*.cpp)
file(GLOB SHADERS ${CMAKE_SOURCE_DIR}/shaders/*.*)

source_group("Shaders" FILES ${SHADERS})

add_executable(
	${OUT_NAME}
	${HEADERS}
	${SOURCES}
	${SOURCES_COMMON}
	${GLAD_INCLUDE_DIR}/../src/glad.c
	${SHADERS}
	)
	
#说明1.此处添加依赖库
target_link_libraries(
    ${OUT_NAME}
	${OPENSCENEGRAPH_LIBRARIES}
	${GLFW_LIB}
	${ASSIMP_DIR}/lib/assimp-vc140-mt.lib
	${ASSIMP_DIR}/lib/IrrXML.lib
	)