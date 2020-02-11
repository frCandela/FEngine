#pragma once

#include "render/fanRenderPrecompiled.hpp"

namespace fan
{
	class Device;
	class Texture;
	class Sampler;

	//================================================================================================================================
	//================================================================================================================================
	class DescriptorTextures
	{
	public:
		DescriptorTextures( Device& _device, const uint32_t _maxTextures, VkSampler _sampler = VK_NULL_HANDLE );
		~DescriptorTextures();

		uint32_t Append( VkImageView _imageView );
		void	 Set( const uint32_t _index, VkImageView _imageView );
		void	 UpdateRange( const uint32_t _begin, const uint32_t _end );
		uint32_t Count() { return static_cast< uint32_t >( m_views.size() ); }
		VkDescriptorSetLayout GetLayout() { return m_descriptorSetLayout; }
		VkDescriptorSet		  GetSet( const uint32_t _index = 0 ) { return m_descriptorSets[ _index ]; }
	private:
		Device& m_device;

		const uint32_t m_maxTextures;
		std::vector< VkImageView >      m_views;

		VkSampler m_sampler = VK_NULL_HANDLE;
		VkDescriptorType m_descriptorType;

		VkDescriptorSetLayout			m_descriptorSetLayout;
		VkDescriptorPool				m_descriptorPool;
		std::vector< VkDescriptorSet >	m_descriptorSets;
	};
}