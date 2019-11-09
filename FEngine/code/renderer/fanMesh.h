#pragma once

#include "core/ressources/fanRessource.h"
#include "renderer/util/fanVertex.h"
#include "core/math/shapes/fanConvexHull.h"

namespace fan {
	class Buffer;
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class Mesh {
	public:
		static Signal< Mesh* > s_onMeshLoad;
		static Signal<> s_onMeshDelete;

		Mesh(const std::string& _path = "" );
		~Mesh();

		void		SetPath(const std::string& _path)			{ m_path = _path; }
		std::string GetPath() const								{ return m_path;  }
	
		bool RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const;

		std::vector<Vertex> &			GetVertices()			{ return m_vertices;	}
		const std::vector<Vertex> &		GetVertices() const		{ return m_vertices;	}
		std::vector<uint32_t> &			GetIndices()			{ return m_indices;		}
		const std::vector<uint32_t> &	GetIndices() const		{ return m_indices;		}
		Buffer *						GetIndexBuffer()		{ return m_indexBuffer[m_currentBuffer];	}
		Buffer *						GetVertexBuffer()		{ return m_vertexBuffer[m_currentBuffer];	}
		const ConvexHull&				GetHull()				{ return m_convexHull; }

		// Usefull when the mesh changes very often
		void SetHostVisible( const bool _hostVisible ){ m_hostVisible = _hostVisible; } 
		void SetOptimizeVertices( const bool _optimizeVertices ) { m_optimizeVertices = _optimizeVertices; }
		void SetAutoUpdateHull( const bool _autoUpdateHull ) { m_autoUpdateHull = _autoUpdateHull; }

		void GenerateBuffers( Device & _device );
		bool Load();
		bool LoadFromVertices( const std::vector<Vertex>&	_vertices );
		DECLARE_TYPE_INFO(Mesh, void )
	private:
		std::string				m_path;
		std::vector<Vertex>		m_vertices;
		std::vector<uint32_t>	m_indices;

		bool m_hostVisible = false;
		bool m_optimizeVertices = true;		
		bool m_autoUpdateHull = true;

		uint32_t m_currentBuffer = 0;
		Buffer * m_indexBuffer[3];
		Buffer * m_vertexBuffer[3];
		ConvexHull m_convexHull;

		void OptimizeVertices();
		void GenerateConvexHull();
	};
}