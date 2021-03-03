#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imGui/imgui.h"
#include "imGui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"

// Screen settings
const unsigned int SCR_WIDTH = 1600/1.5;
const unsigned int SCR_HEIGHT = 1200/1.5;

// Camera
Camera camera(glm::vec3(-0.8f, 0.0f, 4.5f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

int main() {
    // Initializing render context and OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shader Evaluator", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0,0, SCR_WIDTH, SCR_HEIGHT);

    glPointSize(4.0f);
    glLineWidth(3.0f);

    // Turn on tu enable camera movement
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    // ImGui setup
    // -----------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoNav;

    const char* items[] = { "Lambertian", "Phong", "Blinn-Phong", "Oren-Nayar"};
    static int currentShader = 0;
    // -----------

    // Sphere resolution
    int resolution[2] = {16, 32};
    int prevResolution[2] = {16, 32};

    Mesh light = generateSphere(0.05);
    Mesh plane = generatePlane(100);
    Mesh sphere = generateSphere(1, resolution[0], resolution[1]);

    // Lights
    Shader lightShader("shaders/lightVert.glsl", "shaders/lightFrag.glsl");

    // Lambert
    Shader lambert("shaders/lambertV.glsl", "shaders/lambertF.glsl");
    glm::vec3 diffuseColor = glm::vec3(0.6f, 0.3f, 0.7f);
    float albedo = 0.8f;

    glm::vec3 light1Diffuse(1.0f, 1.0f, 1.0f);
    glm::vec3 light2Diffuse(1.0f, 1.0f, 1.0f);

    // Phong and Blinn-Phong
    Shader phong("shaders/phongV.glsl", "shaders/phongF.glsl");
    Shader blinnPhong("shaders/blinnPhongV.glsl", "shaders/blinnPhongF.glsl");

    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 ambient = glm::vec3(0.2f, 0.1f, 0.3f);
    glm::vec3 diffuse = glm::vec3(0.6f, 0.3f, 0.7f);
    float shininess = 32;

    auto specularIntensity1 = glm::vec3(1.0f);
    auto diffuseIntensity1 = glm::vec3(1.0f);
    auto ambientIntensity1 = glm::vec3(0.1f);

    auto specularIntensity2 = glm::vec3(1.0f);
    auto diffuseIntensity2 = glm::vec3(1.0f);
    auto ambientIntensity2 = glm::vec3(0.1f);

    // Oren-Nayar
    Shader orenNayar("shaders/orenNayarV.glsl", "shaders/orenNayarF.glsl");
    float roughness = 0;

    // In application settings
    bool showGui = true;
    bool rotateLights = false;
    bool showLights = false;
    int renderStyle = GL_TRIANGLES;
    int smoothInterp = true;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 model = glm::mat4(1.0f);

        // Rendering lights
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        glm::vec4 light1Pos(-2.2f, -0.5f, 4.0f, 1.0f);
        glm::vec4 light2Pos(2.4f, 2.4f, -1.8f, 1.0f);

        if (rotateLights) {
            model = glm::rotate(model, currentFrame, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(light1Pos));
            light1Pos = model * light1Pos;
            lightShader.setMat4("model", model);
            if (showLights)
                light.draw(lightShader, GL_TRIANGLES);

            model = glm::mat4(1.0f);
            model = glm::rotate(model, currentFrame, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(light2Pos));
            light2Pos = model * light2Pos;
            lightShader.setMat4("model", model);
            if (showLights)
                light.draw(lightShader, GL_TRIANGLES);

        } else {
            model = glm::translate(model, glm::vec3(light1Pos));
            lightShader.setMat4("model", model);
            if (showLights)
                light.draw(lightShader, GL_TRIANGLES);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(light2Pos));
            lightShader.setMat4("model", model);
            if (showLights)
                light.draw(lightShader, GL_TRIANGLES);
        }

        if (resolution[0] != prevResolution[0] || resolution[1] != prevResolution[1]) {
            sphere = generateSphere(1, resolution[0], resolution[1]);
            prevResolution[0] = resolution[0];
            prevResolution[1] = resolution[1];
        }

        if (currentShader == 0) {
            lambert.use();
            lambert.setBool("interpolation", smoothInterp);
            lambert.setVec3("lights[0].position", glm::vec3(light1Pos));
            lambert.setVec3("lights[0].color", light1Diffuse);
            lambert.setVec3("lights[1].position", glm::vec3(light2Pos));
            lambert.setVec3("lights[1].color", light2Diffuse);

            lambert.setVec3("material.color", diffuseColor);
            lambert.setFloat("material.albedo", albedo);

            model = glm::mat4(1.0f);

            lambert.setMat4("projection", projection);
            lambert.setMat4("view", view);
            lambert.setMat4("model", model);

            sphere.draw(lambert, renderStyle);


        } else if (currentShader == 1) {
            phong.use();
            phong.setBool("interpolation", smoothInterp);

            phong.setVec3("lights[0].position", light1Pos);
            phong.setVec3("lights[0].specularIntensity", specularIntensity1);
            phong.setVec3("lights[0].diffuseIntensity", light1Diffuse);
            phong.setVec3("lights[0].ambientIntensity", ambientIntensity1);

            phong.setVec3("lights[1].position", light2Pos);
            phong.setVec3("lights[1].specularIntensity", specularIntensity2);
            phong.setVec3("lights[1].diffuseIntensity", light2Diffuse);
            phong.setVec3("lights[1].ambientIntensity", ambientIntensity2);

            phong.setVec3("material.specularReflection", specular);
            phong.setVec3("material.diffuseReflection", diffuse);
            phong.setVec3("material.ambientReflection", ambient);
            phong.setFloat("material.shininess", shininess);

            model = glm::mat4(1.0f);

            phong.setVec3("CameraPos", camera.position);
            phong.setMat4("projection", projection);
            phong.setMat4("view", view);
            phong.setMat4("model", model);

            sphere.draw(phong, renderStyle);

        } else if (currentShader == 2) {
            blinnPhong.use();
            blinnPhong.setBool("interpolation", smoothInterp);

            blinnPhong.setVec3("lights[0].position", light1Pos);
            blinnPhong.setVec3("lights[0].specularIntensity", specularIntensity1);
            blinnPhong.setVec3("lights[0].diffuseIntensity", light1Diffuse);
            blinnPhong.setVec3("lights[0].ambientIntensity", ambientIntensity1);

            blinnPhong.setVec3("lights[1].position", light2Pos);
            blinnPhong.setVec3("lights[1].specularIntensity", specularIntensity2);
            blinnPhong.setVec3("lights[1].diffuseIntensity", light2Diffuse);
            blinnPhong.setVec3("lights[1].ambientIntensity", ambientIntensity2);

            blinnPhong.setVec3("material.specularReflection", specular);
            blinnPhong.setVec3("material.diffuseReflection", diffuse);
            blinnPhong.setVec3("material.ambientReflection", ambient);
            blinnPhong.setFloat("material.shininess", shininess);

            model = glm::mat4(1.0f);

            blinnPhong.setVec3("CameraPos", camera.position);
            blinnPhong.setMat4("projection", projection);
            blinnPhong.setMat4("view", view);
            blinnPhong.setMat4("model", model);

            sphere.draw(blinnPhong, renderStyle);
        } else if (currentShader == 3) {
            orenNayar.use();
            orenNayar.setBool("interpolation", smoothInterp);

            orenNayar.setVec3("lights[0].position", light1Pos);
            orenNayar.setVec3("lights[0].intensity", light1Diffuse);

            orenNayar.setVec3("lights[1].position", light2Pos);
            orenNayar.setVec3("lights[1].intensity", light2Diffuse);

            orenNayar.setFloat("material.albedo", albedo);
            orenNayar.setFloat("material.roughness", roughness);

            model = glm::mat4(1.0f);

            orenNayar.setVec3("CameraPos", camera.position);
            orenNayar.setMat4("projection", projection);
            orenNayar.setMat4("view", view);
            orenNayar.setMat4("model", model);

            sphere.draw(orenNayar, renderStyle);
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::GetStyle().WindowBorderSize = 0.0;

        //ImGui::ShowDemoWindow(&showGui);

        if (showGui) {
            ImGui::Begin("Shading", &showGui, window_flags);
            ImGui::Text("SHADING MODEL:");
            ImGui::Combo("", &currentShader, items, IM_ARRAYSIZE(items));
            ImGui::Separator();
            ImGui::Text("SCENE SETTINGS:");
            ImGui::Text("Sphere resolution:");
            ImGui::DragInt2("Rings, Segments", resolution, 1, 3, 128);
            ImGui::Text("Interpolation:"); ImGui::SameLine();
            ImGui::RadioButton("Flatt", &smoothInterp, 0); ImGui::SameLine();
            ImGui::RadioButton("Smooth", &smoothInterp, 1);
            ImGui::Text("Mesh display:"); ImGui::SameLine();
            ImGui::RadioButton("Triangles", &renderStyle, GL_TRIANGLES); ImGui::SameLine();
            ImGui::RadioButton("Lines", &renderStyle, GL_LINES); ImGui::SameLine();
            ImGui::RadioButton("Points", &renderStyle, GL_POINTS);
            ImGui::Checkbox("Rotate lights", &rotateLights); ImGui::SameLine();
            ImGui::Checkbox("Show lights", &showLights);

            ImGui::Separator();
            ImGui::Text("SHADER SETTINGS:");
            if (currentShader == 0 || currentShader == 3) {
                ImGui::ColorEdit3("Light 1", glm::value_ptr(light1Diffuse));
                ImGui::ColorEdit3("Light 2", glm::value_ptr(light2Diffuse));
            } else if (currentShader == 1 || currentShader == 2) {
                ImGui::ColorEdit3("Light 1 diffuse", glm::value_ptr(light1Diffuse));
                ImGui::ColorEdit3("Light 1 specular", glm::value_ptr(specularIntensity1));
                ImGui::ColorEdit3("Light 1 ambient", glm::value_ptr(ambientIntensity1));
                ImGui::Text("");

                ImGui::ColorEdit3("Light 2 diffuse", glm::value_ptr(light2Diffuse));
                ImGui::ColorEdit3("Light 2 specular", glm::value_ptr(specularIntensity2));
                ImGui::ColorEdit3("Light 2 ambient", glm::value_ptr(ambientIntensity2));
            }
            ImGui::Separator();
            if (currentShader == 0) {
                ImGui::SliderFloat("Albedo", &albedo, 0.0f, 1.0f);
                ImGui::ColorEdit3("Diffuse color", glm::value_ptr(diffuseColor));
            } else if (currentShader == 1 || currentShader == 2) {
                ImGui::ColorEdit3("Diffuse color", glm::value_ptr(diffuse));
                ImGui::ColorEdit3("Specular color", glm::value_ptr(specular));
                ImGui::ColorEdit3("Ambient color", glm::value_ptr(ambient));
                ImGui::SliderFloat("Shininess", &shininess, 1.0f, 128.0f);
            } else if (currentShader == 3) {
                ImGui::SliderFloat("Albedo", &albedo, 0.0f, 1.0f);
                ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


