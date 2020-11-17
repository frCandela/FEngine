#pragma once

#include <vector>
#include <string>
#include "render/core/fanBuffer.hpp"

namespace fan
{
	struct Mesh2D;

	//========================================================================================================
	// Owns all the ui meshes of the engine
	//========================================================================================================
	class Mesh2DManager
	{
	public:
        Mesh2DManager(){}
        Mesh2DManager( Mesh2DManager const& ) = delete;
        Mesh2DManager& operator=( Mesh2DManager const& ) = delete;

		void Clear( Device& _device );
        Mesh2D* Get( const std::string& _path ) const;
        void Add( Mesh2D* _mesh, const std::string& _name );
        void Remove( const std::string& _path );
        bool Empty() const { return mMeshes.empty(); }
		int  MeshCount() const { return (int)mMeshes.size(); }

        void CreateNewMeshes( Device& _device );
        void DestroyRemovedMeshes( Device& _device );

        int DestroyListSize() const  { return (int)mDestroyList.size(); }
        const std::vector< Mesh2D * >& GetMeshes() const { return mMeshes; }

	private:
		std::vector< Mesh2D* > mMeshes;
        std::vector< Mesh2D* > mDestroyList;
	};
}