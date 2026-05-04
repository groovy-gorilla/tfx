#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.h>


std::vector<char> ReadFile(const std::string& filename);
uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);
