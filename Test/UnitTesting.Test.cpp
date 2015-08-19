
#include "UnitTesting.h"

using namespace Wild::UnitTesting;

void ThrowsInvalidArgument()
{
    throw std::invalid_argument("foo");
}

void ThrowsOutOfRange()
{
    throw std::out_of_range("foo");
}


void ThrowException() { throw std::runtime_error("kaboom"); }

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

    // Unit testing library tests

    AssertTrue(true);
    bool bar = true;
    AssertTrue(bar);
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

    if (Failed() == 1)  // we called Fail directly to test the output so we expect one failure
    {
        std::cout << "Unit testing tests passed" << std::endl;
        return 0;
    }
    else
        return 1;
}

