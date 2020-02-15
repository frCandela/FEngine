#pragma once

#include "render/fanRenderPrecompiled.hpp"

#include "render/util/fanVertex.hpp"
#include "render/fanUIMeshManager.hpp"
#include "core/resources/fanResource.hpp"
#include "core/math/shapes/fanConvexHull.hpp"


namespace fan
{
	class Buffer;
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class UIMesh : public Resource
	{
	public:
		static UIMeshManager s_resourceManager;

		UIMesh();
		~UIMesh();

		std::vector<UIVertex> &			GetVertices()		{ return m_vertices; }
		const std::vector<UIVertex> &	GetVertices() const { return m_vertices; }
		Buffer *						GetVertexBuffer()	{ return m_vertexBuffer[m_currentBuffer]; }
		std::string						GetPath() const		{ return m_path; }

		void SetHostVisible( const bool _hostVisible ){ m_hostVisible = _hostVisible; } // Useful when the mesh changes very often
		
		bool LoadFromVertices( const std::vector<UIVertex>&	_vertices );
		bool LoadFromFile( const std::string& _path );
		void GenerateGpuData( Device & _device );
		void DeleteGpuData( Device & _device );
	private:
		std::vector<UIVertex>		m_vertices;
		std::string		m_path;

		bool m_hostVisible = false;

		uint32_t m_currentBuffer = 0;
		Buffer * m_vertexBuffer[3];
	};
}