#include "pch.h"
#include "VulkanTextureDescriptor.h"
#include "RenderTarget.h"
#include "Debug/ErrorDialog.h"
#include "Core/ApplicationDesc.h"

void VulkanTextureDescriptor::Create(VkDevice device, uint32_t maxFramesInFlight, RenderTarget& colorTarget, RenderTarget& depthTarget, TextureFilter filter) {

    CreateDescriptorResources(device, maxFramesInFlight);

    for (uint32_t i = 0; i < maxFramesInFlight; i++) {
        UpdateColor(device, i, colorTarget, filter);
        UpdateDepth(device, i, depthTarget, filter);
    }

}

void VulkanTextureDescriptor::Create(VkDevice device, uint32_t maxFramesInFlight, TextureFilter filter) {

    CreateDescriptorResources(device, maxFramesInFlight);

}

void VulkanTextureDescriptor::CreateColor(VkDevice device, uint32_t maxFramesInFlight) {

    // DESCRIPTOR SET LAYOUT
    VkDescriptorSetLayoutBinding binding{};

    // COLOR
    binding.binding = 0;
    binding.descriptorCount = 1;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &binding;

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo,nullptr, &m_layout));

    // DESCRIPTOR POOL
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 2 * maxFramesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = maxFramesInFlight;

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_pool));

    // DESCRIPTOR SET
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType =VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = maxFramesInFlight;

    std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight, m_layout);

    allocInfo.pSetLayouts = layouts.data();

    m_sets.resize(maxFramesInFlight);

    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, m_sets.data()));

}

void VulkanTextureDescriptor::CreateSMAABlend(VkDevice device, uint32_t maxFramesInFlight) {

    VkDescriptorSetLayoutBinding bindings[3]{};

    bindings[0].binding = 1;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings[1].binding = 2;
    bindings[1].descriptorCount = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings[2].binding = 3;
    bindings[2].descriptorCount = 1;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 3;
    layoutInfo.pBindings = bindings;

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_layout));

    // DESCRIPTOR POOL
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 3 * maxFramesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = maxFramesInFlight;

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_pool));

    // DESCRIPTOR SETS
    std::vector<VkDescriptorSetLayout> layouts(
        maxFramesInFlight,
        m_layout
    );

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = maxFramesInFlight;
    allocInfo.pSetLayouts = layouts.data();

    m_sets.resize(maxFramesInFlight);

    VK_CHECK(vkAllocateDescriptorSets( device, &allocInfo, m_sets.data()));

}

void VulkanTextureDescriptor::CreateSMAANeighborhood(VkDevice device, uint32_t maxFramesInFlight) {

    VkDescriptorSetLayoutBinding bindings[2]{};

    // INPUT COLOR
    bindings[0].binding = 0;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // BLEND WEIGHTS
    bindings[1].binding = 4;
    bindings[1].descriptorCount = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = bindings;

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_layout));

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 2 * maxFramesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = maxFramesInFlight;

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_pool));

    //...
    // DESCRIPTOR SETS
    std::vector<VkDescriptorSetLayout> layouts(
        maxFramesInFlight,
        m_layout
    );

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = maxFramesInFlight;
    allocInfo.pSetLayouts = layouts.data();

    m_sets.resize(maxFramesInFlight);

    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, m_sets.data()));

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

void VulkanTextureDescriptor::CreateDescriptorResources(VkDevice device, uint32_t maxFramesInFlight) {

    // DESCRIPTOR SET LAYOUT
    VkDescriptorSetLayoutBinding bindings[2]{};

    // COLOR
    bindings[0].binding = 0;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // DEPTH
    bindings[1].binding = 1;
    bindings[1].descriptorCount = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = bindings;

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo,nullptr, &m_layout));

    // DESCRIPTOR POOL
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 2 * maxFramesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = maxFramesInFlight;

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_pool));

    // DESCRIPTOR SET
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType =VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = maxFramesInFlight;

    std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight, m_layout);

    allocInfo.pSetLayouts = layouts.data();

    m_sets.resize(maxFramesInFlight);

    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, m_sets.data()));

}

