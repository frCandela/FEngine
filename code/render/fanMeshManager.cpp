#include "render/fanMeshManager.hpp"

#include <filesystem>
#include "render/fanMesh.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/core/fanDevice.hpp"
#include "core/fanDebug.hpp"


namespace fan
{
    //========================================================================================================
    //========================================================================================================
    MeshManager::~MeshManager()
    {
        if( ! mMeshes.empty() || ! mDestroyList.empty() )
        {
            Debug::Warning() << "Mesh manager was not cleared before destroy" << Debug::Endl();
        }
    }

    //========================================================================================================
    //========================================================================================================
    Mesh* MeshManager::GetMesh( const std::string& _path ) const
    {
        const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();
        for( Mesh* mesh : mMeshes )
        {
            if( mesh->mPath == cleanPath )
            {
                return mesh;
            }
        }
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    Mesh* MeshManager::GetOrLoad( const std::string& _path )
    {
        Mesh * mesh = GetMesh( _path );
        if( mesh == nullptr )
        {
            mesh = LoadMesh( _path );
        }
        return mesh;
    }

    //========================================================================================================
    //========================================================================================================
    Mesh* MeshManager::LoadMesh( const std::string& _path )
    {
        Mesh* mesh = GetMesh( _path );
        if( mesh != nullptr ) { return mesh; }

        // Load
        const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();
        mesh = new Mesh();
        if( mesh->LoadFromFile( cleanPath ) )
        {
            mesh->mIndex = (int)mMeshes.size();
            mMeshes.push_back( { mesh } );
            return mesh;
        }
        delete mesh;
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::AddMesh( Mesh * mesh, const std::string& _name )
    {
        assert( mesh != nullptr );
        mesh->mIndex = (int)mMeshes.size();
        mesh->mPath = _name;
        mesh->mExternallyOwned = true;
        mMeshes.push_back( { mesh } );
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::Remove( const std::string& _path )
    {
        const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();
        for( int          meshIndex = 0; meshIndex < mMeshes.size(); meshIndex++ )
        {
            if( mMeshes[meshIndex]->mPath == cleanPath )
            {
                (*mMeshes.rbegin())->mIndex = meshIndex;

                for( int i = 0; i < SwapChain::s_maxFramesInFlight; i++ )
                {
                    mDestroyList.push_back( mMeshes[meshIndex]->mIndexBuffer[i] );
                    mDestroyList.push_back( mMeshes[meshIndex]->mVertexBuffer[i] );
                }
                if( ! mMeshes[meshIndex]->mExternallyOwned )
                {
                    delete mMeshes[meshIndex];
                }

                mMeshes[meshIndex] = mMeshes[mMeshes.size() - 1];
                mMeshes.pop_back();
                return;
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::Clear( Device& _device )
    {
        for ( Mesh* mesh : mMeshes )
        {
            for( int i = 0 ; i < SwapChain::s_maxFramesInFlight; i++)
            {
                mesh->mIndexBuffer[i].Destroy( _device );
                mesh->mVertexBuffer[i].Destroy( _device );
            }
            if( ! mesh->mExternallyOwned )
            {
                delete mesh;
            }
        }
        mMeshes.clear();
        DestroyBuffers( _device );
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::GenerateBuffers( Device& _device )
    {
        for( Mesh * mesh : mMeshes )
        {
            if( mesh->mBuffersOutdated )
            {
                mesh->GenerateBuffers( _device );
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::DestroyBuffers( Device& _device )
    {
        for( Buffer& buffer : mDestroyList )
        {
            buffer.Destroy( _device );
        }
        mDestroyList.clear();
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::ResolvePtr( ResourcePtr<Mesh >& _resourcePtr )
    {
        assert( ! _resourcePtr.IsValid() );

        MeshPtr& meshPtr = static_cast< MeshPtr& >( _resourcePtr );
        Mesh    * mesh    = GetOrLoad( meshPtr.GetPath() );
        if ( mesh != nullptr )
        {
            meshPtr = mesh;
        }
    }
}