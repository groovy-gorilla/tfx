#pragma once

#include "Bitmap.h"
#include "Settings.h"
#include <vulkan/vulkan.h>


struct Vertex {
    float position[2];
    float texCoord[2];
};

class BitmapRenderer {

public:
    BitmapRenderer();
    ~BitmapRenderer();

    void Initialize(VkDevice& device, VkPhysicalDevice& physicalDevice, VkRenderPass& renderPass, VkExtent2D& extent);
    void Shutdown();
    void SetBitmap(Bitmap& bitmap);
    void Draw(VkCommandBuffer& cmdBuffer);
    void SetPosition(float x, float y, float width, float height);

private:
    VkDevice m_device;
    Bitmap* m_bitmap;
    VkDescriptorSetLayout m_descriptorSetLayout;
    VkDescriptorPool m_descriptorPool;
    VkDescriptorSet m_descriptorSet;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_pipeline;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkBuffer m_fragmentBuffer;
    VkDeviceMemory m_fragmentBufferMemory;
    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferMemory;

    void CreateDescriptorSetLayout();
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void UpdateDescriptor();
    void CreatePipeline(VkRenderPass renderPass, VkExtent2D extent);
    std::vector<char> ReadFile(const std::string &filename);
    VkShaderModule CreateShaderModule(const std::vector<char> &code);

    void CreateVertexBuffer(VkPhysicalDevice physicalDevice);
    void CreateIndexBuffer(VkPhysicalDevice physicalDevice);
};