#include "fanMeshManager.hpp"

#include "core/fanPath.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    Mesh* MeshManager::Get( const std::string& _path ) const
    {
        for( Mesh* mesh : mMeshes )
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
        mesh = new Mesh();
        if( mesh->LoadFromFile( _path ) )
        {
            mesh->mIndex = (int)mMeshes.size();
            mMeshes.push_back( mesh );
            return mesh;
        }
        delete mesh;
        return nullptr;
    }

    //========================================================================================================
    // Every mesh added using this method will be deleted by the MeshManager
    //========================================================================================================
    void MeshManager::Add( Mesh * _mesh, const std::string& _name )
    {
       fanAssert( _mesh != nullptr );
        _mesh->mIndex = (int)mMeshes.size();
        _mesh->mPath = _name;
        mMeshes.push_back( _mesh );
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::Remove( const std::string& _path )
    {
        for( int          meshIndex = 0; meshIndex < (int)mMeshes.size(); meshIndex++ )
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
    void MeshManager::Clear( Device& _device )
    {
        for ( Mesh* mesh : mMeshes )
        {
            mesh->Destroy( _device );
            delete mesh;
        }
        mMeshes.clear();
        DestroyRemovedMeshes( _device );
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::CreateNewMeshes( Device& _device )
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
    void MeshManager::DestroyRemovedMeshes( Device& _device )
    {
        for( Mesh* mesh : mDestroyList )
        {
            mesh->Destroy( _device );
            delete mesh;
        }
        mDestroyList.clear();
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager::ResolvePtr( ResourcePtr<Mesh>& _resourcePtr )
    {
        fanAssert( !_resourcePtr.IsValid() );

        MeshPtr& meshPtr = static_cast< MeshPtr& >( _resourcePtr );
        const std::string& path = meshPtr.GetPath();
        if( !path.empty() && !Path::Extension( path ).empty() )
        {
            Mesh* mesh = GetOrLoad( meshPtr.GetPath() );
            if( mesh != nullptr )
            {
                meshPtr = mesh;
            }
        }
    }
}