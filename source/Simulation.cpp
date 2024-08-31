#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Fabric.h"

/*  Code modified from learnopengl.com
 *  Zoe Johnston, 2024
 */

float x_angle = 30.0;
float y_angle = 290.0;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int WIDTH = 30;

glm::vec3 lightPos(0.0f, 40.0f, 20.0f);

/*  Vertex shader
 */
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "   Normal = aNormal;\n"
    "}\0";

/*  Fragment shader
 */
const char* fragmentShaderSource = "#version 330 core\n"
    "in vec3 FragPos;\n"
    "in vec3 Normal;\n"
    "out vec4 FragColor;\n"
    "uniform vec3 lightPos;\n"
    "void main()\n"
    "{\n"
    "    vec3 norm = normalize(Normal);\n"
    "    vec3 lightDir = normalize(lightPos - FragPos);\n"
    "    float diff = max(dot(norm, lightDir), 0.0);\n"
    "    vec3 ambient = vec3(0.4f, 0.0f, 0.4f);\n"
    "    vec3 diffuse = diff * vec3(1.0f, 1.0f, 0.7f);\n"
    "    vec3 result = (ambient + diffuse) * vec3(1.0f, 0.8f, 0.8f);\n"
    "    FragColor = vec4(result, 1.0);\n"
    "}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void createAndLinkProgram(unsigned int* program);

/*  Main
 */
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

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

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram;
    createAndLinkProgram(&shaderProgram);

    // Vertex data
    unsigned int vertices_size = WIDTH * WIDTH * 6;
    unsigned int indices_size = (WIDTH - 1) * WIDTH * 4;

    double *vertices = new double[vertices_size];
    unsigned int *indices = new unsigned int[indices_size];
    Fabric* sheet = new Fabric(WIDTH, WIDTH, vertices, indices);
    sheet->fix(7, 7);
    sheet->fix(17, 5);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * vertices_size, vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices_size, indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(double), (void*)(3 * sizeof(double)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glLineWidth(5.0);

        unsigned int ptr;
        ptr = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3fv(ptr, 1, glm::value_ptr(lightPos));

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -50.0f));

        ptr = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(ptr, 1, GL_FALSE, glm::value_ptr(projection));
        ptr = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(ptr, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(x_angle), glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, glm::radians(y_angle), glm::vec3(0.0, 1.0, 0.0));
        model = glm::translate(model, glm::vec3(-14.5f, 10.0f, -14.5f));

        ptr = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(ptr, 1, GL_FALSE, glm::value_ptr(model));

        // Steps forward the animation
        sheet->step();

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(double) * vertices_size, vertices);

        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, indices_size, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    delete[] vertices;
    delete[] indices;

    glfwTerminate();
	return 0;
}

/*  Takes a GLFWwindow as its first argument and two integers indicating the new window dimensions
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

/*  Checks whether the user has pressed the escape key
 */
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        x_angle -= 0.5;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        x_angle += 0.5;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        y_angle -= 0.5;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        y_angle += 0.5;
}

/*  Compiles the vertex and fragment shaders, then links them to a shader program object
 */
void createAndLinkProgram(unsigned int* program) {
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    *program = shaderProgram;
}