void VulkanTextureDescriptor::UpdateColor(VkDevice device, uint32_t frameIndex, RenderTarget& color, TextureFilter filter) {

    VkDescriptorImageInfo colorInfo{};
    colorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    colorInfo.imageView = color.GetImageView();
    colorInfo.sampler = (filter == TextureFilter::Nearest) ? color.GetNearestSampler() : color.GetLinearSampler();

    VkWriteDescriptorSet colorWrite{};
    colorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    colorWrite.dstSet = m_sets[frameIndex];
    colorWrite.dstBinding = 0;
    colorWrite.dstArrayElement = 0;
    colorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    colorWrite.descriptorCount = 1;
    colorWrite.pImageInfo = &colorInfo;

    vkUpdateDescriptorSets(device, 1, &colorWrite, 0, nullptr);

}

void VulkanTextureDescriptor::UpdateDepth(VkDevice device, uint32_t frameIndex, RenderTarget& depth, TextureFilter filter) {

    VkDescriptorImageInfo depthInfo{};
    depthInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    depthInfo.imageView = depth.GetImageView();
    depthInfo.sampler = (filter == TextureFilter::Nearest) ? depth.GetNearestSampler() : depth.GetLinearSampler();

    VkWriteDescriptorSet depthWrite{};
    depthWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    depthWrite.dstSet = m_sets[frameIndex];
    depthWrite.dstBinding = 1;
    depthWrite.dstArrayElement = 0;
    depthWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    depthWrite.descriptorCount = 1;
    depthWrite.pImageInfo = &depthInfo;

    vkUpdateDescriptorSets(device, 1, &depthWrite, 0, nullptr);

}

void VulkanTextureDescriptor::UpdateSMAABlend(VkDevice device, uint32_t frameIndex, RenderTarget& edge, RenderTarget& area, RenderTarget& search) {

    VkDescriptorImageInfo edgeInfo{};
    edgeInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    edgeInfo.imageView = edge.GetImageView();
    edgeInfo.sampler = edge.GetLinearSampler();

    VkDescriptorImageInfo areaInfo{};
    areaInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    areaInfo.imageView = area.GetImageView();
    areaInfo.sampler = area.GetLinearSampler();

    VkDescriptorImageInfo searchInfo{};
    searchInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    searchInfo.imageView = search.GetImageView();
    searchInfo.sampler = search.GetNearestSampler();

    VkWriteDescriptorSet writes[3]{};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = m_sets[frameIndex];
    writes[0].dstBinding = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[0].descriptorCount = 1;
    writes[0].pImageInfo = &edgeInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = m_sets[frameIndex];
    writes[1].dstBinding = 2;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].descriptorCount = 1;
    writes[1].pImageInfo = &areaInfo;

    writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstSet = m_sets[frameIndex];
    writes[2].dstBinding = 3;
    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[2].descriptorCount = 1;
    writes[2].pImageInfo = &searchInfo;

    vkUpdateDescriptorSets( device, 3, writes, 0, nullptr);

}

void VulkanTextureDescriptor::UpdateSMAANeighborhood(VkDevice device, uint32_t currentFrame, RenderTarget& inputColor, RenderTarget& blendWeights) {

    VkDescriptorImageInfo inputInfo{};
    inputInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    inputInfo.imageView = inputColor.GetImageView();
    inputInfo.sampler = inputColor.GetLinearSampler();

    VkDescriptorImageInfo blendInfo{};
    blendInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    blendInfo.imageView = blendWeights.GetImageView();
    blendInfo.sampler = blendWeights.GetLinearSampler();

    VkWriteDescriptorSet writes[2]{};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = m_sets[currentFrame];
    writes[0].dstBinding = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[0].pImageInfo = &inputInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = m_sets[currentFrame];
    writes[1].dstBinding = 4;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo = &blendInfo;

    vkUpdateDescriptorSets(device, 2, writes,0, nullptr);

}


