#pragma once

#include "core/ressources/fanRessource.h"
#include "renderer/util/fanVertex.h"

namespace fan {
	class ConvexHull;
	class Buffer;
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class Mesh : public Ressource {
	public:
		Mesh(const std::string& _path = "");
		~Mesh();

		void		SetPath(const std::string& _path)			{ m_path = _path; }
		std::string GetPath() const								{ return m_path;  }

		std::vector<Vertex> &			GetVertices()			{ return m_vertices;	}
		const std::vector<Vertex> &		GetVertices() const		{ return m_vertices;	}
		std::vector<uint32_t> &			GetIndices()			{ return m_indices;		}
		const std::vector<uint32_t> &	GetIndices() const		{ return m_indices;		}
		void 							GenerateConvexHull( ConvexHull & _outConvexHull );
		Buffer *						GetIndexBuffer()		{ return m_indexBuffer;	}
		Buffer *						GetVertexBuffer()		{ return m_vertexBuffer;	}
		
		void GenerateBuffers( Device & _device );
		bool Load() override;

		DECLARE_TYPE_INFO(Mesh, Ressource )
	private:
		std::string				m_path;
		std::vector<Vertex>		m_vertices;
		std::vector<uint32_t>	m_indices;

		Buffer * m_indexBuffer;
		Buffer * m_vertexBuffer;

		void OptimizeVertices();
		
	};
}