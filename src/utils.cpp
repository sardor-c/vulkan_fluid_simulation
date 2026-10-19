#include "utils.h"

#include <vector>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include "color.h"


namespace utils {
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger
    ) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator
    ) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }


    void vkCheck(VkResult result, const char* what) {
        if (result != VK_SUCCESS) {
            throw std::runtime_error(std::string(what) + " xato qaytardi: " + std::to_string(result));
        }
    }


    bool printVulkanVersion() {
        uint32_t version = 0;
        VkResult result = vkEnumerateInstanceVersion(&version);
        vkCheck(result, "printVulkanVersion");
        std::cout << "Vulkan (loader) versiyasi: "
            << VK_API_VERSION_MAJOR(version) << '.'
            << VK_API_VERSION_MINOR(version) << '.'
            << VK_API_VERSION_PATCH(version) << '\n';

        return true;
    }

    bool isLayerAvailable(const char* name) {
        uint32_t count = 0;
        vkCheck(vkEnumerateInstanceLayerProperties(&count, nullptr), "vkEnumerateInstanceLayerProperties");

        std::vector<VkLayerProperties> layers(count);
        vkCheck(vkEnumerateInstanceLayerProperties(&count, layers.data()), "vkEnumerateInstanceLayerProperties");

        for (const auto& layer : layers) {
            if (std::strcmp(layer.layerName, name) == 0) {
                return true;
            }
        }
        return false;
    }

    void printAvailableLayers() {
        uint32_t count = 0;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));

        std::vector<VkLayerProperties> layers(count);
        vkCheck(vkEnumerateInstanceLayerProperties(&count, layers.data()), "vkEnumerateInstanceLayerProperties");

        std::cout << color::YASHIL << "-----[ Layerlar ]-----" << std::endl;
        for (const auto& layer : layers) {
            std::cout << "| " << layer.layerName << std::endl;
        }
        std::cout << color::YASHIL << "-----------------------" << color::RESET;

    }

    std::string createInfoLog(const std::string& matn) {
        return std::string(color::RESET) + color::FIRUZA + "----[ INFO ]-----" + matn + color::RESET + "\n";
    }

    std::string createErrorLog(const std::string& matn) {
        return std::string(color::RESET) + color::QIZIL + "----[ ERROR ]-----" + matn + color::RESET + "\n";
    }

    std::string createWarningLog(const std::string& matn) {
        return std::string(color::RESET) + color::SARIQ + "----[ WARNING ]-----" + matn + color::RESET + "\n";
    }
}




