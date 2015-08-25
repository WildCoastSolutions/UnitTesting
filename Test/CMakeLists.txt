
include_directories (../)

# Make sure the linker can find the Hello library once it is built.
#link_directories (${WILDLIB_BINARY_DIR}/Example)

add_executable (UnitTesting UnitTesting.Test.cpp)

add_custom_command(
	TARGET UnitTesting POST_BUILD
   	COMMAND UnitTesting
 )
