#pragma once

#include "core/ressources/fanRessource.h"
#include "renderer/util/fanVertex.h"

namespace fan {
	class ConvexHull;
	class Buffer;
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class Mesh {
	public:
		static Signal< Mesh* > s_onMeshLoad;
		static Signal<> s_onMeshDelete;

		Mesh(const std::string& _path = "");
		~Mesh();

		void		SetPath(const std::string& _path)			{ m_path = _path; }
		std::string GetPath() const								{ return m_path;  }
	

		std::vector<Vertex> &			GetVertices()			{ return m_vertices;	}
		const std::vector<Vertex> &		GetVertices() const		{ return m_vertices;	}
		std::vector<uint32_t> &			GetIndices()			{ return m_indices;		}
		const std::vector<uint32_t> &	GetIndices() const		{ return m_indices;		}
		void 							GenerateConvexHull( ConvexHull & _outConvexHull );
		Buffer *						GetIndexBuffer()		{ return m_indexBuffer[m_currentBuffer];	}
		Buffer *						GetVertexBuffer()		{ return m_vertexBuffer[m_currentBuffer];	}
		
		void GenerateBuffers( Device & _device );
		bool Load();
		bool LoadFromVertices( const std::vector<Vertex>&	_vertices );
		DECLARE_TYPE_INFO(Mesh, Ressource )
	private:
		std::string				m_path;
		std::vector<Vertex>		m_vertices;
		std::vector<uint32_t>	m_indices;

		uint32_t m_currentBuffer = 0;
		Buffer * m_indexBuffer[3];
		Buffer * m_vertexBuffer[3];

		void OptimizeVertices();
		
	};
}