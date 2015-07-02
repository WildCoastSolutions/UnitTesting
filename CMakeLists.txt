cmake_minimum_required (VERSION 2.6)

project (UnitTesting)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DWILD_UNITTESTING_SHOW_FAILURE_DETAILS" )

add_subdirectory (Test)