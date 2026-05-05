#include "VulkanValidation.h"
#include <vulkan/vulkan.h>
#include <cstring>

namespace VulkanValidation {

    const std::vector<const char*> Layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    bool CheckSupport() {

        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> available(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, available.data());

        for (const char* required : Layers) {

            bool found = false;

            for (const auto& layer : available) {
                if (strcmp(required, layer.layerName) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                return false;
            }
        }

        return true;
    }

}

