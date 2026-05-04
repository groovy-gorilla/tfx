#pragma once
#include <vulkan/vulkan.h>

class VulkanSampler {
public:
    void Create(VkDevice device);
    void Destroy(VkDevice device);

    VkSampler Get() const { return m_sampler; }

private:
    VkSampler m_sampler = VK_NULL_HANDLE;
};

