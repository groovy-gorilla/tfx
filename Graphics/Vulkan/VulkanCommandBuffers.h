#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanCommandBuffers {
public:
    void Create(VkDevice device, VkCommandPool pool, uint32_t count);

    void Destroy(VkDevice device, VkCommandPool pool);

    const std::vector<VkCommandBuffer>& Get() const;

private:
    std::vector<VkCommandBuffer> m_commandBuffers;

};
