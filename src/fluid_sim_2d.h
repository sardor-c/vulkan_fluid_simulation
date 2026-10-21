#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>


class FluidSimulation2D {
public:
    FluidSimulation2D() = default;
    ~FluidSimulation2D();

    FluidSimulation2D(const FluidSimulation2D&) = delete;
    FluidSimulation2D& operator=(const FluidSimulation2D&) = delete;

    void run();

private:
    GLFWwindow* window = nullptr;
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t queueFamilyIndex = 0;
    VkQueue queue = VK_NULL_HANDLE;

    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();


    void createInstance();

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions(); // complete
    void checkRequiredExtensions(const std::vector<const char*>& requiredExtensions);
    void makeDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    void pickPhysicalDevice();
    int rateGpu(VkPhysicalDevice gpu);
    bool isDeviceSuitable(VkPhysicalDevice gpu);
    std::optional<uint32_t> findQueueFamily(VkPhysicalDevice gpu);
    void createLogicalDevice();

};