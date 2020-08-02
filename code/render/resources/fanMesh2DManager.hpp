#pragma once

#include <cassert>
#include <vector>
#include <string>
#include "render/core/fanBuffer.hpp"

namespace fan
{
	class Mesh2D;
	struct Device;

	//================================================================================================================================
	// Owns all the ui meshes of the engine
	//================================================================================================================================
	class Mesh2DManager
	{
	public:
		void Clear( Device& _device );
        Mesh2D* Get( const std::string& _path ) const;
        void Add( Mesh2D* _mesh, const std::string& _name, const bool _externalOwner = false );
        void Remove( const std::string& _path );
        bool Empty() const { return mMeshes.empty(); }
		int  MeshCount() const { return (int)mMeshes.size(); }

        void Create( Device& _device );
        void Destroy( Device& _device );

        int DestroyListSize() const  { return (int)mDestroyList.size(); }
        const std::vector< Mesh2D * >& GetMeshes() const { return mMeshes; }
	private:
		std::vector< Mesh2D* > mMeshes;
        std::vector< Mesh2D* > mDestroyList;
	};
}