
#include "UnitTesting.h"
#include "Tests.h"
#include <thread>
#include <set>

using namespace Wild::UnitTesting;
using namespace std;

void ThrowsInvalidArgument()
{
    throw std::invalid_argument("foo");
}

void ThrowsOutOfRange()
{
    throw std::out_of_range("foo");
}


void ThrowException() { throw std::runtime_error("kaboom"); }


// threads create a bunch of test failures so we can check that the operation of the
// library is thread safe - that the output and pass/fails counts are correct
void Thread1()
{
    for (int i = 0; i < 100; i++)
    {
        AssertTrue(false);
        AssertFalse(true);
        AssertEquals(1, 2);
        AssertPrints(cout << "foo", "bar");
        AssertPrintsToStderr(cerr << "bar", "foo");
        AssertThrows(throw invalid_argument("foo"), out_of_range);
    }
}

void Thread2()
{
    for (int i = 0; i < 100; i++)
    {
        AssertTrue(false);
        AssertFalse(true);
        AssertEquals(1, 2);
        AssertPrints(cout << "foo", "bar");
        AssertPrintsToStderr(cerr << "bar", "foo");
        AssertThrows(throw invalid_argument("foo"), out_of_range);
    }
}

void TestThreadSafety()
{
    // output acts as cout from here, and will be accessed concurrently from both threads
    // which is a good test of whether the library can handle it
    stringstream output;
    streambuf* original = std::cout.rdbuf(output.rdbuf());
    stringstream outputStderr;
    streambuf* originalStderr = std::cerr.rdbuf(outputStderr.rdbuf());

    thread t1(Thread1);
    thread t2(Thread2);
    t2.join();
    t1.join();

    std::cout.rdbuf(original);
    std::cerr.rdbuf(originalStderr);

    // error printouts should not be interleaved, and failure count should be correct
    string line;

    // These tests below check the line length to see if any lines have been interleaved by
    // concurrent access to stdout. It's not guaranteed to produce a failure, but often does,
    // so we can be confident that the thread safety works if there are no failures across
    // multiple test runs.

    // Also, AssertPrints flat out crashes if the code isn't thread safe
    
#ifdef WILD_UNITTESTING_SHOW_FAILURE_DETAILS
    // Possible sizes of the failure details lines created by the failed tests in the threads.
    // Different environments have different ways of representing the filename, hence the length differences.
    set<size_t> possibleLineLengths = {
        107, 154, 98, 76, 122,  // WIN32
        70, 61, 117, 39, 85,    // X64
        118, 109, 133, 165, 87, // GCC
    };
    while (std::getline(output, line, '\n')) {
        AssertTrue(possibleLineLengths.find(line.size()) != possibleLineLengths.end());
    }
    while (std::getline(outputStderr, line, '\n')) {
        AssertTrue(possibleLineLengths.find(line.size()) != possibleLineLengths.end());
    }
#else
    // Possible sizes of the failure details lines created by the failed tests in the threads.
    // Different environments have different ways of representing the filename, hence the length differences.
    set<size_t> possibleLineLengths = {
        74,     // WIN32
        37,     // X64
        85      // GCC
    };
    while (std::getline(output, line, '\n')) {
        AssertTrue(possibleLineLengths.find(line.size()) != possibleLineLengths.end());
    }
    while (std::getline(outputStderr, line, '\n')) {
        AssertTrue(possibleLineLengths.find(line.size()) != possibleLineLengths.end());
    }
#endif

    AssertEquals(Failed(), 1200);
    
}

int main(int argc, char* argv[])
{
    // Readme example tests

    int x = 1;
    int y = 2;
    std::string s = "foo";

    AssertTrue(x == y - 1);
    AssertEquals(x + 1, y);
    AssertEquals(s, "foo");
    AssertThrows(ThrowException(), std::runtime_error);
    AssertPrints(std::cout << s << "bar", "foobar");
    AssertPrints(std::cout << s << "bar" << endl; , "foobar\n");

    // Unit testing library tests

    TestThreadSafety();

    AssertTrue(true);
    bool bar = true;
    AssertTrue(bar);
    AssertFalse(!bar);
    AssertEquals(13, 13);
    AssertEquals(true, true);
    AssertEquals(false, false);
    AssertEquals("foo", s);

    Test test;
    test.Assert(true, "filename", 1, "passed");
    AssertEquals(1, test.passed);
    AssertEquals(0, test.failed);

    std::stringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    test.Assert(false, "filename", 2, "(false == true)");
    std::string text = output.str();
    output.str("");

    std::stringstream failFileAndLine;
    Fail("test"); failFileAndLine << __FILE__ << "(" << __LINE__ << ")";
    std::string text2 = output.str();

    std::cout.rdbuf(original);

#ifdef WILD_UNITTESTING_SHOW_FAILURE_DETAILS
    AssertEquals(text, "filename(2): test failed, (false == true)\n");
    AssertEquals(text2, failFileAndLine.str() + ": test failed, test\n");
#else
    AssertEquals(text, "filename(2): test failed\n");
    AssertEquals(text2, failFileAndLine.str() + ": test failed\n");
#endif

    AssertEquals(1, test.passed);
    AssertEquals(1, test.failed);

    AssertThrows(throw std::out_of_range("foo"), std::out_of_range);
    AssertThrows(ThrowsOutOfRange(), std::out_of_range);
    AssertThrows(ThrowsInvalidArgument(), std::invalid_argument);
    AssertPrints(Wild::UnitTesting::AllTests::instance().Results(), "1223 passed, 1201 failed, 2424 total\n");

    AdditionalTests();

    // Two additional tests should have passed
    AssertPrints(Wild::UnitTesting::AllTests::instance().Results(), "1225 passed, 1201 failed, 2426 total\n");

    if (Failed() > 0)  // we called Fail directly to test the output so we expect one failure, plus all the threading test fails
    {
        std::cout << "Unit testing tests passed" << std::endl;
        return 0;
    }
    else
        return 1;
}

