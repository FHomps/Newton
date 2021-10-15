#include "stdafx.h"
#include "FPSLimiter.h"
#include <array>

#define ZOOM_DAMPENER 30

using glm::ivec2, glm::dvec2;

template <class T> std::ostream& operator<<(std::ostream& os, glm::vec<2, T> const& vec) {
    return os << vec.x << ' ' << vec.y;
}

struct SharedData {
    ivec2 viewport{ 1400, 600 };
    GLint loc_viewport;

    double scale{ 1.f };
    GLint loc_scale;

    dvec2 origin{ 0., 0. };
    GLint loc_origin;
};

inline SharedData* getSharedData(GLFWwindow* window) {
    return (SharedData*)glfwGetWindowUserPointer(window);
}

dvec2 getCursorGraphCoords(GLFWwindow* window) {
    auto d = getSharedData(window);
    dvec2 c;
    glfwGetCursorPos(window, &c.x, &c.y);
    c = (c - (dvec2)d->viewport / 2.) / (double)d->viewport.y * 2.;
    c.y = -c.y;
    c = c * d->scale + d->origin;
    return c;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto d = getSharedData(window);
    glViewport(0, 0, width, height);
    glUniform2i(d->loc_viewport, d->viewport.x = width, d->viewport.y = height);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto d = getSharedData(window);
    
    double zoom = -yoffset / 15.0;
    d->origin = d->origin + zoom * (d->origin - getCursorGraphCoords(window));
    glUniform1d(d->loc_scale, d->scale = d->scale * (1 + zoom));
    glUniform2d(d->loc_origin, d->origin.x, d->origin.y);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    SharedData d;

    GLFWwindow* window = glfwCreateWindow(d.viewport.x, d.viewport.y, "Main Window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float vertices[] = {
        //position       //colors      
       -1.f, -1.f, 0.0f, 1.f, 0.f, 0.f,
        1.f, -1.f, 0.0f, 0.f, 1.f, 0.f,
       -1.f,  1.f, 0.0f, 0.f, 0.f, 1.f,
        1.f,  1.f, 0.0f, 1.f, 1.f, 1.f
    };

    uint indices[] = {
        0, 1, 2,
        2, 1, 3
    };

    uint VAO;
    glGenVertexArrays(1, &VAO);

    uint VBO;
    glGenBuffers(1, &VBO);

    uint EBO;
    glGenBuffers(1, &EBO);

    ShaderProgram shaderProgram("shaders/default.vert", "shaders/default.frag");

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram.ID);


    d.loc_viewport = glGetUniformLocation(shaderProgram.ID, "viewport");
    glUniform2i(d.loc_viewport, d.viewport.x, d.viewport.y);

    d.loc_scale = glGetUniformLocation(shaderProgram.ID, "scale");
    glUniform1d(d.loc_scale, d.scale);

    d.loc_origin = glGetUniformLocation(shaderProgram.ID, "origin");
    glUniform2d(d.loc_origin, d.origin.x, d.origin.y);

    glfwSetWindowUserPointer(window, (void*)&d);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetScrollCallback(window, scrollCallback);

    FPSLimiter limiter(60);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram.ID);
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        limiter.update();
    }

    glfwTerminate();
    return 0;
}