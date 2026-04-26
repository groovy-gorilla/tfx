#pragma once
#include <vulkan/vulkan.h>

// ABSTRAKCJA
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual void Render() = 0;
    virtual void RecreatePipeline() = 0;
};


// KLASA BAZOWA
class Renderer : public IRenderer {
public:
    Renderer();
    virtual ~Renderer() = default;

protected:
    VkDevice m_device;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    virtual void CreatePipeline() = 0;



};


