#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <string>


bool printVulkanVersion() {
    uint32_t version = 0;
    VkResult result = vkEnumerateInstanceVersion(&version);
    if (result != VK_SUCCESS) {
        std::cerr << "vkEnumerateInstanceVersion xato tashladi: " << result << std::endl;
        return false;
    }
    std::cout << "Vulkan (loader) versiyasi: "
        << ((version >> 22U) & 0b1111111) << '.'
        << ((version >> 12U) & 0b1111111111) << '.'
        << (version & 0b111111111111) << '\n';

    return true;
}

bool isPaused = false;

int main() {
    if (!printVulkanVersion()) {
        return 1;
    }

    if (!glfwInit()) {
        std::cerr << "GLFW ishga tushmadi!" << std::endl;
        return 1;
    }

    try {

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }


    uint32_t count = 0;
    const char** names = glfwGetRequiredInstanceExtensions(&count);

    for (uint32_t i = 0; i < count; i++) {
        std::cout << std::to_string(i) + ". " + names[i] << std::endl;
    }


    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid Sim Tutorial", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Oyna yaratilmadi!" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int /*scancode*/, int action, int /*mods*/) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(w, GLFW_TRUE);
        }

        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            isPaused = !isPaused;
        }

        });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* /*window*/, int width, int height) {
        std::cout << "Yangi Oyna O'lchami: (w: " << width << " h: " << height << ")" << std::endl;
        });

    double lastTime = glfwGetTime();
    double fpsTimer = lastTime;
    int frameCount = 0;

    double mouseX, mouseY;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwGetCursorPos(window, &mouseX, &mouseY);

        double now = glfwGetTime();
        double deltaTime = now - lastTime;
        lastTime = now;


        frameCount++;
        if (now - fpsTimer >= 0.5) {
            std::string title = "Fluid Sim Tutorial | FPS : " + std::to_string(frameCount / (now - fpsTimer)) + " | kadr vaqti: " + std::to_string(deltaTime * 1000.0) + " ms"
                + " | mouse-(x: " + std::to_string(mouseX) + ", y: " + std::to_string(mouseY) + ")";
            if (isPaused) {
                title.append("  | PAUSED");
            }
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            fpsTimer = now;
        }


    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}