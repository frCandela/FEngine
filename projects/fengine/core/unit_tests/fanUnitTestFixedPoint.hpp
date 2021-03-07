#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "core/math/fanFixedPoint.hpp"

namespace fan
{
        //======================================================================================================================
        //======================================================================================================================
        class UnitTestFixedPoint : public UnitTest<UnitTestFixedPoint>
        {
        public:
            static std::vector<TestMethod> GetTests()
            {
                return { { &UnitTestFixedPoint::Test,   "Test" },
                };
            }

            void Create() override
            {
            }

            void Destroy() override
            {
            }

            void Test()
            {
                TEST_ASSERT( false );
            }
        };

}