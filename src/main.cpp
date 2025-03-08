#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include "camera.h"
#include "shader.h"
#include "portal.h"

// Window dimensions
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool flightMode = true;
float gravity = 5.0f;
float verticalVelocity = 0.0f;
float jumpSpeed = 5.0f;
float groundLevel = 0.0f; // Y position of the ground
float nonEuclideanFactor = 1.0f; // Controls the strength of non-Euclidean effects

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Player previous position (for portal crossing detection)
glm::vec3 prevPosition(0.0f);

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, std::vector<Portal*>& portals);
unsigned int createCube(std::vector<float>& vertices);
unsigned int createPlane(std::vector<float>& vertices, float size);
void renderScene(const glm::mat4& view, const glm::mat4& projection, Shader& shader,
    unsigned int planeVAO, unsigned int cubeVAO, const glm::vec3& portalAOffset,
    const glm::vec3& portalBOffset, float time, bool applyNonEuclidean = true);
void renderPortals(std::vector<Portal*>& portals, const glm::mat4& projection,
    Shader& portalShader, Shader& devShader, unsigned int planeVAO,
    unsigned int cubeVAO, float time);

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Endless - Non-Euclidean Space", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Enable experimental features
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Print OpenGL version information
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // Set the initial viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Build and compile shader programs
    Shader portalShader("v_portal.glsl", "f_portal.glsl");
    Shader devShader("v_basic.glsl", "f_dev.glsl");
    Shader psychShader("v_warping.glsl", "f_psychedelic_dev.glsl");
    //Shader frameShader("v_basic.glsl", "f_portal_frame.glsl");

    // Set up vertex data
    std::vector<float> cubeVertices;
    unsigned int cubeVAO = createCube(cubeVertices);

    std::vector<float> planeVertices;
    unsigned int planeVAO = createPlane(planeVertices, 50.0f);

    // Define the two non-Euclidean spaces
    glm::vec3 portalAOffset(0.0f, 0.0f, 0.0f);
    glm::vec3 portalBOffset(20.0f, 0.0f, 0.0f);

    // Initialize portals
    std::vector<Portal*> portals;

    // Create Portal A: Blue portal in first area
    Portal* portalA = new Portal(
        portalAOffset + glm::vec3(5.0f, 1.5f, 0.0f),  // Position
        glm::vec3(1.0f, 0.0f, 0.0f),                  // Normal (facing +X)
        glm::vec3(0.0f, 1.0f, 0.0f),                  // Up
        2.5f, 4.0f,                                    // Width, Height
        glm::vec4(0.0f, 0.4f, 0.8f, 0.7f),            // Blue edge color (more transparent)
        SCR_WIDTH, SCR_HEIGHT,
        0.2f,                                          // Scale effect - dramatic shrinking (1/3 original size)
        glm::vec3(0.0f, glm::radians(5.0f), 0.0f)     // Slight Y-axis rotation
    );

    // Create Portal B: Orange portal in second area with dramatic scaling effect (enlarging)
    Portal* portalB = new Portal(
        portalBOffset + glm::vec3(-5.0f, 1.5f, 0.0f), // Position
        glm::vec3(-1.0f, 0.0f, 0.0f),                 // Normal (facing -X)
        glm::vec3(0.0f, 1.0f, 0.0f),                  // Up
        2.5f, 4.0f,                                    // Width, Height
        glm::vec4(1.0f, 0.5f, 0.0f, 0.7f),            // Orange edge color (more transparent)
        SCR_WIDTH, SCR_HEIGHT,
        5.0f,                                          // Scale effect - dramatic enlarging (3x original size)
        glm::vec3(0.0f, glm::radians(-5.0f), 0.0f)    // Slight Y-axis rotation in opposite direction
    );

    Portal* portalC = new Portal(
        portalAOffset + glm::vec3(0.0f, 1.5f, 8.0f),  // Position
        glm::vec3(0.0f, 0.0f, -1.0f),                 // Normal (facing -Z)
        glm::vec3(0.0f, 1.0f, 0.0f),                  // Up
        2.5f, 4.0f,                                   // Width, Height
        glm::vec4(0.5f, 0.0f, 0.5f, 0.7f),           // Purple edge color
        SCR_WIDTH, SCR_HEIGHT,
        1.0f,                                         // No scale change
        glm::vec3(glm::radians(90.0f), 0.0f, 0.0f)   // Dramatic 90-degree flip on X axis
    );

    Portal* portalD = new Portal(
        portalBOffset + glm::vec3(0.0f, 1.5f, -8.0f), // Position
        glm::vec3(0.0f, 0.0f, 1.0f),                 // Normal (facing +Z)
        glm::vec3(0.0f, 1.0f, 0.0f),                 // Up
        2.5f, 4.0f,                                   // Width, Height
        glm::vec4(0.5f, 0.0f, 0.5f, 0.7f),           // Matching purple edge color
        SCR_WIDTH, SCR_HEIGHT,
        1.0f,                                         // No scale change
        glm::vec3(0.0f, glm::radians(180.0f), 0.0f)  // 180-degree flip on Y axis (complete reversal)
    );

    // Link portals bidirectionally
    Portal::linkPortals(portalA, portalB);
    Portal::linkPortals(portalC, portalD);

    // Add portals to vector
    portals.push_back(portalA);
    portals.push_back(portalB);
    portals.push_back(portalC);
    portals.push_back(portalD);

    // Store initial camera position for portal detection
    prevPosition = camera.Position;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input and check for portal crossing
        processInput(window, portals);

        // Create projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);

        // Render portals (with view from other side)
        renderPortals(portals, projection, portalShader, psychShader, planeVAO, cubeVAO, currentFrame);
        // Clear main framebuffer
        glClearColor(0.03f, 0.03f, 0.05f, 1.0f);  // Very dark blue/purple background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get view matrix for main camera
        glm::mat4 view = camera.GetViewMatrix();

        // Render the scene from main camera view
        renderScene(view, projection, psychShader, planeVAO, cubeVAO, portalAOffset, portalBOffset, currentFrame, nonEuclideanFactor > 0.0f);

        // Render portal surfaces with their textures
        portalShader.use();
        portalShader.setMat4("projection", projection);
        portalShader.setMat4("view", view);
        portalShader.setVec3("viewPos", camera.Position);
        portalShader.setFloat("time", currentFrame);

        for (const auto& portal : portals) {
            // Set model matrix for this portal
            glm::mat4 model = glm::mat4(1.0f);
            portalShader.setMat4("model", model);

            // Set portal edge color
            portalShader.setVec4("edgeColor", portal->edgeColor);

            // Bind the portal texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, portal->getTextureID());
            portalShader.setInt("portalTexture", 0);

            // Render portal surface
            glBindVertexArray(portal->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, portal->getVertexCount());

            // Render portal frame
            //frameShader.use();
            //frameShader.setMat4("projection", projection);
            //frameShader.setMat4("view", view);
            //frameShader.setVec3("viewPos", camera.Position);
            //frameShader.setFloat("time", currentFrame);
            //portal->renderPortalFrame(frameShader, currentFrame);

            // Switch back to portal shader for next portal
            portalShader.use();
        }

        // Store current position for next frame's portal detection
        prevPosition = camera.Position;

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    for (auto portal : portals) {
        delete portal;
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glfwTerminate();
    return 0;
}

