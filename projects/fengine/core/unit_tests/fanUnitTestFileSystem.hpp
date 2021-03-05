#pragma once

#include "core/fanFileSystem.hpp"
#include "core/unit_tests/fanUnitTest.hpp"

namespace fan
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
                         { &UnitTestFileSystem::TestInitAppendContent,      "Init append content 1" },
                         { &UnitTestFileSystem::TestAppendContent2,         "Init append content 2" },
                         { &UnitTestFileSystem::TestNormalizeAbsolute,      "Normalize absolute" },
                };
            }

            void Create() override
            {
                FileSystem::Reset();
            }

            void Destroy() override
            {
                FileSystem::Reset();
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
                bool result = FileSystem::SetProjectPath( "" );
                TEST_ASSERT( result == false );
                TEST_ASSERT( FileSystem::GetProjectPath() == "" );
            }

            void TestInitAppendContent()
            {
                bool              result             = FileSystem::SetProjectPath( "D:/code/" );
                TEST_ASSERT( result == true );
                TEST_ASSERT( FileSystem::GetProjectPath() == "D:/code/content" );
            }

            void TestAppendContent2()
            {
                bool              result           = FileSystem::SetProjectPath( "D:/code" );
                TEST_ASSERT( result == true );
                TEST_ASSERT( FileSystem::GetProjectPath() == "D:/code/content" );
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
                FileSystem::SetProjectPath( "D:/code" );
                const std::string result         = FileSystem::NormalizePath( "test.png" );
                TEST_ASSERT( result == "D:/code/content/test.png" );

                const std::string result2 = FileSystem::NormalizePath( "/test.png" );
                TEST_ASSERT( result2 == "D:/code/content/test.png" );
            }
        };

}