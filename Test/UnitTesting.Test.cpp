

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

int main(int argc, char* argv[])
{
    AssertEquals(13, 13);
    AssertEquals(true, true);
    AssertEquals(false, false);
    std::string s = "foo";
    AssertEquals("foo", s);

    Test test;
    test.Assert(true, "filename", 1, "passed");
    AssertEquals(1, test.passed);
    AssertEquals(0, test.failed);

    std::stringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    test.Assert(false, "filename", 2, "false == false");
    std::string text = output.str();
    output.str("");

    Fail("test"); std::string failLine = std::string(__FILE__) + "(" + std::to_string(__LINE__) + ")";
    std::string text2 = output.str();

    std::cout.rdbuf(original);

    AssertEquals(text, "filename(2): test failed, tried \"false == false\"\n");
    AssertEquals(text2, failLine + ": test failed, tried \"test\"\n");

    AssertEquals(1, test.passed);
    AssertEquals(1, test.failed);
}

