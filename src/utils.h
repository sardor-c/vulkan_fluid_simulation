#pragma once

#include <vulkan/vulkan.h>
#include <string>


namespace utils {

    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger
    );

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator
    );



    void vkCheck(VkResult result, const char* what);


    void printVulkanVersion();
    bool isLayerAvailable(const char* name);
    void printAvailableLayers();


    std::string createInfoLog(const std::string& matn);
    std::string createErrorLog(const std::string& matn);
    std::string createWarningLog(const std::string& matn);
}

#define VK_CHECK(x) ::utils::vkCheck((x), #x)