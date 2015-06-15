#pragma once
#ifndef WILD_UNIT_TESTING_H
#define WILD_UNIT_TESTING_H

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

namespace Wild
{
    namespace UnitTesting
    {
        class Test
        {
        public:

            void Assert(bool test, const std::string &file, int line, const std::string &details = 0)
            {
                test ? Pass() : Fail(file, line, details);
            }

            void Pass()
            {
                passed++;
                total++;
            }

            void Fail(const std::string &file, int line, const std::string &details)
            {
                std::cout << file << "(" << line << "): test failed, tried \"" << details << "\"" << std::endl;
                failed++;
                total++;
            }

            void Results()
            {
                std::cout << passed << " passed, " << failed << " failed, " << total << " total" << std::endl;
            }

            // Running counts for relevant stats. We'll trust the user application not to modify them.
            int passed = 0;
            int failed = 0;
            int total = 0;
        };

        // Make sure that each file that includes this header is using the same object.
        // This is useful to keep a global count of passed and failed tests across all files.
        inline Test& AllTests()
        {
            static Test t = Test();
            return t;
        }
    }
}


// Macros to make the testing process easier

#define Pass() Wild::UnitTesting::AllTests().Pass();

#define Fail(details) Wild::UnitTesting::AllTests().Fail(__FILE__, __LINE__, details);

#define AssertTrue(x, details) Wild::UnitTesting::AllTests().Assert(x, __FILE__, __LINE__, details);

static std::stringstream wildUnitTestingSs;
#define AssertEquals(x, y) \
    wildUnitTestingSs << x << " == " << y; \
    AssertTrue(x == y, wildUnitTestingSs.str()); \
    wildUnitTestingSs.str("");

#define AssertThrows(code, ex) try \
{ \
    code; \
    Fail(""); \
} \
catch (ex){ Pass(); } \
catch (...){ Fail(""); }

#define EndTest Wild::UnitTesting::AllTests().Results(); return Wild::UnitTesting::AllTests().failed;



#endif