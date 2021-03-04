#pragma once

#include "core/fanDebug.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/ecs/fanSlot.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "core/ecs/fanEcsComponent.hpp"
#include "core/ecs/fanEcsSingleton.hpp"

namespace fan
{
    namespace test
    {
        //========================================================================================================
        //========================================================================================================
        class UnitTestFileSystem : public UnitTest<UnitTestFileSystem>
        {
        public:
            static std::vector<TestMethod> GetTests()
            {
                return { { &UnitTestFileSystem::TestNormalizeBackSlashes,   "Normalize  back slashes" },
                         { &UnitTestFileSystem::TestNormalizeDoubleSlashes, "Normalize  double slashes" },
                         { &UnitTestFileSystem::TestNormalizeEndsSlashes,   "Normalize  ends slashes" },
                         { &UnitTestFileSystem::TestIsAbsolute,             "Is absolute" },
                         { &UnitTestFileSystem::TestEmptyInit,              "Empty init " },
                         { &UnitTestFileSystem::TestInitInvalidEnd,         "Init invalid end" },
                         { &UnitTestFileSystem::TestInitValid,              "Init valid" },
                         { &UnitTestFileSystem::TestNormalizeAbsolute,      "Normalize absolute" },
                };
            }

            void Create() override
            {
                FileSystem::Reset();
            }

            void Destroy() override
            {
                Debug::Get().onNewLog.Disconnect( (size_t)this );
            }

            void TestNormalizeBackSlashes()
            {
                const std::string unNormalized       = "D:\\code\\content/";
                const std::string expectedNormalized = "D:/code/content";
                const std::string normalized         = FileSystem::NormalizePath( unNormalized );
                TEST_ASSERT( normalized == expectedNormalized );
            }

            void TestNormalizeDoubleSlashes()
            {
                const std::string unNormalized       = "D:\\\\code/\\content//\\//\\/";
                const std::string expectedNormalized = "D:/code/content";
                const std::string normalized         = FileSystem::NormalizePath( unNormalized );
                TEST_ASSERT( normalized == expectedNormalized );
            }

            void TestNormalizeEndsSlashes()
            {
                const std::string unNormalized       = "/code/content/";
                const std::string expectedNormalized = "code/content";
                const std::string normalized         = FileSystem::NormalizePath( unNormalized );
                TEST_ASSERT( normalized == expectedNormalized );

                const std::string unNormalized2       = "\\code\\content\\";
                const std::string expectedNormalized2 = "code/content";
                const std::string normalized2         = FileSystem::NormalizePath( unNormalized2 );
                TEST_ASSERT( normalized2 == expectedNormalized2 );
            }

            void TestEmptyInit()
            {
                bool result = FileSystem::Init( "" );
                TEST_ASSERT( result == false );
                TEST_ASSERT( FileSystem::GetProjectPath() == "" );
            }

            void TestInitInvalidEnd()
            {
                const std::string contentPathInvalid = "D:/code/";
                bool              result             = FileSystem::Init( contentPathInvalid );
                TEST_ASSERT( result == true );
                TEST_ASSERT( FileSystem::GetProjectPath() == "D:/code" );
            }

            void TestInitValid()
            {
                const std::string contentPathValid = "D:/code";
                bool              result           = FileSystem::Init( contentPathValid );
                TEST_ASSERT( result == true );
                TEST_ASSERT( FileSystem::GetProjectPath() == contentPathValid );
            }

            void TestIsAbsolute()
            {
                const std::string absolute = "D:/code/";
                TEST_ASSERT( FileSystem::IsAbsolute( absolute ) == true );
                const std::string notAbsolute = "code/";
                TEST_ASSERT( FileSystem::IsAbsolute( notAbsolute ) == false );
                const std::string notAbsolute2 = ":";
                TEST_ASSERT( FileSystem::IsAbsolute( notAbsolute2 ) == false );
            }

            void TestNormalizeAbsolute()
            {
                const std::string contentPath = "D:/code";
                FileSystem::Init( contentPath );

                const std::string expectedResult = "D:/code/content/test.png";
                const std::string result         = FileSystem::NormalizePath( "content/test.png" );
                TEST_ASSERT( result == expectedResult );

                const std::string result2 = FileSystem::NormalizePath( "/content/test.png" );
                TEST_ASSERT( result2 == expectedResult );
            }
        };
    }
}