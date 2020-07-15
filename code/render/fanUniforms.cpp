#include "render/fanUniforms.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ForwardUniforms::Create( const VkDeviceSize _minUniformBufferOffsetAlignment )
	{
		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)_minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformsVert );
		size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
		if( minUboAlignment > 0 )
		{
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformsVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
			dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}

		m_dynamicUniformsVert.SetAlignement( dynamicAlignmentVert );
		m_dynamicUniformsMaterial.SetAlignement( dynamicAlignmentFrag );

		m_dynamicUniformsVert.Resize( 256 );
		m_dynamicUniformsMaterial.Resize( 256 );

		for( int uniformIndex = 0; uniformIndex < m_dynamicUniformsMaterial.Size(); uniformIndex++ )
		{
			m_dynamicUniformsMaterial[uniformIndex].color = glm::vec4( 1 );
			m_dynamicUniformsMaterial[uniformIndex].shininess = 1;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void UiUniforms::Create( const VkDeviceSize _minUniformBufferOffsetAlignment )
	{
		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)_minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformUIVert );
		if( minUboAlignment > 0 )
		{
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformUIVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}
		mUidynamicUniformsVert.SetAlignement( dynamicAlignmentVert );
		mUidynamicUniformsVert.Resize( 256 );
	}
}