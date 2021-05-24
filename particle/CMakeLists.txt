#osg入门
SET(OUT_NAME particle_test)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
SET(APPLICATION_DIR ${CMAKE_SOURCE_DIR}/particle)

file(GLOB SOURCES ${APPLICATION_DIR}/*.cpp)
file(GLOB HEADERS ${APPLICATION_DIR}/*.h)

add_executable(
	${OUT_NAME}
	${HEADERS}
	${SOURCES}
	)
	
#说明1.此处添加依赖库
target_link_libraries(
    ${OUT_NAME}
	${OPENSCENEGRAPH_LIBRARIES}
	)