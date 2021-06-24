# 检查cmake的最低版本
cmake_minimum_required(VERSION 3.10.0)

# 设置在VS中建立文件夹
SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS ON )

# 定义工程名称
project(cgal_practice)
find_package(CGAL)

find_package(OpenSceneGraph  REQUIRED 
	osg 
	osgGA 
	osgDB 
	osgViewer 
	osgUtil 
	osgText 
	osgQt 
	osgVolume 
	osgTerrain 
	osgSim 
	osgFX 
	osgAnimation 
	osgManipulator
	osgParticle
)

IF((OSG_LIBRARY OR OSG_LIBRARY_DEBUG) AND OSG_INCLUDE_DIR)

ELSE()
	MESSAGE(FATAL_ERROR "Fatal:  OSG NO FOUND")
ENDIF()

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/*.h)


INCLUDE_DIRECTORIES(
    ${OSG_INCLUDE_DIR}
)


# 生成可执行程序
add_executable(${PROJECT_NAME}
	${HEADERS}
	${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
	${OPENSCENEGRAPH_LIBRARIES} 
	CGAL::CGAL
) 

message("${OSG_LIBRARY}")
message("${GLFW_LIBRARIES}")
