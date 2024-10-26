If (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.7)
    message(STATUS "C++11 activated.")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -W")
	add_definitions (-ggdb  -g -Wall -std=c++11 -gdwarf-2 -O2 -D_GNU_SOURCE=1 -D__STDC_LIMIT_MACROS=1 )
else()
    add_definitions (-ggdb  -Wall -D_GNU_SOURCE=1 -D__STDC_LIMIT_MACROS=1)
    message(STATUS "C++98 activated.")
endif()


#if you want to compile static lib    
#link_libraries(-static-libstdc++ -static-libgcc)


