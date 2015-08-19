# C++ Unit Testing

This is a simple header only unit testing library that implements the bare minimum functionality needed to effectively unit test. It is the result of having built a much larger and more comprehensive unit testing library and realising that most of it went unused and the extra boilerplate wasn't adding much.

# Usage

All you need to use this library is ```UnitTesting.h```, for how to use it have a look at ```Test/UnitTesting.Test.cpp``` and the example below.

It supports:

```
AssertTrue(value)
AssertEquals(a, b)
AssertThrows(code, exception_type)
AssertPrints(code, string)
```

If a test fails it prints file and line number on failure in a format that allows double clicking to get to the line in Visual Studio.

Additional details can be shown on failure if you compile with the preprocessor flag ```WILD_UNITTESTING_SHOW_FAILURE_DETAILS```.
This will attempt to print out the values that were compared. Note that the types being compared must be streamable to a stringstream. 

```x is streamable if "cout << x" compiles.```

You can add this to your classes by overloading the << operator.

## Platforms

It was built on VS2013 but should work on earlier versions, and tested on gcc 4.8.2. Info in this document is VS specific but all the concepts translate to Linux. The Linux build uses cmake, to test the lib it you can clone it, enter the directory and use

```
cmake .
make
Test\UnitTesting
```

## Suggested Use

The way we use it is to link the code under test and the unit testing framework into an executable project which contains testing code. This executable is run as a post build step and returns the number of test failures. 

This means that the tests are run on every build, and the build fails if any tests fail. It does mean that code under test needs to be divided up into small, self-contained libraries, which works well for our project architecture.

## Example

Say I am building a StateMachine library and want to test the basic functionality, such as transitioning between states. This code is in a library called StateMachine which compiles to a static library ```StateMachine.lib```.

I create the project ```StateMachine.Test``` in a folder related to StateMachine (sub folder or alongside) and add the StateMachine library as a reference and include ```UnitTesting.h```. This project builds an executable that is designed to return 0 if all tests pass, otherwise the number of failures. I add this to the solution so it is built on a full rebuild.

Note that the macro ```EndTest``` prints out the results and outputs the required value for you.

```C++
#include "StateMachine.h"
#include "UnitTesting.h"

using namespace Wild::StateMachine;

int main(int argc, char* argv[])
{
	// setup state machine under test, add states and actions etc
	...

	// Test state machine
	sm.HandleEvent(Event::CallStart, callStartMessage);
	AssertEquals(sm.CurrentState(), StateName::Recording);
	AssertEquals(callStartMessage.get()->handled, true);
	
	sm.HandleEvent(Event::CallEnd);
	AssertEquals(sm.CurrentState(), StateName::Idle);

	sm.HandleEvent(Event::CallEnd);
	sm.HandleEvent(Event::CallStart, callStartMessage);

	AssertEquals(rec.startCount, 2);
	AssertEquals(rec.stopCount, 1);

	// AssertThrows example
    AssertThrows(ThrowsOutOfRange(), std::out_of_range);

	EndTest
}
```

In the post build step of ```StateMachine.Test``` I add ```"$(TargetPath)"``` (quotes included) which causes the exe, and therefore the tests to be run every time the project successfully builds.
