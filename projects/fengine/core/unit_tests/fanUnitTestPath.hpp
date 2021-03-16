#pragma once

#include "core/fanPath.hpp"
#include "core/unit_tests/fanUnitTest.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestPath : public UnitTest<UnitTestPath>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestPath::TestNormalize,         "Normalize" },
                     { &UnitTestPath::TestIsAbsolute,        "Is absolute" },
                     { &UnitTestPath::TestSetProjectPath,    "Set project path" },
                     { &UnitTestPath::TestNormalizeAbsolute, "Normalize absolute" },
                     { &UnitTestPath::TestDirectory,         "Directory" },
                     { &UnitTestPath::TestFilename,          "Filename" },
                     { &UnitTestPath::TestExtension,         "Extension" },
                     { &UnitTestPath::TestParent,            "Parent" },
                     { &UnitTestPath::TestList,              "List" },
            };
        }

        std::string mOldProjectPath;

        void Create() override
        {
            mOldProjectPath = Path::GetProjectPath();
            Path::Reset();
        }

        void Destroy() override
        {
            Path::Reset();
            if( !mOldProjectPath.empty() )
            {
                Path::SetProjectPath( mOldProjectPath );
            }
        }

        void TestNormalize()
        {
            // back slashes
            TEST_ASSERT( Path::Normalize( "D:\\test\\patate/" ) == "D:/test/patate/" );
            TEST_ASSERT( Path::Normalize( "\\models\\patate" ) == "content/models/patate" );

            // double slashes
            TEST_ASSERT( Path::Normalize( "D:\\\\test/\\content" ) == "D:/test/content" );
            TEST_ASSERT( Path::Normalize( "\\\\models/\\patate//\\//\\/" ) == "content/models/patate/" );

            // ends slashes
            TEST_ASSERT( Path::Normalize( "/models/patate" ) == "content/models/patate" );
            TEST_ASSERT( Path::Normalize( "\\models\\patate\\" ) == "content/models/patate/" );
        }

        void TestSetProjectPath()
        {
            bool result = Path::SetProjectPath( "" );
            TEST_ASSERT( result == false );
            TEST_ASSERT( Path::GetProjectPath() == "" );

            Path::SetProjectPath( "D:/test" );
            TEST_ASSERT( Path::GetProjectPath() == "D:/test/" );

            Path::Reset();
            Path::SetProjectPath( "D:////test/\\//" );
            TEST_ASSERT( Path::GetProjectPath() == "D:/test/" );
        }

        void TestIsAbsolute()
        {
            TEST_ASSERT( Path::IsAbsolute( "D:/code/" ) == true );
            TEST_ASSERT( Path::IsAbsolute( "code/" ) == false );
            TEST_ASSERT( Path::IsAbsolute( ":" ) == false );
            TEST_ASSERT( Path::IsAbsolute( "" ) == false );
        }

        void TestNormalizeAbsolute()
        {
            Path::SetProjectPath( "D:/test" );
            TEST_ASSERT( Path::Normalize( "test.png" ) == "D:/test/content/test.png" );
            TEST_ASSERT( Path::Normalize( "/test.png" ) == "D:/test/content/test.png" );
        }

        void TestDirectory()
        {
            TEST_ASSERT( !Path::IsDirectory( "file.png" ) );
            TEST_ASSERT( !Path::IsDirectory( "file" ) );
            TEST_ASSERT( Path::IsDirectory( "file/" ) );
            TEST_ASSERT( Path::IsDirectory( "/" ) );
            TEST_ASSERT( Path::IsDirectory( "" ) );

            TEST_ASSERT( Path::Directory( "file.png" ) == "/" );
            TEST_ASSERT( Path::Directory( "/" ) == "/" );
            TEST_ASSERT( Path::Directory( "lol/file.png") == "lol/" );
            TEST_ASSERT( Path::Directory( "D:/lol/file.png" ) == "D:/lol/" );
            TEST_ASSERT( Path::Directory("D:/lol/filewithoutextension") == "D:/lol/" );
        }

        void TestFilename()
        {
            TEST_ASSERT( Path::FileName( "file.png" ) == "file.png" );
            TEST_ASSERT( Path::FileName( "lol/file.png") == "file.png" );
            TEST_ASSERT( Path::FileName( "D:/lol/file.png" ) == "file.png" );
            TEST_ASSERT( Path::FileName("D:/lol/filewithoutextension") == "filewithoutextension" );
            TEST_ASSERT( Path::FileName("D:/lol/") == "" );
        }

        void TestExtension()
        {
            TEST_ASSERT( Path::Extension( "file.png" ) == "png" );
            TEST_ASSERT( Path::Extension( "/" ) == "" );
            TEST_ASSERT( Path::Extension( "test/" ) == "" );
            TEST_ASSERT( Path::Extension( "test/bwa" ) == "" );
        }

        void TestParent()
        {
            TEST_ASSERT( !Path::IsRootDrive("") );
            TEST_ASSERT( !Path::IsRootDrive("/") );
            TEST_ASSERT( !Path::IsRootDrive("d/") );
            TEST_ASSERT( !Path::IsRootDrive("d:/a") );
            TEST_ASSERT( Path::IsRootDrive("d:/") );

            TEST_ASSERT( Path::Parent( "D:/parent/son/" ) == "D:/parent/" );
            TEST_ASSERT( Path::Parent( "D:/parent/son/file" ) == "D:/parent/son/" );
            TEST_ASSERT( Path::Parent( "D:/parent/son/file.png" ) == "D:/parent/son/" );
            TEST_ASSERT( Path::Parent( "/" ) == "/" );
            TEST_ASSERT( Path::Parent( "" ) == "/" );
            TEST_ASSERT( Path::Parent( "D:/bwa" ) == "D:/" );
            TEST_ASSERT( Path::Parent( "D:/" ) == "D:/" );
        }

        void TestList()
        {
            Path::SetProjectPath( mOldProjectPath );
            std::vector files = Path::ListDirectory( Path::Normalize("/") );
            TEST_ASSERT( ! files.empty() );
            for( std::string path : files )
            {
                TEST_ASSERT( Path::Normalize(path) == path );
            }
        }
    };
}