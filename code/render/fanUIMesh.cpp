#include "render/fanUIMesh.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanBuffer.hpp"

namespace fan
{
	UIMeshManager UIMesh::s_resourceManager;

	//================================================================================================================================
	//================================================================================================================================
	UIMesh::UIMesh() :
		m_vertices( 0 )
	{}

	//================================================================================================================================
	//================================================================================================================================
	UIMesh::~UIMesh() 
	{ 
		DeleteGpuData( s_resourceManager.GetDevice() );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool UIMesh::LoadFromVertices( const std::vector<UIVertex>& _vertices )
	{
		m_vertices = _vertices;
		GenerateGpuData( s_resourceManager.GetDevice() );
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool UIMesh::LoadFromFile( const std::string& _path )
	{
		assert( false );
		m_path = _path;
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMesh::GenerateGpuData( Device& _device )
	{
		if ( m_vertices.empty() ) { return; }

		m_currentBuffer = ( m_currentBuffer + 1 ) % 3;

		const VkMemoryPropertyFlags memPropertyFlags = ( m_hostVisible ?
														 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :
														 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		Buffer& vertexBuffer = m_vertexBuffer[ m_currentBuffer ];
		const VkDeviceSize requiredVertexSize = sizeof( m_vertices[ 0 ] ) * m_vertices.size();
		if ( vertexBuffer.mBuffer == VK_NULL_HANDLE || vertexBuffer.mSize < requiredVertexSize )
		{
			vertexBuffer.Destroy( _device );
			m_vertexBuffer[ m_currentBuffer ].Create(
				_device,
				requiredVertexSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				memPropertyFlags
			);
		}

		if ( m_hostVisible )
		{
			vertexBuffer.SetData( _device, m_vertices.data(), requiredVertexSize );
		}
		else
		{
			Buffer stagingBuffer2;
			stagingBuffer2.Create(
				_device,
				requiredVertexSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			stagingBuffer2.SetData( _device, m_vertices.data(), requiredVertexSize );
			VkCommandBuffer cmd2 = _device.BeginSingleTimeCommands();
			stagingBuffer2.CopyBufferTo( cmd2, vertexBuffer.mBuffer, requiredVertexSize );
			_device.EndSingleTimeCommands( cmd2 );
			stagingBuffer2.Destroy( _device );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMesh::DeleteGpuData( Device& _device )
	{
		vkDeviceWaitIdle( _device.mDevice );
		Debug::Highlight( "Renderer idle4" );

		for ( int bufferIndex = 0; bufferIndex < 3; bufferIndex++ )
		{
			m_vertexBuffer[bufferIndex].Destroy( _device );
		}
	}
}