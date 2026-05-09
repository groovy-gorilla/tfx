#include "pch.h"
#include "VulkanTextureDescriptor.h"
#include "RenderTarget.h"
#include "Debug/ErrorDialog.h"
#include "Core/ApplicationDesc.h"

void VulkanTextureDescriptor::Create(VkDevice device, RenderTarget& colorTarget, RenderTarget& depthTarget, TextureFilter filter) {

    // DESCRIPTOR SET LAYOUT
    VkDescriptorSetLayoutBinding bindings[2]{};
    // Scene color
    bindings[0].binding = 0;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[0].pImmutableSamplers = nullptr;
    // Depth
    bindings[1].binding = 1;
    bindings[1].descriptorCount = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[1].pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = bindings;

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_layout));

    // DESCRIPTOR POOL
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 2;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_pool));

    // DESCRIPTOR SET
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_layout;

    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &m_set));

    // IMAGE INFO
    VkDescriptorImageInfo colorInfo{};
    colorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    colorInfo.imageView = colorTarget.View;
    colorInfo.sampler = (filter == TextureFilter::Nearest) ? colorTarget.NearestSampler : colorTarget.LinearSampler;
    VkDescriptorImageInfo depthInfo{};
    depthInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    depthInfo.imageView = depthTarget.View;
    depthInfo.sampler = (filter == TextureFilter::Nearest) ? depthTarget.NearestSampler : depthTarget.LinearSampler;

    // WRITE DESCRIPTOR
    VkWriteDescriptorSet writes[2]{};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = m_set;
    writes[0].dstBinding = 0;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[0].pImageInfo = &colorInfo;
    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = m_set;
    writes[1].dstBinding = 1;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo = &depthInfo;

    vkUpdateDescriptorSets(device, 2, writes, 0, nullptr);

}

void VulkanTextureDescriptor::Destroy(VkDevice device) {

    if (m_pool) {
        vkDestroyDescriptorPool(device, m_pool, nullptr);
        m_pool = VK_NULL_HANDLE;
    }

    if (m_layout) {
        vkDestroyDescriptorSetLayout(device, m_layout, nullptr);
        m_layout = VK_NULL_HANDLE;
    }

}

void VulkanTextureDescriptor::UpdateSampler(VkDevice device, RenderTarget& color, TextureFilter filter) {

    VkDescriptorImageInfo colorInfo{};
    colorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    colorInfo.imageView = color.View;
    colorInfo.sampler = (filter == TextureFilter::Nearest) ? color.NearestSampler : color.LinearSampler;

    VkWriteDescriptorSet colorWrite{};
    colorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    colorWrite.dstSet = m_set;
    colorWrite.dstBinding = 0;
    colorWrite.dstArrayElement = 0;
    colorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    colorWrite.descriptorCount = 1;
    colorWrite.pImageInfo = &colorInfo;

    vkUpdateDescriptorSets(device, 1, &colorWrite, 0, nullptr);

}