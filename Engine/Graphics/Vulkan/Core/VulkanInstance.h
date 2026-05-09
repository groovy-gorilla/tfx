#pragma once

struct ApplicationDesc;

class VulkanInstance {
public:

    void Create(ApplicationDesc& desc);
    void Destroy();

    [[nodiscard]] VkInstance Get() const;

private:
    VkInstance m_instance = VK_NULL_HANDLE;

    std::vector<const char*> GetRequiredExtensions();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

};