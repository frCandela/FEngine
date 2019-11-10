#pragma once

#include "core/ressources/fanRessource.h"
#include "renderer/util/fanVertex.h"
#include "core/math/shapes/fanConvexHull.h"

namespace fan
{
	class Buffer;
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class UIMesh
	{
	public:
		static Signal< UIMesh* > s_onMeshLoad;
		static Signal<> s_onMeshDelete;

		UIMesh();
		~UIMesh();

		//bool RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const;

		std::vector<UIVertex> &			GetVertices() { return m_vertices; }
		const std::vector<UIVertex> &	GetVertices() const { return m_vertices; }
		Buffer *						GetVertexBuffer() { return m_vertexBuffer[m_currentBuffer]; }

		// Useful when the mesh changes very often
		void SetHostVisible( const bool _hostVisible ){ m_hostVisible = _hostVisible; } 
		void GenerateBuffers( Device & _device );
		
		bool LoadFromVertices( const std::vector<UIVertex>&	_vertices );
		DECLARE_TYPE_INFO( UIMesh, void )
	private:
		std::vector<UIVertex>		m_vertices;

		bool m_hostVisible = false;

		uint32_t m_currentBuffer = 0;
		Buffer * m_vertexBuffer[3];


	};
}