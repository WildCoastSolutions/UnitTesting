
include_directories (../)

add_executable (UnitTesting UnitTesting.Test.cpp AdditionalTestFile.cpp)

add_custom_command(
	TARGET UnitTesting POST_BUILD
   	COMMAND UnitTesting
 )
