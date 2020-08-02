#include "fanMesh2DManager.hpp"

#include "fanMesh2D.hpp"

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
                mDestroyList.push_back( mMeshes[meshIndex] );
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
            mesh->Destroy( _device );
            if( ! mesh->mExternallyOwned )
            {
                delete mesh;
            }
        }
        mMeshes.clear();
        Destroy( _device );
    }

    //========================================================================================================
    //========================================================================================================
    void Mesh2DManager::Create( Device& _device )
    {
        for( Mesh2D * mesh : mMeshes )
        {
            if( mesh->mBuffersOutdated )
            {
                mesh->Create( _device );
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    void Mesh2DManager::Destroy( Device& _device )
    {
        for( Mesh2D* mesh : mDestroyList )
        {
            mesh->Destroy( _device );
            if( !mesh->mExternallyOwned )
            {
                delete mesh;
            }
        }
        mDestroyList.clear();
    }
}