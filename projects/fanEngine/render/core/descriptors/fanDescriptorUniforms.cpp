#include "render/core/descriptors/fanDescriptorUniforms.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DescriptorUniforms::Destroy( Device& _device )
    {
        if( mDescriptorPool != VK_NULL_HANDLE )
        {
            vkDestroyDescriptorPool( _device.mDevice, mDescriptorPool, nullptr );
            mDescriptorPool = VK_NULL_HANDLE;
        }

        if( mDescriptorSetLayout != VK_NULL_HANDLE )
        {
            vkDestroyDescriptorSetLayout( _device.mDevice, mDescriptorSetLayout, nullptr );
            mDescriptorSetLayout = VK_NULL_HANDLE;
        }

        for( int bindingIndex = 0; bindingIndex < (int)mBindingData.size(); bindingIndex++ )
        {
            for( int bufferIndex = 0; bufferIndex < (int)SwapChain::sMaxFramesInFlight; bufferIndex++ )
            {
                mBindingData[bindingIndex].mBuffers[bufferIndex].Destroy( _device );
            }
        }
    }

    //==================================================================================================================================================================================================
    // Adds a dynamic uniform buffer binding
    //==================================================================================================================================================================================================
    void DescriptorUniforms::AddUniformBinding( Device& _device,
                                                const size_t _count,
                                                const VkShaderStageFlags _stage,
                                                const VkDeviceSize _bufferSize )
    {
        BindingData bindingData;
        bindingData.CreateBuffers( _device, _count, _bufferSize );

        bindingData.mLayoutBinding.binding            = static_cast<uint32_t>( mBindingData.size() );
        bindingData.mLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindingData.mLayoutBinding.descriptorCount    = static_cast<uint32_t>( 1 );
        bindingData.mLayoutBinding.stageFlags         = _stage;
        bindingData.mLayoutBinding.pImmutableSamplers = nullptr;

        mBindingData.push_back( bindingData );
    }

    //==================================================================================================================================================================================================
    // Adds a uniform buffer binding
    //==================================================================================================================================================================================================
    void DescriptorUniforms::AddDynamicUniformBinding( Device& _device, const size_t _count, VkShaderStageFlags _stage, VkDeviceSize _bufferSize, VkDeviceSize _alignment )
    {
        BindingData bindingData;
        bindingData.CreateBuffers( _device, _count, _bufferSize * _alignment, _alignment );

        bindingData.mLayoutBinding.binding            = static_cast<uint32_t>( mBindingData.size() );
        bindingData.mLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        bindingData.mLayoutBinding.descriptorCount    = static_cast<uint32_t>( 1 );
        bindingData.mLayoutBinding.stageFlags         = _stage;
        bindingData.mLayoutBinding.pImmutableSamplers = nullptr;

        mBindingData.push_back( bindingData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DescriptorUniforms::ResizeDynamicUniformBinding( Device& _device, const size_t _count, VkDeviceSize _bufferSize, VkDeviceSize _alignment, const int _index )
    {
        fanAssert( _index < (int)mBindingData.size() );
        for( int bufferIndex = 0; bufferIndex < (int)_count; bufferIndex++ )
        {
            mBindingData[_index].mBuffers[bufferIndex].Destroy( _device );
        }
        mBindingData[_index].CreateBuffers( _device, _count, _bufferSize * _alignment, _alignment );
    }

    //==================================================================================================================================================================================================
    // For uniform buffers only, update buffer data of the binding at _index
    //==================================================================================================================================================================================================
    void DescriptorUniforms::SetData( Device& _device, const size_t _indexBinding, const size_t _indexBuffer, const void* _data, VkDeviceSize _size, VkDeviceSize _offset )
    {
        fanAssert( _indexBinding < mBindingData.size() );
        mBindingData[_indexBinding].mBuffers[_indexBuffer].SetData( _device, _data, _size, _offset );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DescriptorUniforms::Bind( VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout, const size_t _index )
    {
        vkCmdBindDescriptorSets( _commandBuffer,
                                 VK_PIPELINE_BIND_POINT_GRAPHICS,
                                 _pipelineLayout,
                                 0,
                                 1,
                                 &mDescriptorSets[_index],
                                 0,
                                 nullptr );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool DescriptorUniforms::Create( Device& _device, const uint32_t _count )
    {
        // Create DescriptorSetLayout
        {
            std::vector<VkDescriptorSetLayoutBinding> bindingsArray;
            bindingsArray.reserve( mBindingData.size() );
            for( int bindingIndex = 0; bindingIndex < (int)mBindingData.size(); bindingIndex++ )
            {
                bindingsArray.push_back( mBindingData[bindingIndex].mLayoutBinding );
            }

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
            descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutCreateInfo.pNext        = nullptr;
            descriptorSetLayoutCreateInfo.flags        = 0;
            descriptorSetLayoutCreateInfo.bindingCount = static_cast< uint32_t >( mBindingData.size() );
            descriptorSetLayoutCreateInfo.pBindings    = bindingsArray.data();

            if( vkCreateDescriptorSetLayout( _device.mDevice,
                                             &descriptorSetLayoutCreateInfo,
                                             nullptr,
                                             &mDescriptorSetLayout ) != VK_SUCCESS )
            {
                Debug::Error( "Could not allocate descriptor set layout." );
                return false;
            }
            Debug::Log() << Debug::Type::Render << std::hex << "VkDescriptorSetLayout " << mDescriptorSetLayout << std::dec << Debug::Endl();
        }

        // Create DescriptorPool
        {
            std::vector<VkDescriptorPoolSize> poolSizes( mBindingData.size() );
            for( int                          i                        = 0; i < (int)mBindingData.size(); i++ )
            {
                poolSizes[i].type            = mBindingData[i].mLayoutBinding.descriptorType;
                poolSizes[i].descriptorCount = mBindingData[i].mLayoutBinding.descriptorCount;
            }
            VkDescriptorPoolCreateInfo        descriptorPoolCreateInfo = {};
            descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptorPoolCreateInfo.pNext         = nullptr;
            descriptorPoolCreateInfo.flags         = 0;
            descriptorPoolCreateInfo.maxSets       = _count;
            descriptorPoolCreateInfo.poolSizeCount = static_cast< uint32_t >( poolSizes.size() );
            descriptorPoolCreateInfo.pPoolSizes    = poolSizes.data();
            if( vkCreateDescriptorPool( _device.mDevice, &descriptorPoolCreateInfo, nullptr, &mDescriptorPool ) != VK_SUCCESS )
            {
                Debug::Error( "Could not allocate descriptor pool." );
                return false;
            }
            Debug::Log() << Debug::Type::Render << std::hex << "VkDescriptorPool      " << mDescriptorPool << std::dec << Debug::Endl();
        }

        // Create descriptor set
        {
            std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
            for( uint32_t                      layoutIndex = 0; layoutIndex < _count; layoutIndex++ )
            {
                descriptorSetLayouts.push_back( mDescriptorSetLayout );
            }
            VkDescriptorSetAllocateInfo        descriptorSetAllocateInfo;
            descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocateInfo.pNext              = nullptr;
            descriptorSetAllocateInfo.descriptorPool     = mDescriptorPool;
            descriptorSetAllocateInfo.descriptorSetCount = _count;
            descriptorSetAllocateInfo.pSetLayouts        = descriptorSetLayouts.data();

            if( vkAllocateDescriptorSets( _device.mDevice,
                                          &descriptorSetAllocateInfo,
                                          mDescriptorSets ) != VK_SUCCESS )
            {
                Debug::Error( "Could not allocate descriptor set." );
                return false;
            }
            for( uint32_t setIndex = 0; setIndex < _count; setIndex++ )
            {
                Debug::Log() << Debug::Type::Render << std::hex << "VkDescriptorSet       " << mDescriptorSets[setIndex] << std::dec << Debug::Endl();
            }
        }

        UpdateDescriptorSets( _device, _count );

        return true;
    }

    //==================================================================================================================================================================================================
    // Update the descriptors sets using the old WriteDescriptor sets
    //==================================================================================================================================================================================================
    void DescriptorUniforms::UpdateDescriptorSets( Device& _device, const uint32_t _count )
    {
        std::vector<VkWriteDescriptorSet>                 writeDescriptors;
        std::vector<std::vector<VkDescriptorBufferInfo> > descriptorBufferInfos( mBindingData.size() );
        std::vector<std::vector<VkWriteDescriptorSet> >   writeDescriptorSets( mBindingData.size() );

        for( int bindingIndex = 0; bindingIndex < (int)mBindingData.size(); bindingIndex++ )
        {
            BindingData& bindingData = mBindingData[bindingIndex];

            std::vector<VkDescriptorBufferInfo>& descriptorBufferInfo = descriptorBufferInfos[bindingIndex];
            std::vector<VkWriteDescriptorSet>  & writeDescriptorSet   = writeDescriptorSets[bindingIndex];
            descriptorBufferInfo.resize( _count );
            writeDescriptorSet.resize( _count );

            for( uint32_t i = 0; i < _count; i++ )
            {
                descriptorBufferInfo[i].buffer = bindingData.mBuffers[i].mBuffer;
                descriptorBufferInfo[i].offset = 0;
                descriptorBufferInfo[i].range  = bindingData.mBuffers[i].mAlignment > 1 ?
                        bindingData.mBuffers[i].mAlignment :
                        bindingData.mBuffers[i].mSize;

                writeDescriptorSet[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet[i].pNext           = nullptr;
                writeDescriptorSet[i].dstSet          = mDescriptorSets[i];
                writeDescriptorSet[i].dstBinding      = static_cast<uint32_t>( bindingIndex );
                writeDescriptorSet[i].dstArrayElement = 0;
                writeDescriptorSet[i].descriptorCount = bindingData.mLayoutBinding.descriptorCount;
                writeDescriptorSet[i].descriptorType  = bindingData.mLayoutBinding.descriptorType;
                writeDescriptorSet[i].pImageInfo      = nullptr;
                writeDescriptorSet[i].pBufferInfo     = &descriptorBufferInfo[i];
                //uboWriteDescriptorSet.pTexelBufferView = nullptr;

                writeDescriptors.push_back( writeDescriptorSet[i] );
            }
        }

        vkUpdateDescriptorSets(
                _device.mDevice,
                static_cast< uint32_t >( writeDescriptors.size() ),
                writeDescriptors.data(),
                0,
                nullptr
        );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DescriptorUniforms::BindingData::CreateBuffers( Device& _device, const size_t _count, VkDeviceSize _sizeBuffer, VkDeviceSize _alignment )
    {
        fanAssert( _count <= SwapChain::sMaxFramesInFlight );
        for( int bufferIndex = 0; bufferIndex < (int)_count; bufferIndex++ )
        {
            mBuffers[bufferIndex].Create( _device, _sizeBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, _alignment );
        }
    }
}
      