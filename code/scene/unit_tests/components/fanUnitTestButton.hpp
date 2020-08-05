#pragma once

#include "core/fanUnitTest.hpp"
#include "scene/fanPrefabManager.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestButton : public UnitTest<UnitTestButton>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestButton::TestLoad,       "Load" },
            };
        }
        void Create() override {}
        void Destroy() override {}

        void TestLoad()
        {

        }

    };
}