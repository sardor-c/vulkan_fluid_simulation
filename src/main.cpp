#include <GLFW/glfw3.h>

#include <iostream>
#include <string>


bool isPaused = false;

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW ishga tushmadi!" << std::endl;
        return 1;
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

    double distance = 0.0;

    while (!glfwWindowShouldClose(window)) {
        //glfwPollEvents();
        glfwWaitEvents();
        glfwGetCursorPos(window, &mouseX, &mouseY);

        double now = glfwGetTime();
        double deltaTime = now - lastTime;
        lastTime = now;

        distance += 1.0;

        frameCount++;
        if (now - fpsTimer >= 0.5) {
            std::string title = "Fluid Sim Tutorial | FPS : " + std::to_string(frameCount) + " | kadr vaqti: " + std::to_string(deltaTime * 1000.0) + " ms"
                + " | mouse-(x: " + std::to_string(mouseX) + ", y: " + std::to_string(mouseY) + ")" + "(distance: " + std::to_string(distance) + ")";
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