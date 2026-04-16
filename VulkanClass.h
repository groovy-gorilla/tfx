#ifndef VULKANCLASS_H
#define VULKANCLASS_H
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <fstream>



class VulkanClass {

public:
    VulkanClass();
    ~VulkanClass();

    void Initialize(GLFWwindow *window);
    void Shutdown();

    VkDevice GetDevice();
    void DrawFrame(GLFWwindow* window);
    void SetResolution(uint32_t width, uint32_t height);
    void SetMSAA(VkSampleCountFlagBits msaa);
    void SetFilter(VkFilter filter);
    void SetFramebufferResized(bool value);
    void SetAspectRatioEnabled(bool value);

private:
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};


    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VkImage offscreenImage;
    VkDeviceMemory offscreenMemory;
    VkImageView offscreenImageView;
    VkImage msaaImage;
    VkDeviceMemory msaaMemory;
    VkImageView msaaImageView;
    VkRenderPass offscreenRenderPass;
    VkRenderPass msaaRenderPass;
    VkFramebuffer framebuffer;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout scenePipelineLayout;
    VkPipeline scenePipeline;
    VkPipelineLayout postPipelineLayout;
    VkPipeline postPipeline;
    VkSampler sampler;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkSampleCountFlagBits> msaaSamples;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    void CreateInstance();
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    void SetupDebugMessenger();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void CreateSurface(GLFWwindow *window);
    void PickPhysicalDevice();
    bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
    void CreateSupportedSampleCounts(VkPhysicalDevice physicalDevice);
    void CreateLogicalDevice();
    void CreateSwapChain(GLFWwindow* window);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow* window);
    void CreateImageViews();
    void CreateOffscreenResources();
    void CreateOffscreenRenderPass();
    void CreateMSAAResources();
    void CreateMSAARenderPass();
    void CreateFramebuffer();
    void CreateRenderPass();
    void CreateDescriptorSetLayout();
    void CreateScenePipeline();
    void CreatePostPipeline();
    void CreateSampler();
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void UpdateDescriptorSet();
    void CreateFramebuffers();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void CreateSyncObjects();
    void RecreateSwapChain(GLFWwindow* window);
    void RecreatePipeline();
    void RecreateSampler();
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    std::vector<char> ReadFile(const std::string &filename);
    VkShaderModule CreateShaderModule(const std::vector<char> &code);
    void CleanupSwapChain();
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

};



#endif //VULKANCLASS_H