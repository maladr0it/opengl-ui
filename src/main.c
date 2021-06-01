#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include "utils.h"
#include "ui.h"
#include "v3.h"
#include "mat4x4.h"

static const int WINDOW_WIDTH = 400;
static const int WINDOW_HEIGHT = 400;

mat4x4_t createUITransform(void)
{
    // translate to viewspace ie x [-1, 1], y [-1, 1]
    // flip y direction here
    mat4x4_t result = mat4x4_createIdentity();
    result = mat4x4_mul(result, mat4x4_createTranslate(v3_create(-1.0f, +1.0f, 0.0f)));
    result = mat4x4_mul(result, mat4x4_createScale(v3_create(2.0f / (float)WINDOW_WIDTH, -2.0f / (float)WINDOW_HEIGHT, 1.0f)));

    return result;
}

static void handleWindowResize(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void handleInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(void)
{
    //
    // Create window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL UI", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, handleWindowResize);

    //
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        exit(EXIT_FAILURE);
    };

    //
    // load shaders
    int success;
    char infoLog[512];

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char *vertexShaderSource = utils_getFileContent("./src/shaders/tri.vs");
    glShaderSource(vertexShader, 1, (const char *const *)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
        printf("%s", infoLog);
        exit(EXIT_FAILURE);
    }
    free(vertexShaderSource);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char *fragmentShaderSource = utils_getFileContent("./src/shaders/tri.fs");
    glShaderSource(fragmentShader, 1, (const char *const *)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        printf("%s", infoLog);
        exit(EXIT_FAILURE);
    }
    free(fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        printf("%s", infoLog);
        exit(EXIT_FAILURE);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //
    // create data and configure vertex attributes
    ui_vert_t *verts = createTestVerts();

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(*verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*verts), (void *)offsetof(ui_vert_t, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(*verts), (void *)offsetof(ui_vert_t, color));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        // Handle events
        handleInput(window);

        //
        // Render
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        // create ui space -> view space transform
        mat4x4_t uiTransform = createUITransform();
        glUseProgram(shaderProgram);
        int uniformLocation = glGetUniformLocation(shaderProgram, "uiTransform");
        glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, (float *)(uiTransform.m));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //
    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}