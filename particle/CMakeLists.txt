#osg入门
SET(OUT_NAME particle_test)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
SET(APPLICATION_DIR ${CMAKE_SOURCE_DIR}/particle)

file(GLOB SRC ${APPLICATION_DIR}/*.*)
SOURCE_GROUP(Src FILES ${SRC})

add_executable(
	${OUT_NAME}
	${SRC}
	)
	
#说明1.此处添加依赖库
target_link_libraries(
    ${OUT_NAME}
	${OPENSCENEGRAPH_LIBRARIES}
	)