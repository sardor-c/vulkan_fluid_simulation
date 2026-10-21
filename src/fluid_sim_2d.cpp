#include "fluid_sim_2d.h"

#include <iostream>
#include <stdexcept>
#include <cstring>
#include "utils.h"

namespace {
    constexpr uint32_t WIDTH = 1280;
    constexpr uint32_t HEIGHT = 720;


    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };


#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif

}

FluidSimulation2D::~FluidSimulation2D(){
    cleanup();
}

void FluidSimulation2D::run() {
    initWindow();
    initVulkan();
    mainLoop();
}

void FluidSimulation2D::initWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW ishga tushmadi!");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Fluid Sim 2D", nullptr, nullptr);
    if (window == nullptr) {
        throw std::runtime_error("GLFW window yaratilmadi!");
    }

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        });

}

void FluidSimulation2D::initVulkan() {
    utils::printVulkanVersion();
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
}

void FluidSimulation2D::mainLoop() {
    double lastTime = glfwGetTime();
    double fpsTimer = lastTime;
    int frameCount = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double now = glfwGetTime();
        double deltaTime = now - lastTime;
        lastTime = now;

        frameCount++;
        if (now - fpsTimer >= 1.0)
        {
            std::string title = "Fluid Sim 2D | FPS: " + std::to_string(frameCount) +
                " | kadr vaqti: " + std::to_string(deltaTime * 1000.0) + " ms";
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            fpsTimer = now;
        }
    }

}

void FluidSimulation2D::cleanup() {

    if (device != VK_NULL_HANDLE){
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
    
    if (debugMessenger != VK_NULL_HANDLE) {
        utils::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        debugMessenger = VK_NULL_HANDLE;
    }
    
    if (instance != VK_NULL_HANDLE){
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }

    if (window != nullptr){
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

void FluidSimulation2D::createInstance() {

    if (enableValidationLayers) {
        std::cout << utils::createInfoLog("Validation layer yoqilgan!");
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layer yoqilgan, lekin topilmadi!");
        }
    }
    else {
        std::cout << utils::createInfoLog("Validation layer o'chirilgan!");
    }

    VkApplicationInfo appInfo{};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Fluid Sim 2D";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    auto extensions = getRequiredExtensions();
    checkRequiredExtensions(extensions);

    VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef __APPLE__
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        makeDebugMessengerInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
        utils::printAvailableLayers();
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
    std::cout << utils::createInfoLog("VkInstance muvaffaqiyatli yaratildi!");
}


bool FluidSimulation2D::checkValidationLayerSupport() {
    uint32_t layerCount = 0;
    utils::vkCheck(vkEnumerateInstanceLayerProperties(&layerCount, nullptr), "vkEnumerateInstanceLayerProperties");

    std::vector<VkLayerProperties> availableLayers(layerCount);
    utils::vkCheck(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()), "vkEnumerateInstanceLayerProperties");

    for (const char* required : validationLayers) {
        bool layerFound = false;
        for (const auto& available : availableLayers) {
            if (strcmp(required, available.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char*> FluidSimulation2D::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

#ifdef __APPLE__
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    return extensions;
}

void FluidSimulation2D::checkRequiredExtensions(const std::vector<const char*>& requiredExtensions) {
    uint32_t availableCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(availableCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableCount, availableExtensions.data());
    bool hasError = false;
    for (const char* required : requiredExtensions) {
        bool found = false;
        for (const auto& available : availableExtensions) {
            if (strcmp(required, available.extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::cerr << utils::createErrorLog(std::string(required) + " extension talab qilindi, lekin topilmadi!");
            hasError = true;
        }
    }
    if (hasError) {
        throw std::runtime_error("Kerakli Extensionlar topilmadi!");
    }
    else {
        std::cout << utils::createInfoLog("Barcha kerakli extensionlar topildi!");
    }
}

void FluidSimulation2D::makeDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        //VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void FluidSimulation2D::setupDebugMessenger() {
    if (!enableValidationLayers) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    makeDebugMessengerInfo(createInfo);

    VK_CHECK(utils::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger));
    std::cout << utils::createInfoLog("Debug messenger yaratildi!");

}

VKAPI_ATTR VkBool32 VKAPI_CALL FluidSimulation2D::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
) {
    (void)messageType;
    (void)pUserData;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::cerr << utils::createErrorLog(pCallbackData->pMessage);
    }
    else {
        std::cout << utils::createWarningLog(pCallbackData->pMessage);
    }
    return VK_FALSE;
}

void FluidSimulation2D::pickPhysicalDevice() {
    uint32_t count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &count, nullptr));
    if (count == 0){
        throw std::runtime_error("Vulkanni qo'llab quvvatlovchi GPU topilmadi!");
    }

    std::vector<VkPhysicalDevice> devices(count);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &count, devices.data()));
    int highScore = 0;
    for (const auto& item: devices){
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(item, &properties);
        bool isDiscretGpu = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        int score = rateGpu(item);
        std::cout << "GPU: " << properties.deviceName << (isDiscretGpu ? " | diskret " : "")  << " | score: " << score << std::endl;
        if (score > highScore){
            highScore = score;
            physicalDevice = item;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE){
        throw std::runtime_error("Mos GPU topilmadi!");
    }

    queueFamilyIndex = findQueueFamily(physicalDevice).value();

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    std::cout << utils::createInfoLog(
        "Tanlangan GPU: " + std::string(properties.deviceName) + ". queueFamilyIndex: " 
        + std::to_string(queueFamilyIndex));
    utils::printQueueFamilies(physicalDevice);
}


int FluidSimulation2D::rateGpu(VkPhysicalDevice gpu) {
    if (!isDeviceSuitable(gpu)){
        return 0;
    }

    int score = 1;
    
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(gpu, &properties);
    
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 100000;
    }
    score += properties.limits.maxImageDimension2D;
    return score;
}

bool FluidSimulation2D::isDeviceSuitable(VkPhysicalDevice gpu) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(gpu, &properties);
    if (properties.apiVersion < VK_API_VERSION_1_3){
        return false;
    }

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 features2{};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &features13;

    vkGetPhysicalDeviceFeatures2(gpu, &features2);
    if (!features13.dynamicRendering || !features13.synchronization2){
        return false;
    }

    return findQueueFamily(gpu).has_value();
}

std::optional<uint32_t> FluidSimulation2D::findQueueFamily(VkPhysicalDevice gpu)
{
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, nullptr);

    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, families.data());

    const VkQueueFlags required = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
    for(uint32_t i = 0; i < count; i++){
        if ((families[i].queueFlags & required) == required){
            return i;
        }
    }

    return std::nullopt;
}

void FluidSimulation2D::createLogicalDevice() {
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = queueFamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = VK_TRUE;
    features13.synchronization2 = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &features13;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = nullptr;

    uint32_t deviceExtensionCount = 0;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr));
    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data()));

    std::vector<const char*> enabledExtensions;

    for (const auto& deviceExtension: deviceExtensions){
        if (std::strcmp(deviceExtension.extensionName, "VK_KHR_portability_subset") == 0){
            enabledExtensions.push_back("VK_KHR_portability_subset");
        }
    }

    createInfo.pQueueCreateInfos = &queueInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));

    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
    std::cout << utils::createInfoLog("Logical device va queue yaratildi");
}
