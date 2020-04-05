#include "render/fanRendererDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanSwapChain.hpp"
#include "render/pipelines/fanDebugPipeline.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "core/math/fanBasicModels.hpp"
#include "core/time/fanProfiler.hpp"

namespace fan
{
	RendererDebug* RendererDebug::s_rendererInstance = nullptr; // Used for global debug draw

	//================================================================================================================================
	//================================================================================================================================
	RendererDebug::RendererDebug( Device& _device, SwapChain& _swapchain )
		: m_device( _device )
		, m_swapchain( _swapchain )
	{
		m_debugLinesvertexBuffers.resize( m_swapchain.GetSwapchainImagesCount() );
		m_debugLinesNoDepthTestVertexBuffers.resize( m_swapchain.GetSwapchainImagesCount() );
		m_debugTrianglesvertexBuffers.resize( m_swapchain.GetSwapchainImagesCount() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::Create( VkRenderPass _renderPass, FrameBuffer* _frameBuffer )
	{
		m_renderPass = _renderPass;
		m_frameBuffer = _frameBuffer;


		m_debugLinesPipeline = new DebugPipeline( m_device, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, true );
		m_debugLinesPipeline->Init( _renderPass, m_swapchain.GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag" );
		m_debugLinesPipeline->CreateDescriptors( m_swapchain.GetSwapchainImagesCount() );
		m_debugLinesPipeline->Create();

		m_debugLinesPipelineNoDepthTest = new DebugPipeline( m_device, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false );
		m_debugLinesPipelineNoDepthTest->Init( _renderPass, m_swapchain.GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag" );
		m_debugLinesPipelineNoDepthTest->CreateDescriptors( m_swapchain.GetSwapchainImagesCount() );
		m_debugLinesPipelineNoDepthTest->Create();

		m_debugTrianglesPipeline = new DebugPipeline( m_device, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false );
		m_debugTrianglesPipeline->Init( _renderPass, m_swapchain.GetExtent(), "code/shaders/debugTriangles.vert", "code/shaders/debugTriangles.frag" );
		m_debugTrianglesPipeline->CreateDescriptors( m_swapchain.GetSwapchainImagesCount() );
		m_debugTrianglesPipeline->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	RendererDebug::~RendererDebug()
	{
		delete m_debugLinesPipeline;
		delete m_debugLinesPipelineNoDepthTest;
		delete m_debugTrianglesPipeline;

		for ( int bufferIndex = 0; bufferIndex < m_debugLinesvertexBuffers.size(); bufferIndex++ )
		{
			delete m_debugLinesvertexBuffers[ bufferIndex ];
		} m_debugLinesvertexBuffers.clear();

		for ( int bufferIndex = 0; bufferIndex < m_debugLinesNoDepthTestVertexBuffers.size(); bufferIndex++ )
		{
			delete m_debugLinesNoDepthTestVertexBuffers[ bufferIndex ];
		} m_debugLinesNoDepthTestVertexBuffers.clear();

		for ( int bufferIndex = 0; bufferIndex < m_debugTrianglesvertexBuffers.size(); bufferIndex++ )
		{
			delete m_debugTrianglesvertexBuffers[ bufferIndex ];
		} m_debugTrianglesvertexBuffers.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::Resize( const VkExtent2D _extent )
	{
		m_debugLinesPipeline->Resize( _extent );
		m_debugLinesPipelineNoDepthTest->Resize( _extent );
		m_debugTrianglesPipeline->Resize( _extent );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::UpdateUniformBuffers( const size_t _index )
	{
		m_debugLinesPipeline->UpdateUniformBuffers( _index );
		m_debugLinesPipelineNoDepthTest->UpdateUniformBuffers( _index );
		m_debugTrianglesPipeline->UpdateUniformBuffers( _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view )
	{
		std::array< DebugPipeline*, 3 > debugLinesPipelines = { m_debugLinesPipeline, m_debugLinesPipelineNoDepthTest, m_debugTrianglesPipeline };
		for ( int pipelingIndex = 0; pipelingIndex < debugLinesPipelines.size(); pipelingIndex++ )
		{
			debugLinesPipelines[ pipelingIndex ]->m_debugUniforms.model = glm::mat4( 1.0 );
			debugLinesPipelines[ pipelingIndex ]->m_debugUniforms.view = _view;
			debugLinesPipelines[ pipelingIndex ]->m_debugUniforms.proj = _projection;
			debugLinesPipelines[ pipelingIndex ]->m_debugUniforms.color = glm::vec4( 1, 1, 1, 1 );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::UpdateDebugBuffer( const size_t _index )
	{
		SCOPED_PROFILE( update_buffer )
			if ( m_debugLines.size() > 0 )
			{
				SCOPED_PROFILE( lines )
					delete m_debugLinesvertexBuffers[ _index ];	// TODO update instead of delete
				const VkDeviceSize size = sizeof( DebugVertex ) * m_debugLines.size();
				m_debugLinesvertexBuffers[ _index ] = new Buffer( m_device );
				m_debugLinesvertexBuffers[ _index ]->Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);

				if ( size > 0 )
				{
					Buffer stagingBuffer( m_device );
					stagingBuffer.Create(
						size,
						VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
					);
					stagingBuffer.SetData( m_debugLines.data(), size );
					VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
					stagingBuffer.CopyBufferTo( cmd, m_debugLinesvertexBuffers[ _index ]->GetBuffer(), size );
					m_device.EndSingleTimeCommands( cmd );
				}
			}

		if ( m_debugLinesNoDepthTest.size() > 0 )
		{
			SCOPED_PROFILE( lines_no_depth )
				delete m_debugLinesNoDepthTestVertexBuffers[ _index ];
			const VkDeviceSize size = sizeof( DebugVertex ) * m_debugLinesNoDepthTest.size();
			m_debugLinesNoDepthTestVertexBuffers[ _index ] = new Buffer( m_device );
			m_debugLinesNoDepthTestVertexBuffers[ _index ]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if ( size > 0 )
			{
				Buffer stagingBuffer( m_device );
				stagingBuffer.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( m_debugLinesNoDepthTest.data(), size );
				VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugLinesNoDepthTestVertexBuffers[ _index ]->GetBuffer(), size );
				m_device.EndSingleTimeCommands( cmd );
			}
		}

		if ( m_debugTriangles.size() > 0 )
		{
			SCOPED_PROFILE( triangles )
				delete m_debugTrianglesvertexBuffers[ _index ];
			const VkDeviceSize size = sizeof( DebugVertex ) * m_debugTriangles.size();
			m_debugTrianglesvertexBuffers[ _index ] = new Buffer( m_device );
			m_debugTrianglesvertexBuffers[ _index ]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if ( size > 0 )
			{
				Buffer stagingBuffer( m_device );
				stagingBuffer.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( m_debugTriangles.data(), size );
				VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugTrianglesvertexBuffers[ _index ]->GetBuffer(), size );
				m_device.EndSingleTimeCommands( cmd );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::ClearDebug()
	{
		m_debugLines.clear();
		m_debugLinesNoDepthTest.clear();
		m_debugTriangles.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::ReloadShaders()
	{
		m_debugLinesPipeline->ReloadShaders();
		m_debugLinesPipelineNoDepthTest->ReloadShaders();
		m_debugTrianglesPipeline->ReloadShaders();
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::RecordCommandBufferDebug( const size_t _index )
	{
		SCOPED_PROFILE( debug )
			if ( HasNoDebugToDraw() == false )
			{
				UpdateDebugBuffer( _index );

				VkCommandBuffer commandBuffer = m_debugCommandBuffers[ _index ];

				VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
				commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
				commandBufferInheritanceInfo.pNext = nullptr;
				commandBufferInheritanceInfo.renderPass = m_renderPass;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = m_frameBuffer->GetFrameBuffer( _index );
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags				=;
				//commandBufferInheritanceInfo.pipelineStatistics		=;

				VkCommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				commandBufferBeginInfo.pNext = nullptr;
				commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
				{
					VkDeviceSize offsets[] = { 0 };
					if ( m_debugLines.size() > 0 )
					{
						m_debugLinesPipeline->Bind( commandBuffer, _index );
						VkBuffer vertexBuffers[] = { m_debugLinesvertexBuffers[ _index ]->GetBuffer() };
						vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
						vkCmdDraw( commandBuffer, static_cast< uint32_t >( m_debugLines.size() ), 1, 0, 0 );
					}
					if ( m_debugLinesNoDepthTest.size() > 0 )
					{
						m_debugLinesPipelineNoDepthTest->Bind( commandBuffer, _index );
						VkBuffer vertexBuffers[] = { m_debugLinesNoDepthTestVertexBuffers[ _index ]->GetBuffer() };
						vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
						vkCmdDraw( commandBuffer, static_cast< uint32_t >( m_debugLinesNoDepthTest.size() ), 1, 0, 0 );
					}
					if ( m_debugTriangles.size() > 0 )
					{
						m_debugTrianglesPipeline->Bind( commandBuffer, _index );
						VkBuffer vertexBuffers[] = { m_debugTrianglesvertexBuffers[ _index ]->GetBuffer() };
						vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
						vkCmdDraw( commandBuffer, static_cast< uint32_t >( m_debugTriangles.size() ), 1, 0, 0 );
					}
					if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
					{
						Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
					}
				}
				else
				{
					Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
				}
			}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::DebugPoint( const btVector3 _pos, const Color _color )
	{
		const float size = 0.2f;
		DebugLine( _pos - size * btVector3::Up(), _pos + size * btVector3::Up(), _color );
		DebugLine( _pos - size * btVector3::Left(), _pos + size * btVector3::Left(), _color );
		DebugLine( _pos - size * btVector3::Forward(), _pos + size * btVector3::Forward(), _color );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::DebugLine( const btVector3 _start, const btVector3 _end, const Color _color, const bool _depthTestEnable )
	{
		if ( _depthTestEnable )
		{
			m_debugLines.push_back( DebugVertex( ToGLM( _start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
			m_debugLines.push_back( DebugVertex( ToGLM( _end ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
		}
		else
		{
			m_debugLinesNoDepthTest.push_back( DebugVertex( ToGLM( _start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
			m_debugLinesNoDepthTest.push_back( DebugVertex( ToGLM( _end ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
		}

	}

	//================================================================================================================================
	// takes a list of triangle and a list of colors
	// 3 vertices per triangle
	// 1 color per triangle
	//================================================================================================================================
	void RendererDebug::DebugTriangles( const std::vector<btVector3>& _triangles, const std::vector<Color>& _colors )
	{
		assert( _triangles.size() % 3 == 0 );
		assert( _colors.size() == _triangles.size() / 3 );

		m_debugTriangles.resize( m_debugTriangles.size() + _triangles.size() );
		for ( int triangleIndex = 0; triangleIndex < _triangles.size() / 3; triangleIndex++ )
		{
			const btVector3 v0 = _triangles[ 3 * triangleIndex + 0 ];
			const btVector3 v1 = _triangles[ 3 * triangleIndex + 1 ];
			const btVector3 v2 = _triangles[ 3 * triangleIndex + 2 ];
			const glm::vec3 normal = glm::normalize( ToGLM( ( v1 - v2 ).cross( v0 - v2 ) ) );

			m_debugTriangles[ 3 * triangleIndex + 0 ] = DebugVertex( ToGLM( v0 ), normal, _colors[ triangleIndex ].ToGLM() );
			m_debugTriangles[ 3 * triangleIndex + 1 ] = DebugVertex( ToGLM( v1 ), normal, _colors[ triangleIndex ].ToGLM() );
			m_debugTriangles[ 3 * triangleIndex + 2 ] = DebugVertex( ToGLM( v2 ), normal, _colors[ triangleIndex ].ToGLM() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::DebugCircle( const btVector3 _pos, const float _radius, btVector3 _axis, uint32_t _nbSegments, const Color _color, const bool _depthTestEnable )
	{
		assert( _nbSegments > 2 && _radius >= 0.f );

		const btVector3 other = btVector3( -_axis[ 1 ], -_axis[ 2 ], _axis[ 0 ] );
		btVector3 orthogonal = _radius * _axis.cross( other ).normalized();
		const float angle = 2.f * PI / ( float ) _nbSegments;

		std::vector<DebugVertex>& lines = _depthTestEnable ? m_debugLines : m_debugLinesNoDepthTest;
		for ( uint32_t segmentIndex = 0; segmentIndex < _nbSegments; segmentIndex++ )
		{

			btVector3 start = _pos + orthogonal;
			orthogonal = orthogonal.rotate( _axis, angle );
			btVector3 end = _pos + orthogonal;

			lines.push_back( DebugVertex( ToGLM( start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
			lines.push_back( DebugVertex( ToGLM( end ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::DebugTriangle( const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color )
	{
		const glm::vec3 normal = glm::normalize( ToGLM( ( _v1 - _v2 ).cross( _v0 - _v2 ) ) );
		m_debugTriangles.push_back( DebugVertex( ToGLM( _v0 ), normal, _color.ToGLM() ) );
		m_debugTriangles.push_back( DebugVertex( ToGLM( _v1 ), normal, _color.ToGLM() ) );
		m_debugTriangles.push_back( DebugVertex( ToGLM( _v2 ), normal, _color.ToGLM() ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::DebugCube( const btTransform _transform, const btVector3 _halfExtent, const Color _color, const bool _depthTestEnable )
	{
		std::vector< btVector3 > cube = GetCube( _halfExtent );

		for ( int vertIndex = 0; vertIndex < cube.size(); vertIndex++ )
		{
			cube[ vertIndex ] = _transform * cube[ vertIndex ];
		}

		glm::vec4 glmColor = _color.ToGLM();

		std::vector<DebugVertex>& lines = _depthTestEnable ? m_debugLines : m_debugLinesNoDepthTest;

		lines.push_back( DebugVertex( ToGLM( cube[0] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[1] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[1] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[3] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[3] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[2] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[2] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[0] ), glm::vec3( 0, 0, 0 ), glmColor ) );

		lines.push_back( DebugVertex( ToGLM( cube[4] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[5] ), glm::vec3( 0, 0, 0 ), glmColor ) );
 		lines.push_back( DebugVertex( ToGLM( cube[5] ), glm::vec3( 0, 0, 0 ), glmColor ) );
 		lines.push_back( DebugVertex( ToGLM( cube[7] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[7] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[6] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[6] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[4] ), glm::vec3( 0, 0, 0 ), glmColor ) );

		lines.push_back( DebugVertex( ToGLM( cube[0] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[4] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[1] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[5] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[3] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[7] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[2] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[6] ), glm::vec3( 0, 0, 0 ), glmColor ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::DebugIcoSphere( const btTransform _transform, const float _radius, const int _numSubdivisions, const Color _color, const bool _depthTestEnable )
	{
		if ( _radius <= 0 )
		{
			Debug::Warning( "Debug sphere radius cannot be zero or negative" );
			return;
		}

		std::vector<btVector3> sphere = GetSphere( _radius, _numSubdivisions );

		for ( int vertIndex = 0; vertIndex < sphere.size(); vertIndex++ )
		{
			sphere[ vertIndex ] = _transform * sphere[ vertIndex ];
		}

		for ( int triangleIndex = 0; triangleIndex < sphere.size() / 3; triangleIndex++ )
		{
			const btVector3 v0 = sphere[ 3 * triangleIndex + 0 ];
			const btVector3 v1 = sphere[ 3 * triangleIndex + 1 ];
			const btVector3 v2 = sphere[ 3 * triangleIndex + 2 ];
			DebugLine( v0, v1, _color, _depthTestEnable );
			DebugLine( v1, v2, _color, _depthTestEnable );
			DebugLine( v2, v0, _color, _depthTestEnable );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::DebugSphere( const btTransform _transform, const float _radius, const Color _color, const bool _depthTestEnable )
	{
		DebugCircle( _transform.getOrigin(), _radius, btVector3::Up(), 32, _color, _depthTestEnable );
		DebugCircle( _transform.getOrigin(), _radius, btVector3::Left(), 32, _color, _depthTestEnable );
		DebugCircle( _transform.getOrigin(), _radius, btVector3::Forward(), 32, _color, _depthTestEnable );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::DebugCone( const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const Color _color )
	{
		std::vector<btVector3> cone = GetCone( _radius, _height, _numSubdivisions );

		for ( int vertIndex = 0; vertIndex < cone.size(); vertIndex++ )
		{
			cone[ vertIndex ] = _transform * cone[ vertIndex ];
		}

		for ( int triangleIndex = 0; triangleIndex < cone.size() / 3; triangleIndex++ )
		{
			DebugTriangle( cone[ 3 * triangleIndex + 0 ], cone[ 3 * triangleIndex + 1 ], cone[ 3 * triangleIndex + 2 ], _color );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::DebugAABB( const AABB& _aabb, const Color _color )
	{
		std::vector< btVector3 > corners = _aabb.GetCorners();
		// Top
		DebugLine( corners[ 0 ], corners[ 1 ], _color );
		DebugLine( corners[ 1 ], corners[ 2 ], _color );
		DebugLine( corners[ 2 ], corners[ 3 ], _color );
		DebugLine( corners[ 3 ], corners[ 0 ], _color );
		// Bot
		DebugLine( corners[ 4 ], corners[ 5 ], _color );
		DebugLine( corners[ 5 ], corners[ 6 ], _color );
		DebugLine( corners[ 6 ], corners[ 7 ], _color );
		DebugLine( corners[ 7 ], corners[ 4 ], _color );
		//Vertical sides
		DebugLine( corners[ 0 ], corners[ 4 ], _color );
		DebugLine( corners[ 1 ], corners[ 5 ], _color );
		DebugLine( corners[ 2 ], corners[ 6 ], _color );
		DebugLine( corners[ 3 ], corners[ 7 ], _color );
	}

}