#include "render/fanMesh2DManager.hpp"

#include "render/fanMesh2D.hpp"

namespace fan
{
    //================================================================================================================================
    // _externalOwner set to true means the Mesh2DManager will destroy the mesh
    //================================================================================================================================
    void Mesh2DManager::Add( Mesh2D* _mesh, const std::string& _name, const bool _externalOwner )
    {
        assert( _mesh != nullptr );
        _mesh->mIndex = (int)mMeshes.size();
        _mesh->mPath = _name;
        _mesh->mExternallyOwned = _externalOwner;
        mMeshes.push_back( { _mesh } );
    }

    //========================================================================================================
    //========================================================================================================
    void Mesh2DManager::Remove( const std::string& _path )
    {
        for( int          meshIndex = 0; meshIndex < mMeshes.size(); meshIndex++ )
        {
            if( mMeshes[meshIndex]->mPath == _path )
            {
                (*mMeshes.rbegin())->mIndex = meshIndex;

                for( int i = 0; i < SwapChain::s_maxFramesInFlight; i++ )
                {
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
    Mesh2D* Mesh2DManager::Get( const std::string& _path ) const
    {
        for( Mesh2D* mesh : mMeshes )
        {
            if( mesh->mPath == _path )
            {
                return mesh;
            }
        }
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    void Mesh2DManager::Clear( Device& _device )
    {
        for ( Mesh2D* mesh : mMeshes )
        {
            mesh->DestroyBuffers( _device );
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
    void Mesh2DManager::CreateBuffers( Device& _device )
    {
        for( Mesh2D * mesh : mMeshes )
        {
            if( mesh->mBuffersOutdated )
            {
                mesh->CreateBuffers( _device );
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    void Mesh2DManager::DestroyBuffers( Device& _device )
    {
        for( Buffer& buffer : mDestroyList )
        {
            buffer.Destroy( _device );
        }
        mDestroyList.clear();
    }
}