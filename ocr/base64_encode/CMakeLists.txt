
### https://www.ibm.com/developerworks/cn/linux/l-cn-cmake/index.html
cmake_minimum_required(VERSION 2.8.8)
project(lc_lite)
include(basic.cmake)

set(ROOT_DIR ${CMAKE_BINARY_DIR}/bin)

include_directories(
	/usr/local/include
)

### src list
aux_source_directory(src          LC_SRC_BASE)
	 
### set bin out path
set(EXECUTABLE_OUTPUT_PATH ${ROOT_DIR})

### lib path 
link_directories(
	/usr/local/lib64
)
	
### bin
set(EXECUTABLE_NAME  lc_lite_main_${DEV_TYPE})
add_executable(${EXECUTABLE_NAME}
	main.cpp
	${LC_SRC_BASE}
)








  
