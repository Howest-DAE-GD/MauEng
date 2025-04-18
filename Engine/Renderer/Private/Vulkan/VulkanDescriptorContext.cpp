#include "VulkanDescriptorContext.h"

namespace MauRen
{
	// ! THIS IS NOT SAFE TO CALL DURING A FRAME, HAS TO BE HANDLED IF WE WANT THAT
	void VulkanDescriptorContext::BindTexture(uint32_t destLocation, VkImageView imageView, VkImageLayout imageLayout)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = imageLayout;
		imageInfo.imageView = imageView;
		imageInfo.sampler = VK_NULL_HANDLE;

		auto const deviceContext{ VulkanDeviceContextManager::GetInstance().GetDeviceContext() };

		for (size_t i{ 0 }; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[i];
			descriptorWrite.dstBinding = TEXTURE_BINDING_SLOT;
			descriptorWrite.dstArrayElement = destLocation;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			descriptorWrite.descriptorCount = 1;

			descriptorWrite.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(deviceContext->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	// ! THIS IS NOT SAFE TO CALL DURING A FRAME, HAS TO BE HANDLED IF WE WANT THAT
	void VulkanDescriptorContext::BindMaterialBuffer(VkDescriptorBufferInfo bufferInfo, uint32_t frame)
	{
		auto const deviceContext{ VulkanDeviceContextManager::GetInstance().GetDeviceContext() };

		// Update descriptor set
		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_DescriptorSets[frame];

		descriptorWrite.dstBinding = MATERIAL_DATA_BINDING_SLOT;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(deviceContext->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
	}

	void VulkanDescriptorContext::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = UBO_BINDING_SLOT;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		// This could be used to specify a transformation for each of the bones in a skeleton for skeletal animation, for example.
		// Our MVP transformation is in a single uniform buffer object, so we're using a descriptorCount of 1.
		uboLayoutBinding.descriptorCount = 1;

		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		// Binding for global sampler
		VkDescriptorSetLayoutBinding samplerBinding{};
		samplerBinding.binding = SAMPLER_BINDING_SLOT;
		samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		samplerBinding.descriptorCount = 1;
		samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding bindlessTextureBinding{};
		bindlessTextureBinding.binding = TEXTURE_BINDING_SLOT;
		bindlessTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindlessTextureBinding.descriptorCount = MAX_TEXTURES;
		bindlessTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		// Binding for material data
		VkDescriptorSetLayoutBinding materialDataBinding{};
		materialDataBinding.binding = MATERIAL_DATA_BINDING_SLOT;
		materialDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		materialDataBinding.descriptorCount = 1;
		materialDataBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		materialDataBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding meshDataBinding{};
		meshDataBinding.binding = MESH_DATA_BINDING_SLOT;
		meshDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		meshDataBinding.descriptorCount = 1;
		meshDataBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		meshDataBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding meshInstanceDataBinding{};
		meshInstanceDataBinding.binding = MESH_INSTANCE_DATA_BINDING_SLOT;
		meshInstanceDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		meshInstanceDataBinding.descriptorCount = 1;
		meshInstanceDataBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		meshInstanceDataBinding.pImmutableSamplers = nullptr;

		std::array<VkDescriptorSetLayoutBinding, 6> const bindings {
			uboLayoutBinding,
			samplerBinding,
			bindlessTextureBinding,
			materialDataBinding,
			meshDataBinding,
			meshInstanceDataBinding
		};

		// Flags for the binding - only use valid flags for image samplers
		VkDescriptorBindingFlagsEXT bindingFlags[bindings.size()] {
			0,
			0,
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT, // Flags for bindless textures
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT, // Flags for material data
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT,
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT
		};
		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsInfo{};
		bindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
		bindingFlagsInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		bindingFlagsInfo.pBindingFlags = bindingFlags;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.pNext = &bindingFlagsInfo;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		auto const deviceContext{ VulkanDeviceContextManager::GetInstance().GetDeviceContext() };
		if (vkCreateDescriptorSetLayout(deviceContext->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout!");
		}
	}

	void VulkanDescriptorContext::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 6> poolSizes{};
		poolSizes[UBO_BINDING_SLOT].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[UBO_BINDING_SLOT].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		poolSizes[SAMPLER_BINDING_SLOT].type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSizes[SAMPLER_BINDING_SLOT].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		poolSizes[TEXTURE_BINDING_SLOT].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSizes[TEXTURE_BINDING_SLOT].descriptorCount = static_cast<uint32_t>(MAX_TEXTURES * MAX_FRAMES_IN_FLIGHT);

		poolSizes[MATERIAL_DATA_BINDING_SLOT].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[MATERIAL_DATA_BINDING_SLOT].descriptorCount = static_cast<uint32_t>(1 * MAX_FRAMES_IN_FLIGHT);

		poolSizes[MESH_DATA_BINDING_SLOT].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[MESH_DATA_BINDING_SLOT].descriptorCount = static_cast<uint32_t>(1 * MAX_FRAMES_IN_FLIGHT);

		poolSizes[MESH_INSTANCE_DATA_BINDING_SLOT].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[MESH_INSTANCE_DATA_BINDING_SLOT].descriptorCount = static_cast<uint32_t>(1 * MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

		auto const deviceContext{ VulkanDeviceContextManager::GetInstance().GetDeviceContext() };
		if (vkCreateDescriptorPool(deviceContext->GetLogicalDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {

			throw std::runtime_error("Failed to create descriptor pool!");
		}
	}

	void VulkanDescriptorContext::CreateDescriptorSets(std::vector<VulkanBuffer> const& bufferInfoBuffers, VkDeviceSize offset, VkDeviceSize range, VkImageLayout imageLayout, std::vector<VkImageView> const& imageViews, VkSampler sampler)
	{
		std::vector<VkDescriptorSetLayout> const layouts(MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

		auto const deviceContext{ VulkanDeviceContextManager::GetInstance().GetDeviceContext() };
		if (vkAllocateDescriptorSets(deviceContext->GetLogicalDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate descriptor sets!");
		}

		for (size_t i{ 0 }; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			VkWriteDescriptorSet descriptorWrites[2] = {};

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = bufferInfoBuffers[i].buffer;
			bufferInfo.offset = offset;
			bufferInfo.range = range;

			// Descriptor write for uniform buffer (binding 0)
			descriptorWrites[UBO_BINDING_SLOT] = {
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				m_DescriptorSets[i],
				UBO_BINDING_SLOT,  // Binding for the uniform buffer
				0,
				1,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				nullptr,
				&bufferInfo,
				nullptr
				};

			VkDescriptorImageInfo samplerInfo{};
			samplerInfo.sampler = sampler;
			samplerInfo.imageView = VK_NULL_HANDLE;
			samplerInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			descriptorWrites[SAMPLER_BINDING_SLOT] = {
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				m_DescriptorSets[i],
				SAMPLER_BINDING_SLOT,
				0,
				SAMPLER_BINDING_SLOT,
				VK_DESCRIPTOR_TYPE_SAMPLER,
				&samplerInfo,
				nullptr,
				nullptr
			};

			vkUpdateDescriptorSets(deviceContext->GetLogicalDevice(), 2, descriptorWrites, 0, nullptr);
		}
	}

	void VulkanDescriptorContext::Destroy()
	{
		auto const deviceContext{ VulkanDeviceContextManager::GetInstance().GetDeviceContext() };

		VulkanUtils::SafeDestroy(deviceContext->GetLogicalDevice(), m_DescriptorPool, nullptr);
		VulkanUtils::SafeDestroy(deviceContext->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
	}
}
