#include "fanMeshManager.hpp"

#include <filesystem>
#include "fanMesh.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/core/fanDevice.hpp"
#include "core/fanDebug.hpp"


namespace fan
{
    //========================================================================================================
    //========================================================================================================
    Mesh* MeshManager::Get( const std::string& _path ) const
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
        Mesh * mesh = Get( _path );
        if( mesh == nullptr )
        {
            mesh = Load( _path );
        }
        return mesh;
    }

    //========================================================================================================
    //========================================================================================================
    Mesh* MeshManager::Load( const std::string& _path )
    {
        Mesh* mesh = Get( _path );
        if( mesh != nullptr ) { return mesh; }

        // Load
        const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();
        mesh = new Mesh();
        if( mesh->LoadFromFile( cleanPath ) )
        {
            mesh->mIndex = (int)mMeshes.size();
            mMeshes.push_back( mesh );
            return mesh;
        }
        delete mesh;
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::Add( Mesh * _mesh, const std::string& _name )
    {
        assert( _mesh != nullptr );
        _mesh->mIndex = (int)mMeshes.size();
        _mesh->mPath = _name;
        _mesh->mExternallyOwned = true;
        mMeshes.push_back( _mesh );
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
                mDestroyList.push_back( mMeshes[meshIndex] );
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
    void MeshManager::Create( Device& _device )
    {
        for( Mesh * mesh : mMeshes )
        {
            if( mesh->mBuffersOutdated )
            {
                mesh->Create( _device );
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::Destroy( Device& _device )
    {
        for( Mesh* mesh : mDestroyList )
        {
            mesh->Destroy( _device );
            if( ! mesh->mExternallyOwned )
            {
                Debug::Log( mesh->mPath );
                delete mesh;
            }
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