// Process keyboard input and check for portal crossings
void processInput(GLFWwindow* window, std::vector<Portal*>& portals) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Toggle flight mode when F key is pressed
    static bool fKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!fKeyPressed) {
            flightMode = !flightMode;
            verticalVelocity = 0.0f; // Reset vertical velocity when toggling
            fKeyPressed = true;

            // Print current mode
            std::cout << "Mode: " << (flightMode ? "Flying" : "Walking") << std::endl;
        }
    }
    else {
        fKeyPressed = false;
    }

    static bool nKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        if (!nKeyPressed) {
            nonEuclideanFactor = nonEuclideanFactor > 0.0f ? 0.0f : 1.0f;
            nKeyPressed = true;

            // Print current mode
            std::cout << "Non-Euclidean Effects: " << (nonEuclideanFactor > 0.0f ? "ON" : "OFF") << std::endl;
        }
    }
    else {
        nKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        nonEuclideanFactor += 0.05f;
        nonEuclideanFactor = glm::min(nonEuclideanFactor, 2.0f);
        std::cout << "Non-Euclidean Factor: " << nonEuclideanFactor << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        nonEuclideanFactor -= 0.05f;
        nonEuclideanFactor = glm::max(nonEuclideanFactor, 0.0f);
        std::cout << "Non-Euclidean Factor: " << nonEuclideanFactor << std::endl;
    }

    // Store current position before movement
    glm::vec3 preMovementPos = camera.Position;

    // Horizontal movement - same for both modes
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Vertical movement (depends on mode)
    if (flightMode) {
        // Flight mode - direct control
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);
    }
    else {
        // Walking mode - gravity and jumping

        // Apply gravity
        verticalVelocity -= gravity * deltaTime;

        // Update position based on vertical velocity
        camera.Position.y += verticalVelocity * deltaTime;

        // Check for ground collision
        if (camera.Position.y < groundLevel + 1.0f) { // 1.0f is camera height
            camera.Position.y = groundLevel + 1.0f;
            verticalVelocity = 0.0f;

            // Jump when on ground
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                verticalVelocity = jumpSpeed;
            }
        }
    }

    // Check for portal crossings
    for (auto portal : portals) {
        if (portal->isCrossing(preMovementPos, camera.Position)) {
            // Transform camera through the portal
            portal->transformCamera(camera);

            // Update prevPosition to avoid repeated teleportations
            prevPosition = camera.Position;
            break; // Only cross one portal per frame
        }
    }
}

