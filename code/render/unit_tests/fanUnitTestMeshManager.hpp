#pragma once

#include "core/fanUnitTest.hpp"
#include "render/fanMeshManager.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestMeshManager : public UnitTest< UnitTestMeshManager >
    {
    public:
        static std::vector <TestMethod> GetTests()
        {
            return {
                    { &UnitTestMeshManager::TestSomething,     "something" },
                    { &UnitTestMeshManager::TestSomethingElse, "something else" },
            };
        }
        void Create() override {}
        void Destroy() override {}
        void TestSomething()     { TEST_ASSERT( true );  }
        void TestSomethingElse() { TEST_ASSERT( false ); }
    };
}