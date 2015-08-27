# C++ Unit Testing

This is a simple header only unit testing library that implements the bare minimum functionality needed to effectively unit test. It is the result of having built a much larger and more comprehensive unit testing library and realising that most of it went unused and the extra boilerplate wasn't adding much.

# Sample Code

```C++
#include "Wild/UnitTesting.h"

using namespace std;

void ThrowException() { throw runtime_error("kaboom"); }

int main(int argc, char* argv[])
{
	int x = 1;
	int y = 2;
	string s = "foo";

	AssertTrue(x == y - 1);
	AssertEquals(x + 1, y);
	AssertEquals(s, "foo");
	AssertThrows(ThrowException(), runtime_error);
	AssertPrints(cout << s << "bar", "foobar");

	EndTest
}

```

# Installing

## Download

All you need to use this library is to put ```UnitTesting.h``` in your project and include it in your testing code.

## Nuget

There is also a Nuget package for Visual Studio users, more info at:

https://www.nuget.org/packages/WildUnitTesting

The correct include path to use after installing the Nuget package is

```C++
#include "Wild/UnitTesting.h"
```

# Using

It supports:

```C++
AssertTrue(value)                  // Test passes if value is true
AssertFalse(value)                 // Test passes if value is false
AssertEquals(a, b)                 // Test passes if a == b
AssertThrows(code, exception_type) // Test passes if code throws exception_type
AssertPrints(code, string)         // Test passes if code prints string
AssertPrintsToStderr(code, string) // As above but to stderr

EndTest  	// Prints results and exits with the number of failures
```

If a test fails it prints file and line number on failure in a format that allows double clicking to get straight to the line in Visual Studio. For example:

```C++
AssertEquals(x, y);  // x = 1, y = 2
```
results in
```
d:\wildcoast\github\unittesting\test\unittesting.test.cpp(29): test failed
```
or
```
d:\wildcoast\github\unittesting\test\unittesting.test.cpp(29): test failed, (x == y), actual comparison performed (1 == 2)
```
if ```WILD_UNITTESTING_SHOW_FAILURE_DETAILS``` has been defined.


## Preprocessor Flags

###WILD_UNITTESTING_SHOW_FAILURE_DETAILS

As shown above, additional details can be shown on failure if you compile with this preprocessor flag. The library will attempt to print out the values that were compared. Note that the types being compared must be streamable to a stringstream.

```x is streamable if "cout << x" compiles.```

You can add this to your classes by overloading the << operator.

###WILD_UNITTESTING_BREAK_ON_FAIL

If this is defined then a test failure will cause the application to break. This is useful when running in the debugger as the call stack will be available to look at what is going on and what is causing the failure.

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

	EndTest
}
```

In the post build step of ```StateMachine.Test``` I add ```"$(TargetPath)"``` (quotes included) which causes the exe, and therefore the tests to be run every time the project successfully builds.
