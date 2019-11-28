#include "fanGlobalIncludes.h"
#include "renderer/fanUIMesh.h"

#include "renderer/core/fanDevice.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	REGISTER_TYPE_INFO( UIMesh, TypeInfo::Flags::NONE, "ui/" )

	Signal< UIMesh* > UIMesh::s_onGenerateVulkanData;
	Signal< UIMesh* > UIMesh::s_onDeleteVulkanData;

	//================================================================================================================================
	//================================================================================================================================
	UIMesh::UIMesh(  ) :
		m_vertices( 0 )
		, m_vertexBuffer { nullptr ,nullptr ,nullptr }
	{}

	//================================================================================================================================
	//================================================================================================================================
	UIMesh::~UIMesh(){ s_onDeleteVulkanData.Emmit( this ); }

	//================================================================================================================================
	//================================================================================================================================
	bool UIMesh::LoadFromVertices( const std::vector<UIVertex>&	_vertices )
	{
		m_vertices = _vertices;

		// Cleanup
		s_onGenerateVulkanData.Emmit( this );
 		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool UIMesh::LoadFromFile( const std::string& _path ) 
	{
		assert( false );
		return Ressource::LoadFromFile(_path);
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMesh::GenerateVulkanData( Device & _device )
	{
		if ( m_vertices.empty() ) { return; }

		m_currentBuffer = ( m_currentBuffer + 1 ) % 3;

		const VkMemoryPropertyFlags memPropertyFlags = ( m_hostVisible ?
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		Buffer *& vertexBuffer = m_vertexBuffer[m_currentBuffer];
		const VkDeviceSize requiredVertexSize = sizeof( m_vertices[0] ) * m_vertices.size();
		if ( vertexBuffer == nullptr || vertexBuffer->GetSize() < requiredVertexSize )
		{
			delete vertexBuffer;
			vertexBuffer = new Buffer( _device );
			m_vertexBuffer[m_currentBuffer]->Create(
				requiredVertexSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				memPropertyFlags
			);
		}

		if ( m_hostVisible )
		{
			vertexBuffer->SetData( m_vertices.data(), requiredVertexSize );
		}
		else
		{ 
			Buffer stagingBuffer2( _device );
			stagingBuffer2.Create(
				requiredVertexSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			stagingBuffer2.SetData( m_vertices.data(), requiredVertexSize );
			VkCommandBuffer cmd2 = _device.BeginSingleTimeCommands();
			stagingBuffer2.CopyBufferTo( cmd2, vertexBuffer->GetBuffer(), requiredVertexSize );
			_device.EndSingleTimeCommands( cmd2 );			
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMesh::DeleteVulkanData( Device & /*_device*/ )
	{
		for ( int bufferIndex = 0; bufferIndex < 3; bufferIndex++ )
		{
			delete m_vertexBuffer[bufferIndex];
		}
	}
}