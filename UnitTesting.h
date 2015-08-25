//
// Author       Wild Coast Solutions
//              David Hamilton
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.
//
// This file contains an implementation of a simple unit testing framework. It uses a class
// to hold information about the running tests, and macros to help with easy testing.
//
// WILD_UNITTESTING_SHOW_FAILURE_DETAILS can be defined to print out the values that were 
// compared in AssertEquals, but note that the types being compared must be streamable to a stringstream. 
//      i.e. x is streamable if "cout << x" compiles.

#ifndef WILD_UNITTESTING_H
#define WILD_UNITTESTING_H

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

namespace Wild
{
    namespace UnitTesting
    {

        // Class to record our testing progress, i.e. number of passed and failed tests.
        // In theory many instances of this could be used to track different types of tests but
        // for now it's just designed to be used once
        class Test
        {
        public:

            Test() : passed(0), failed(0), total(0)
            {}

            // Test whether a boolean condition is true or false and handle the result
            void Assert(bool test, const std::string &file, int line, const std::string &details = 0)
            {
                test ? Pass() : Fail(file, line, details);
            }

            // Mark a test as passed
            void Pass()
            {
                passed++;
                total++;
            }

            // Mark a test as failed and output useful info about where and what failed
            void Fail(const std::string &file, int line, const std::string &details)
            {
#ifdef WILD_UNITTESTING_SHOW_FAILURE_DETAILS
                std::cout << file << "(" << line << "): test failed, " << details << std::endl;
#else
                std::cout << file << "(" << line << "): test failed" << std::endl;
#endif

#ifdef WILD_UNITTESTING_BREAK_ON_FAIL
                // This is designed for when the tests are failing and you want to look at the call stack
                // in the debugger to see what's going on
                std::abort();
#endif
                failed++;
                total++;
            }

            void Results()
            {
                std::cout << passed << " passed, " << failed << " failed, " << total << " total" << std::endl;
            }

            // Running counts for relevant stats. We'll leave them public and trust the user application not to modify them.
            int passed;
            int failed;
            int total;
        };

        // Make sure that each file that includes this header is using the same object.
        // This is useful to keep a global count of passed and failed tests across all files.
        inline Test& AllTests()
        {
            static Test t = Test();
            return t;
        }

        // Helper functions in case the user application wants to use these values
        inline int Passed() { return AllTests().passed; }
        inline int Failed() { return AllTests().failed; }
        inline int Total() { return AllTests().total; }
    }
}


// Macros to facilitate the testing process

#define Pass() Wild::UnitTesting::AllTests().Pass();

#define Fail(details) Wild::UnitTesting::AllTests().Fail(__FILE__, __LINE__, details);

#define AssertTrueWithDetails(x, details) Wild::UnitTesting::AllTests().Assert(x, __FILE__, __LINE__, details);


// Macros for use by user applications

#define AssertTrue(x) AssertTrueWithDetails(x, #x " does not evaluate to true");
#define AssertFalse(x) AssertTrueWithDetails(!x, #x " evaluates to true");

#ifdef WILD_UNITTESTING_SHOW_FAILURE_DETAILS
static std::stringstream wildUnitTestingSs;
#define AssertEquals(x, y) \
    wildUnitTestingSs << "(" << #x << " == " << #y <<")" \
        << ", actual comparison performed (" << x << " == " << y << ")"; \
    AssertTrueWithDetails(x == y, wildUnitTestingSs.str()); \
    wildUnitTestingSs.str("");
#else
#define AssertEquals(x, y) \
    AssertTrueWithDetails(x == y, "no details");
#endif


#define AssertThrows(code, ex) try \
{ \
    code; \
    Fail(""); \
} \
catch (ex){ Pass(); } \
catch (...){ Fail(""); }

#define AssertPrints(code, x) { \
    std::stringstream wildUnitTestingOutput; \
    std::streambuf* wildUnitTestingOriginal = std::cout.rdbuf(wildUnitTestingOutput.rdbuf()); \
    code; \
    std::string wildUnitTestingOutputText = wildUnitTestingOutput.str(); \
    wildUnitTestingOutput.str(""); \
    std::cout.rdbuf(wildUnitTestingOriginal); \
    AssertEquals(wildUnitTestingOutputText, x) \
}

#define AssertPrintsToStderr(code, x) { \
    std::stringstream wildUnitTestingOutput; \
    std::streambuf* wildUnitTestingOriginal = std::cerr.rdbuf(wildUnitTestingOutput.rdbuf()); \
    code; \
    std::string wildUnitTestingOutputText = wildUnitTestingOutput.str(); \
    wildUnitTestingOutput.str(""); \
    std::cerr.rdbuf(wildUnitTestingOriginal); \
    AssertEquals(wildUnitTestingOutputText, x) \
}

#define EndTest Wild::UnitTesting::AllTests().Results(); return Wild::UnitTesting::AllTests().failed;



#endif