#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include <sstream>
#include <stdexcept>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include "stl_container_ostream.hpp"

template<class T, class U>
void AssertEqual(const T &t, const U &u, const std::string &hint = {}) {
    if (!(t == u)) {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty()) {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

inline void Assert(bool b, const std::string &hint) {
    AssertEqual(b, true, hint);
}

class TestRunner {
public:
    template<class TestFunc>
    void RunTest(TestFunc func, const std::string &test_name) {
        try {
            func();
            std::cerr << test_name << " OK" << std::endl;
        } catch (std::exception &e) {
            ++fail_count;
            std::cerr << test_name << " fail: " << e.what() << std::endl;
        } catch (...) {
            ++fail_count;
            std::cerr << "Unknown exception caught" << std::endl;
        }
    }

    ~TestRunner() {
        std::cerr.flush();
        if (fail_count > 0) {
            std::cerr << fail_count << " unit tests failed. Terminate" << std::endl;
            exit(1);
        }
    }

private:
    int fail_count = 0;
};

#ifndef FILE_NAME
#define FILE_NAME __FILE__
#endif

#define ASSERT_EQUAL(x, y) {                          \
  std::ostringstream __assert_equal_private_os;       \
  __assert_equal_private_os                           \
    << #x << " != " << #y << ", "                     \
    << FILE_NAME << ':' << __LINE__;                  \
  AssertEqual(x, y, __assert_equal_private_os.str()); \
}

#define ASSERT(x) {                          \
  std::ostringstream __assert_private_os;    \
  __assert_private_os << #x << " is false, " \
    << FILE_NAME << ':' << __LINE__;         \
  Assert(x, __assert_private_os.str());      \
}

#define RUN_TEST(tr, func) \
  tr.RunTest(func, #func)

#endif //TEST_RUNNER_HPP