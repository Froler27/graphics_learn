# 检查cmake的最低版本
cmake_minimum_required(VERSION 3.10.0)

# 设置在VS中建立文件夹
SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS ON )

# 定义工程名称
project(Test)

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

INCLUDE_DIRECTORIES(
    ${OSG_INCLUDE_DIR}
)


# 生成可执行程序
add_executable(${PROJECT_NAME}
	main.cpp
)

target_link_libraries(
	${PROJECT_NAME}
	${OPENSCENEGRAPH_LIBRARIES} 
) 

message("${OSG_LIBRARY}")
message("${OPENSCENEGRAPH_LIBRARIES}")

#添加子项目
MACRO(SUBDIRLIST result curdir)
	FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
	SET(dirlist "")
	FOREACH(child ${children})
		IF(IS_DIRECTORY ${curdir}/${child})
			LIST(APPEND dirlist ${child})
		ENDIF()
	ENDFOREACH()
	SET(${result} ${dirlist})
ENDMACRO()
SUBDIRLIST(SRC_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(src_dir ${SRC_DIR_LIST})
	IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}/CMakeLists.txt")
		add_subdirectory(${src_dir})
	ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}/CMakeLists.txt")
ENDFOREACH()