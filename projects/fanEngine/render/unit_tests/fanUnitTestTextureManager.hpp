#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "render/resources/fanTextureManager.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestTextureManager : public UnitTest<UnitTestTextureManager>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestTextureManager::TestLoad,          "Load" },
                    { &UnitTestTextureManager::TestMeshRemove,    "Remove" },
                    { &UnitTestTextureManager::TestIndexing,      "Indexing" },
                    { &UnitTestTextureManager::TestClear,         "Clear" },
                    { &UnitTestTextureManager::TestResolveRscPtr, "Resolve rsc ptr" },
                    { &UnitTestTextureManager::TestCreate,        "Create" },
                    { &UnitTestTextureManager::TestDestroy,       "Destroy" },
            };
        }
        void Create() override
        {
            Device* device = nullptr;
            mTextureManager.Clear( *device );
        }
        void Destroy() override
        {
            Device* device = nullptr;  // when testing we dont generate gpu buffers
            mTextureManager.Clear( *device );
        }

        TextureManager mTextureManager;

        void TestLoad()
        {
            TEST_ASSERT( mTextureManager.Load( "toto" ) == nullptr );

            Texture* defaultTexture = mTextureManager.Load( RenderGlobal::sDefaultTexture );
            TEST_ASSERT( defaultTexture != nullptr );
            Texture* whiteTexture = mTextureManager.GetOrLoad( RenderGlobal::sTextureWhite );
            TEST_ASSERT( whiteTexture != nullptr );

            TEST_ASSERT( mTextureManager.Get( RenderGlobal::sDefaultTexture ) == defaultTexture );
            TEST_ASSERT( mTextureManager.Get( RenderGlobal::sTextureWhite ) == whiteTexture );

            Texture* testTexture = new Texture();
            const std::string testTextureName = "test_texture";
            mTextureManager.Add( testTexture, testTextureName );
            TEST_ASSERT( mTextureManager.Get( testTextureName ) == testTexture );
        }

        void TestMeshRemove()
        {
            mTextureManager.Load( RenderGlobal::sDefaultTexture );
            mTextureManager.Remove( RenderGlobal::sDefaultTexture );
            TEST_ASSERT( mTextureManager.Get( RenderGlobal::sDefaultTexture ) == nullptr );
        }

        void TestIndexing()
        {
            Texture* generatedTexture = new Texture();
            const std::string generatedTextureName = "generated_texture";

            Texture* TextureWhite = mTextureManager.Load( RenderGlobal::sTextureWhite );
            mTextureManager.Add( generatedTexture, generatedTextureName );
            Texture* TextureDefault = mTextureManager.Load( RenderGlobal::sDefaultTexture );

            TEST_ASSERT( TextureWhite->mIndex == 0 );
            TEST_ASSERT( generatedTexture->mIndex == 1 );
            TEST_ASSERT( TextureDefault->mIndex == 2 );

            mTextureManager.Remove( generatedTextureName );
            TEST_ASSERT( TextureWhite->mIndex == 0 );
            TEST_ASSERT( TextureDefault->mIndex == 1 );
        }

        void TestClear()
        {
            mTextureManager.Load( RenderGlobal::sTextureWhite );
            Device* device = nullptr;
            TEST_ASSERT( !mTextureManager.Empty() );
            mTextureManager.Clear( *device );
            TEST_ASSERT( mTextureManager.Empty() );
        }

        void TestResolveRscPtr()
        {
            Texture* textureDefault = mTextureManager.Load( RenderGlobal::sDefaultTexture );
            TexturePtr rscPtr;
            TEST_ASSERT( !rscPtr.IsValid() );
            rscPtr.Init( RenderGlobal::sDefaultTexture );
            mTextureManager.ResolvePtr( rscPtr.mData );
            TEST_ASSERT( rscPtr.IsValid() );
            TEST_ASSERT( textureDefault == rscPtr.mData.mResource );
        }

        void TestCreate()
        {
            Texture* texture1 = new Texture();
            Texture* texture2 = new Texture();

            TEST_ASSERT( !texture1->mBuffersOutdated );
            TEST_ASSERT( !texture2->mBuffersOutdated );

            mTextureManager.Add( texture1, "texture1" );
            mTextureManager.Add( texture2, "texture2" );

            texture1->LoadFromPixels( nullptr, { 0, 0 }, 1 );
            texture2->LoadFromFile( RenderGlobal::sDefaultTexture );
            texture2->FreePixels();

            TEST_ASSERT( texture1->mBuffersOutdated );
            TEST_ASSERT( texture2->mBuffersOutdated );

            Device* device = nullptr;
            mTextureManager.CreateNewTextures( *device );

            TEST_ASSERT( !texture1->mBuffersOutdated );
            TEST_ASSERT( !texture2->mBuffersOutdated );
        }

        void TestDestroy()
        {
            mTextureManager.Load( RenderGlobal::sDefaultTexture );
            mTextureManager.Remove( RenderGlobal::sDefaultTexture );
            TEST_ASSERT( mTextureManager.DestroyListSize() == 1 );
            Device* device = nullptr;
            mTextureManager.Clear( *device );
            TEST_ASSERT( mTextureManager.DestroyListSize() == 0 );
        }
    };
}