// Handle window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Mouse movement callback
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Mouse scroll callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// Create a cube with normals and texture coordinates
unsigned int createCube(std::vector<float>& vertices) {
    // Define cube vertices (position, normal, tex coords)
    vertices = {
        // positions          // normals           // texture coords
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        // Left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        // Right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         // Bottom face
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

         // Top face
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return VAO;
}

// Create a ground plane
unsigned int createPlane(std::vector<float>& vertices, float size) {
    float halfSize = size / 2.0f;

    // Define plane vertices (position, normal, tex coords)
    vertices = {
        // positions            // normals           // texture coords
        -halfSize, 0.0f, -halfSize,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         halfSize, 0.0f, -halfSize,  0.0f, 1.0f, 0.0f,  size, 0.0f,
         halfSize, 0.0f,  halfSize,  0.0f, 1.0f, 0.0f,  size, size,

         halfSize, 0.0f,  halfSize,  0.0f, 1.0f, 0.0f,  size, size,
        -halfSize, 0.0f,  halfSize,  0.0f, 1.0f, 0.0f,  0.0f, size,
        -halfSize, 0.0f, -halfSize,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return VAO;
}

glm::vec3 applyNonEuclideanTransformation(const glm::vec3& position, float time) {
    // Distance from origin in xz plane
    float dist = glm::length(glm::vec2(position.x, position.z));

    // Base position
    glm::vec3 newPos = position;

    // 1. Space expansion/contraction: spaces become larger/smaller than they appear
    // Areas far from origin are compressed, areas near origin are expanded
    if (dist > 5.0f) {
        float compressionFactor = 1.0f - 0.1f * nonEuclideanFactor * (dist - 5.0f) / 10.0f;
        compressionFactor = glm::max(compressionFactor, 0.5f); // Limit compression

        // Apply compression to distance from origin
        glm::vec2 dirXZ = glm::normalize(glm::vec2(position.x, position.z));
        newPos.x = dirXZ.x * dist * compressionFactor;
        newPos.z = dirXZ.y * dist * compressionFactor;
    }

    // 2. Impossible spaces: inside is larger than outside
    // Create subtle spatial distortions that intensify with distance
    float warpFactor = sin(dist * 0.2f + time * 0.3f) * 0.2f * nonEuclideanFactor;
    newPos.y += warpFactor * position.y;

    // 3. Non-Euclidean corridors: turning right 4 times doesn't bring you back to start
    // Apply subtle rotation to create a hyperbolic-like space
    float angle = atan2(position.z, position.x);
    float rotAmount = sin(angle * 4.0f + time * 0.1f) * 0.05f * nonEuclideanFactor;
    float rotatedX = position.x * cos(rotAmount) - position.z * sin(rotAmount);
    float rotatedZ = position.x * sin(rotAmount) + position.z * cos(rotAmount);
    newPos.x = glm::mix(newPos.x, rotatedX, 0.5f);
    newPos.z = glm::mix(newPos.z, rotatedZ, 0.5f);

    return newPos;
}

// Render the scene with two distinct areas
void renderScene(const glm::mat4& view, const glm::mat4& projection, Shader& shader,
    unsigned int planeVAO, unsigned int cubeVAO, const glm::vec3& portalAOffset,
    const glm::vec3& portalBOffset, float time, bool applyNonEuclidean) {

    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setVec3("viewPos", camera.Position);
    shader.setFloat("time", time);

    // Render ground plane in area A - no non-Euclidean effect on ground for stability
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, portalAOffset);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Render ground plane in area B
    model = glm::mat4(1.0f);
    model = glm::translate(model, portalBOffset);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Render cubes in area A with non-Euclidean transformations
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            if (i == 0 && j == 0) continue; // Skip center

            glm::vec3 basePos = portalAOffset + glm::vec3(i * 2.0f, 0.5f, j * 2.0f);
            glm::vec3 transformedPos = basePos;

            if (applyNonEuclidean) {
                // Apply non-Euclidean transformation to object position
                transformedPos = applyNonEuclideanTransformation(basePos, time);
            }

            model = glm::mat4(1.0f);
            model = glm::translate(model, transformedPos);

            // Add some rotation based on position for more dynamic effect
            if (applyNonEuclidean) {
                float rotAngle = sin(time * 0.5f + i * 0.7f + j * 0.5f) * 20.0f * nonEuclideanFactor;
                model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            }

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Render cubes in area B - apply different non-Euclidean transformations
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            if (i == 0 && j == 0) continue; // Skip center

            glm::vec3 basePos = portalBOffset + glm::vec3(i * 2.0f, 0.5f, j * 2.0f);
            glm::vec3 transformedPos = basePos;

            if (applyNonEuclidean) {
                // Apply a different non-Euclidean transformation to create contrast
                float dist = glm::length(glm::vec2(basePos.x - portalBOffset.x, basePos.z - portalBOffset.z));

                // Spiral distortion
                float angle = atan2(basePos.z - portalBOffset.z, basePos.x - portalBOffset.x);
                angle += sin(dist * 0.5f) * 0.3f * nonEuclideanFactor;
                float newX = dist * cos(angle);
                float newZ = dist * sin(angle);

                transformedPos.x = portalBOffset.x + newX;
                transformedPos.z = portalBOffset.z + newZ;

                // Height distortion
                transformedPos.y += sin(dist * 0.8f + time * 0.6f) * 0.4f * nonEuclideanFactor;
            }

            model = glm::mat4(1.0f);
            model = glm::translate(model, transformedPos);

            // Add some distortion to the cubes themselves
            if (applyNonEuclidean) {
                float scaleX = 1.0f + sin(time * 0.3f + i * 0.6f) * 0.2f * nonEuclideanFactor;
                float scaleY = 1.0f + cos(time * 0.4f + j * 0.5f) * 0.2f * nonEuclideanFactor;
                float scaleZ = 1.0f + sin(time * 0.5f + (i + j) * 0.4f) * 0.2f * nonEuclideanFactor;
                model = glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));

                float rotAngle = cos(time * 0.4f + i * 0.5f + j * 0.3f) * 30.0f * nonEuclideanFactor;
                model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            }

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Add some walls in area A with non-Euclidean bend
    model = glm::mat4(1.0f);
    model = glm::translate(model, portalAOffset + glm::vec3(0.0f, 2.0f, -10.0f));

    if (applyNonEuclidean) {
        // Create a curved wall that should be straight in Euclidean space
        for (int i = -10; i <= 10; i++) {
            float x = i * 1.0f;

            // Calculate curved wall position
            float z = -10.0f;
            float bend = sin(x * 0.2f + time * 0.2f) * 2.0f * nonEuclideanFactor;

            glm::mat4 wallSection = glm::mat4(1.0f);
            wallSection = glm::translate(wallSection, portalAOffset + glm::vec3(x, 2.0f, z + bend));

            // Rotate to follow curve
            float rotAngle = cos(x * 0.2f + time * 0.2f) * 15.0f * nonEuclideanFactor;
            wallSection = glm::rotate(wallSection, glm::radians(rotAngle), glm::vec3(0.0f, 1.0f, 0.0f));

            wallSection = glm::scale(wallSection, glm::vec3(1.0f, 4.0f, 0.2f));
            shader.setMat4("model", wallSection);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    else {
        // Straight wall if non-Euclidean effects are disabled
        model = glm::scale(model, glm::vec3(20.0f, 4.0f, 0.2f));
        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Add some walls in area B with different non-Euclidean properties
    if (applyNonEuclidean) {
        // Create a wall that seems to fold into itself (impossible in Euclidean space)
        for (int i = -10; i <= 10; i++) {
            float x = i * 1.0f;

            // Create a wall that folds into the 4th dimension (visually)
            float z = 10.0f;
            float fold = sin(x * 0.3f + time * 0.3f) * 3.0f * nonEuclideanFactor;
            float yOffset = cos(x * 0.3f + time * 0.15f) * 1.0f * nonEuclideanFactor;

            glm::mat4 wallSection = glm::mat4(1.0f);
            wallSection = glm::translate(wallSection, portalBOffset + glm::vec3(x, 2.0f + yOffset, z - fold));

            // Create twisting effect
            float twistAngle = sin(x * 0.2f + time * 0.25f) * 40.0f * nonEuclideanFactor;
            wallSection = glm::rotate(wallSection, glm::radians(twistAngle), glm::vec3(0.0f, 0.0f, 1.0f));

            // Vary the scale to enhance the non-Euclidean feel
            float scaleY = 4.0f + sin(x * 0.4f + time * 0.2f) * 1.0f * nonEuclideanFactor;
            wallSection = glm::scale(wallSection, glm::vec3(1.0f, scaleY, 0.2f));

            shader.setMat4("model", wallSection);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    else {
        // Straight wall if non-Euclidean effects are disabled
        model = glm::mat4(1.0f);
        model = glm::translate(model, portalBOffset + glm::vec3(0.0f, 2.0f, 10.0f));
        model = glm::scale(model, glm::vec3(20.0f, 4.0f, 0.2f));
        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Add floating objects with non-Euclidean movement patterns
    float currentTime = time;
    for (int i = 0; i < 10; i++) {
        // Base values
        float angle = i * (2.0f * 3.14159f / 10.0f) + currentTime * 0.2f;
        float radius = 8.0f + sin(currentTime * 0.5f + i * 0.5f) * 2.0f;
        float height = 2.0f + sin(currentTime * 0.3f + i * 0.4f) * 1.5f;

        // Area A floating object
        glm::vec3 basePos = portalAOffset + glm::vec3(sin(angle) * radius, height, cos(angle) * radius);
        glm::vec3 transformedPos = basePos;

        if (applyNonEuclidean) {
            // Apply non-Euclidean transformation to orbit
            // This makes objects follow impossible trajectories
            float distortion = sin(i * 0.7f + currentTime * 0.4f) * nonEuclideanFactor;
            float distortedAngle = angle + distortion;

            // Klein bottle-inspired trajectory (objects seem to pass through themselves)
            if (distortedAngle > 3.14159f && distortedAngle < 2.0f * 3.14159f) {
                radius *= (1.0f - (distortedAngle - 3.14159f) / 3.14159f * 0.5f * nonEuclideanFactor);
            }

            transformedPos = portalAOffset + glm::vec3(
                sin(distortedAngle) * radius,
                height * (1.0f + cos(distortedAngle * 2.0f) * 0.3f * nonEuclideanFactor),
                cos(distortedAngle) * radius
            );
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, transformedPos);
        model = glm::rotate(model, currentTime + i,
            glm::vec3(sin(i * 0.5f), cos(i * 0.3f), sin(i * 0.7f)));
        float scale = 0.5f + sin(currentTime * 0.6f + i) * 0.2f;
        model = glm::scale(model, glm::vec3(scale));
        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Area B floating object with different non-Euclidean patterns
        basePos = portalBOffset + glm::vec3(sin(angle + 3.14159f) * radius, height * 1.2f, cos(angle + 3.14159f) * radius);
        transformedPos = basePos;

        if (applyNonEuclidean) {
            // Create hyperbolic-inspired orbits
            float loopFactor = sin(currentTime * 0.3f + i * 0.5f) * nonEuclideanFactor;

            // This creates figure-8 patterns that shouldn't be possible in normal space
            transformedPos = portalBOffset + glm::vec3(
                sin(angle * 2.0f) * radius * (0.5f + 0.5f * cos(angle)),
                height * (1.0f + sin(angle * 3.0f) * 0.4f * nonEuclideanFactor),
                cos(angle) * radius * (1.0f + loopFactor * sin(angle * 2.0f))
            );
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, transformedPos);
        model = glm::rotate(model, currentTime * 0.8f + i,
            glm::vec3(cos(i * 0.4f), sin(i * 0.6f), cos(i * 0.5f)));
        scale = 0.6f + cos(currentTime * 0.5f + i) * 0.2f;
        model = glm::scale(model, glm::vec3(scale));
        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

// Render what's visible through each portal
void renderPortals(std::vector<Portal*>& portals, const glm::mat4& projection,
    Shader& portalShader, Shader& sceneShader, unsigned int planeVAO,
    unsigned int cubeVAO, float time) {

    // For each portal, render the scene from the perspective of standing at the linked portal
    for (const auto& portal : portals) {
        if (!portal->destination) continue;

        // Only render if portal is potentially visible from current viewpoint
        if (!portal->isVisible(camera)) continue;

        // Begin rendering to this portal's framebuffer
        portal->beginPortalRender();

        // Calculate the view matrix as if looking through the portal
        glm::mat4 portalView = portal->getPortalView(camera);

        // Get potentially adjusted projection matrix for the portal view
        glm::mat4 portalProjection = portal->getPortalProjection(projection);

        // Render the scene from the portal's perspective
        renderScene(portalView, portalProjection, sceneShader, planeVAO, cubeVAO,
            glm::vec3(0.0f), glm::vec3(20.0f, 0.0f, 0.0f), time, nonEuclideanFactor > 0.0f);

        // End rendering to portal framebuffer
        portal->endPortalRender();
    }
}
