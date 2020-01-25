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
	class UIMesh : public Ressource
	{
	public:
		static Signal< UIMesh* > s_onGenerateVulkanData;
		static Signal< UIMesh* > s_onDeleteVulkanData;

		UIMesh();
		~UIMesh();

		std::vector<UIVertex> &			GetVertices() { return m_vertices; }
		const std::vector<UIVertex> &	GetVertices() const { return m_vertices; }
		Buffer *						GetVertexBuffer() { return m_vertexBuffer[m_currentBuffer]; }

		void SetHostVisible( const bool _hostVisible ){ m_hostVisible = _hostVisible; } // Useful when the mesh changes very often
		
		bool LoadFromVertices( const std::vector<UIVertex>&	_vertices );
		bool LoadFromFile( const std::string& _path ) override;
		void GenerateVulkanData( Device & _device );
		void DeleteVulkanData( Device & _device );

		DECLARE_TYPE_INFO( UIMesh, void )
	private:
		std::vector<UIVertex>		m_vertices;

		bool m_hostVisible = false;

		uint32_t m_currentBuffer = 0;
		Buffer * m_vertexBuffer[3];
	};
}