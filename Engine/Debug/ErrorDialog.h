#pragma once

void ShowErrorDialog(const std::string& errorMessage);
[[nodiscard]] const char* VkResultToString(VkResult r);
[[noreturn]] void VulkanThrowError(const char* call, VkResult result, const char* file, int line);
std::string EscapeMarkup(const std::string& text);
const char* GetFileName(const char* path);

#define VK_CHECK(call)                                                              \
    do {                                                                            \
        VkResult result = call;                                                     \
        if (result < 0) {                                                 \
            VulkanThrowError(#call, result, GetFileName(__FILE__), __LINE__);       \
        }                                                                           \
    } while (0)
