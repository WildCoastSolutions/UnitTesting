cmake_minimum_required (VERSION 2.6)

project (UnitTesting)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DWILD_UNITTESTING_SHOW_FAILURE_DETAIL" )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread" )

add_subdirectory (Test)