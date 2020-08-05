#pragma once

#include "core/fanUnitTest.hpp"
#include "scene/input/fanGlfwMouse.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestGlfwMouse : public UnitTest<UnitTestGlfwMouse>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestGlfwMouse::TestLoad,       "Load" },
            };
        }
        void Create() override {}
        void Destroy() override {}

        void TestLoad()
        {

        }

    };
}