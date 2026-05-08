#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <nfd.hpp>
#include <algorithm>
#include <filesystem>
#include <fstream>

#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "mesh.h"
#include "glm/gtc/matrix_transform.hpp"


Camera camera;
bool   lmbHeld  = false;
double lastX    = 0.0;
double lastY    = 0.0;
int    winWidth  = 1280;
int    winHeight = 720;
std::string modelPath;

void OnLmbClicked(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        lmbHeld = (action == GLFW_PRESS);
    }
}

void OnCursorMove(GLFWwindow* window, double x, double y) {
    float newX = x - lastX;
    float newY = y - lastY;
    lastX = x; lastY = y;

    if (lmbHeld) {
        camera.RotateCamera(newX, newY);
    }
}

void OnMouseScroll(GLFWwindow* window, double xOffset, double yOffset) {
    camera.ZoomCamera((float)yOffset);
}

std::string SelectFileFromExplorer() {
    NFD::UniquePath selectedPath;

    nfdfilteritem_t filters[] = {
        { "3D Models", "fbx" }
    };

    nfdresult_t result = NFD::OpenDialog(selectedPath, filters, 1);

    if (result == NFD_OKAY && selectedPath) {
        return selectedPath.get();
    }

    if (result == NFD_CANCEL) {
        std::cout << "No file selected." << std::endl;
    } else {
        std::cout << "File dialog error: " << NFD::GetError() << std::endl;
    }

    return "";
}

void ImguiRenderLoop() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Opena an FBX model");

    if (ImGui::Button("Open File", ImVec2(90,30)))
    {

        modelPath = SelectFileFromExplorer();

        //Checking if the selected file is FBX
        std::filesystem::path path(modelPath);
        std::string ext = path.extension().string();

        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext != ".fbx") {
            modelPath = "";
            std::cout << "Please select an FBX file." << std::endl;
        }

        std::cout << "FBX file selected: " << modelPath << std::endl;
    }

    ImGui::End();
    ImGui::Render();
}

static std::string readFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "ERROR: can't open shader file: " << path << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static unsigned int compileShader(GLenum type, const char* src) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cout << "ERROR compiling " << ":\n" << log << std::endl;
    }
    return shader;
}

void DrawMesh(const Mesh& mesh, bool isWireframe, GLuint wireframeLoc) {
    glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);

    if (!isWireframe) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 2.0f);
    } else {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    glUniform1i(wireframeLoc, isWireframe);
    mesh.draw();

    if (!isWireframe)
        glDisable(GL_POLYGON_OFFSET_FILL);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "FBX Renderer", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, OnLmbClicked);
    glfwSetCursorPosCallback(window, OnCursorMove);
    glfwSetScrollCallback(window, OnMouseScroll);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);

    Mesh mesh;
    bool meshLoaded = false;

    glm::vec3 center  = (mesh.boundsMin + mesh.boundsMax) * 0.5f;
    glm::vec3 extents =  mesh.boundsMax - mesh.boundsMin;
    float     maxDim  = std::max({extents.x, extents.y, extents.z});
    float     scale   = (maxDim > 0.0001f) ? 2.0f / maxDim : 1.0f;

    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale)) * glm::translate(glm::mat4(1.0f), -center);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

    std::string vertSrc = readFile("shaders/vert.shader");
    std::string fragSrc = readFile("shaders/frag.shader");

    unsigned int vert = compileShader(GL_VERTEX_SHADER,   vertSrc.c_str());
    unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragSrc.c_str());

    unsigned progID = glCreateProgram();
    glAttachShader(progID, vert);
    glAttachShader(progID, frag);
    glLinkProgram(progID);


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImguiRenderLoop();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        if (!modelPath.empty() && !meshLoaded) {
            if (mesh.LoadFromFile(modelPath)) {
                meshLoaded = true;
            } else {
                std::cout << "Failed to load mesh.\n";
                modelPath = "";
            }
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = (winHeight > 0)? (float)w / (float)h : 1.0f;

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (meshLoaded) {
            glUseProgram(progID);

            int w, h;
            glfwGetFramebufferSize(window, &w, &h);

            glm::mat4 view = camera.getViewMatrix();

            glm::mat4 projection = camera.getProjectionMatrix(aspect);

            GLuint modelLoc = glGetUniformLocation(progID, "model");
            GLuint viewLoc  = glGetUniformLocation(progID, "view");
            GLuint projLoc  = glGetUniformLocation(progID, "projection");
            GLuint wireframe = glGetUniformLocation(progID, "wireframe");

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);
            glUniformMatrix4fv(viewLoc,  1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(projLoc,  1, GL_FALSE, &projection[0][0]);

            DrawMesh(mesh, false, wireframe);
            DrawMesh(mesh, true, wireframe);
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
