/*
    This file is part of Corrade.

    Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,
                2017, 2018, 2019 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "Corrade/Containers/ScopedExit.h"
#include "Corrade/TestSuite/Tester.h"

namespace Corrade { namespace Containers { namespace Test { namespace {

struct ScopedExitTest: TestSuite::Tester {
    explicit ScopedExitTest();

    void pointer();
    void value();
    void lambda();
    void returningLambda();
    void release();
};

ScopedExitTest::ScopedExitTest() {
    addTests({&ScopedExitTest::pointer,
              &ScopedExitTest::value,
              &ScopedExitTest::lambda,
              &ScopedExitTest::returningLambda,
              &ScopedExitTest::release});
}

int fd;
void close(float* value) { *value = 3.14f; }
int closeInt(int) { fd = 42; return 5; }

void ScopedExitTest::pointer() {
    float v = 0.0f;
    {
        ScopedExit e{&v, close};
    }
    CORRADE_COMPARE(v, 3.14f);
}

void ScopedExitTest::value() {
    {
        fd = 1337;
        ScopedExit e{fd, closeInt};
    }
    CORRADE_COMPARE(fd, 42);
}

void ScopedExitTest::lambda() {
    {
        fd = 0;
        ScopedExit{&fd, [](int* handle) {
            *handle = 7;
        }};
    }
    CORRADE_COMPARE(fd, 7);
}

void ScopedExitTest::returningLambda() {
    #ifdef CORRADE_MSVC2015_COMPATIBILITY
    CORRADE_SKIP("Lambdas with non-void return type are not supported on MSVC2015 due to a compiler limitation.");
    #else
    {
        fd = 0;
        ScopedExit{&fd, [](int* handle) {
            *handle = 7;
            return true;
        }};
    }
    CORRADE_COMPARE(fd, 7);
    #endif
}

void ScopedExitTest::release() {
    float v = 1.234f;
    {
        ScopedExit e{&v, close};
        e.release();
    }
    CORRADE_COMPARE(v, 1.234f);
}

}}}}

CORRADE_TEST_MAIN(Corrade::Containers::Test::ScopedExitTest)
