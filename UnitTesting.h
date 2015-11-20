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
// Project url: https://github.com/WildCoastSolutions/UnitTesting

#ifndef WILD_UNITTESTING_H
#define WILD_UNITTESTING_H

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <mutex>

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
            {
            }

            Test(const Test&) = delete;

            // Test whether a boolean condition is true or false and handle the result
            void Assert(bool test, const std::string &file, int line, const std::string &details = 0)
            {
                test ? Pass() : Fail(file, line, details);
            }

            // Mark a test as passed
            void Pass()
            {
                std::lock_guard<std::recursive_mutex> lock(mutex);
                passed++;
                total++;
            }

            // Mark a test as failed and output useful info about where and what failed
            void Fail(const std::string &file, int line, const std::string &details)
            {
                std::lock_guard<std::recursive_mutex> lock(mutex);
                std::stringstream output;
#ifdef WILD_UNITTESTING_SHOW_FAILURE_DETAILS
                output << file << "(" << line << "): test failed, " << details << std::endl;
#else
                output << file << "(" << line << "): test failed" << std::endl;
#endif
                std::cout << output.str() << std::flush;

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

            std::recursive_mutex mutex;  // Protect stdout and counts in Pass & Fail from being written to from different threads
        };

        // Make sure that each file that includes this header is using the same object.
        // This is useful to keep a global count of passed and failed tests across all files.
        static Test AllTests;

        // Helper functions in case the user application wants to use these values
        inline int Passed() { return AllTests.passed; }
        inline int Failed() { return AllTests.failed; }
        inline int Total() { return AllTests.total; }
    }
}


// Macros to facilitate the testing process

#define Pass() { \
    Wild::UnitTesting::AllTests.Pass(); \
}

#define AssertTrueWithDetails(x, details){ \
    Wild::UnitTesting::AllTests.Assert(x, __FILE__, __LINE__, details); \
}


#define Fail(details) { \
    Wild::UnitTesting::AllTests.Fail(__FILE__, __LINE__, details); \
}

#ifdef WILD_UNITTESTING_SHOW_FAILURE_DETAILS
#define InternalAssertEquals(x, y) { \
    std::stringstream wildUnitTestingSs; \
    wildUnitTestingSs << "(" << #x << " == " << #y <<")" \
        << ", actual comparison performed (" << x << " == " << y << ")"; \
    AssertTrueWithDetails(x == y, wildUnitTestingSs.str()); \
    wildUnitTestingSs.str(""); \
} 
#else
#define InternalAssertEquals(x, y){ \
    AssertTrueWithDetails(x == y, "no details"); \
}
#endif


// Macros for use by user applications


#define AssertTrue(x) { \
    AssertTrueWithDetails(x, #x " does not evaluate to true"); \
}

#define AssertFalse(x) { \
    AssertTrueWithDetails(!x, #x " evaluates to true"); \
}

#define AssertEquals(x, y) { \
    InternalAssertEquals(x, y) \
} 


#define AssertThrows(code, ex) { \
    try \
    { \
        code; \
        Fail(""); \
    } \
    catch (ex){ Pass(); } \
    catch (...){ Fail(""); } \
}


#define AssertPrints(code, x) { \
    std::lock_guard<std::recursive_mutex> wildUnitTestingLock(Wild::UnitTesting::AllTests.mutex); \
    std::stringstream wildUnitTestingOutput; \
    std::streambuf* wildUnitTestingOriginal = std::cout.rdbuf(wildUnitTestingOutput.rdbuf()); \
    code; \
    std::string wildUnitTestingOutputText = wildUnitTestingOutput.str(); \
    wildUnitTestingOutput.str(""); \
    std::cout.rdbuf(wildUnitTestingOriginal); \
    InternalAssertEquals(wildUnitTestingOutputText, x) \
}


#define AssertPrintsToStderr(code, x) { \
    std::lock_guard<std::recursive_mutex> wildUnitTestingLock(Wild::UnitTesting::AllTests.mutex); \
    std::stringstream wildUnitTestingOutput; \
    std::streambuf* wildUnitTestingOriginal = std::cerr.rdbuf(wildUnitTestingOutput.rdbuf()); \
    code; \
    std::string wildUnitTestingOutputText = wildUnitTestingOutput.str(); \
    wildUnitTestingOutput.str(""); \
    std::cerr.rdbuf(wildUnitTestingOriginal); \
    InternalAssertEquals(wildUnitTestingOutputText, x) \
}

#define EndTest { \
    std::lock_guard<std::recursive_mutex> wildUnitTestingLock(Wild::UnitTesting::AllTests.mutex); \
    Wild::UnitTesting::AllTests.Results(); return Wild::UnitTesting::AllTests.failed; \
}



#endif