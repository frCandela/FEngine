#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "render/resources/fanFontManager.hpp"
#include "render/resources/fanFont.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestFontManager : public UnitTest<UnitTestFontManager>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestFontManager::TestAdd,             "Add" },
                    { &UnitTestFontManager::TestRemove,          "Remove" },
                    { &UnitTestFontManager::TestClear,           "Clear" },
                    { &UnitTestFontManager::TestDuplicates,      "Duplicates" },
                    { &UnitTestFontManager::TestLoadInvalidFont, "Load Invalid" },
            };
        }
        void Create() override {}
        void Destroy() override {}

        FontManager mFontManager;

        void TestAdd()
        {
            TEST_ASSERT( mFontManager.Empty() );
            Font * font = mFontManager.Load( RenderGlobal::sDefaultGameFont );
            TEST_ASSERT( font->GetPath() == RenderGlobal::sDefaultGameFont );
            TEST_ASSERT( ! mFontManager.Empty() );
            TEST_ASSERT( mFontManager.Count() == 1 );
        }

        void TestRemove()
        {
            const std::string fontPath = "content/fonts/VeraBd.ttf";
            Font * font = mFontManager.Load( RenderGlobal::sDefaultGameFont );
            Font * font2 = mFontManager.Load( fontPath );
            TEST_ASSERT( mFontManager.Find( RenderGlobal::sDefaultGameFont ) == font );
            TEST_ASSERT( mFontManager.Find( fontPath ) == font2 );
            TEST_ASSERT( mFontManager.Count() == 2 );
            TEST_ASSERT( ! mFontManager.Empty() );
            mFontManager.Remove( RenderGlobal::sDefaultGameFont );
            TEST_ASSERT( mFontManager.Count() == 1 );
            TEST_ASSERT( ! mFontManager.Empty() );
            TEST_ASSERT( mFontManager.Find( RenderGlobal::sDefaultGameFont ) == nullptr );
            mFontManager.Remove( fontPath );
            TEST_ASSERT( mFontManager.Find( fontPath ) == nullptr );
            TEST_ASSERT( mFontManager.Empty() );
        }

        void TestClear() {
            mFontManager.Load( RenderGlobal::sDefaultGameFont );
            TEST_ASSERT( ! mFontManager.Empty() );
            mFontManager.Clear();
            TEST_ASSERT( mFontManager.Empty() );
        }

        void TestDuplicates()
        {
            Font * font1 = mFontManager.Load(  RenderGlobal::sDefaultGameFont );
            Font * font2 = mFontManager.Load(  RenderGlobal::sDefaultGameFont );

            TEST_ASSERT( font1 != nullptr );
            TEST_ASSERT( font2 != nullptr );
            TEST_ASSERT( font1 == font2 );
        }

        void TestLoadInvalidFont()
        {
            Font * font1 = mFontManager.Load( "qsdfsdfsdfsdf" );
            TEST_ASSERT( font1 == nullptr );
        }
    };
}