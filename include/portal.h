#ifndef PORTAL_H
#define PORTAL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

class Portal {
public:
    // Portal attributes
    glm::vec3 position;       // Center position of the portal
    glm::vec3 normal;         // Normal/facing direction
    glm::vec3 up;             // Up vector for orientation
    float width;              // Portal width
    float height;             // Portal height

    // Connection to another portal
    Portal* destination;      // Linked portal

    // Visual properties
    glm::vec4 edgeColor;      // Portal edge glow color

    // Non-Euclidean transformation properties
    float scaleEffect;        // Scale factor applied when passing through portal
    glm::vec3 rotationEffect; // Rotation applied when passing through portal

    // Framebuffer for rendering portal view
    unsigned int framebuffer;
    unsigned int textureID;
    unsigned int renderbuffer;

    // Constructor
    Portal(glm::vec3 pos, glm::vec3 norm, glm::vec3 upVec, float w, float h, glm::vec4 col,
        unsigned int screenWidth, unsigned int screenHeight,
        float scale = 1.0f, glm::vec3 rotation = glm::vec3(0.0f))
        : position(pos), normal(glm::normalize(norm)), up(glm::normalize(upVec)),
        width(w), height(h), edgeColor(col), destination(nullptr),
        scaleEffect(scale), rotationEffect(rotation) {
        // Calculate right vector for the portal plane
        right = glm::normalize(glm::cross(up, normal));
        // Re-calculate the up vector to ensure it's orthogonal
        up = glm::normalize(glm::cross(normal, right));

        // Initialize vertices for rendering
        initializeVertices();

        // Create framebuffer for portal rendering
        createFramebuffer(screenWidth, screenHeight);
    }

    ~Portal() {
        // Clean up framebuffer resources
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &textureID);
        glDeleteRenderbuffers(1, &renderbuffer);

        // Clean up frame resources
        glDeleteVertexArrays(1, &frameVAO);
        glDeleteBuffers(1, &frameVBO);
    }

    // Link this portal to another
    void linkTo(Portal* dest) {
        destination = dest;
    }

    // Create bidirectional link between portals
    static void linkPortals(Portal* a, Portal* b) {
        a->linkTo(b);
        b->linkTo(a);
    }

    // Check if camera is close enough and facing the portal
    bool isVisible(const Camera& camera, float maxDistance = 50.0f) const {
        // Check distance
        float distance = glm::length(camera.Position - position);
        if (distance > maxDistance) return false;

        // Check if portal is potentially in view (simple check - could be refined)
        glm::vec3 cameraToPortal = glm::normalize(position - camera.Position);
        float dotProduct = glm::dot(camera.Front, cameraToPortal);

        return dotProduct > 0.1f; // Camera is roughly facing toward the portal
    }

    // Check if player is potentially crossing the portal
    bool isCrossing(const glm::vec3& prevPos, const glm::vec3& newPos, float collisionRadius = 0.5f) const {
        // Check if movement line intersects portal plane
        glm::vec3 movement = newPos - prevPos;
        float dotNormalMovement = glm::dot(normal, movement);

        // If movement is parallel to portal or away from portal face
        if (dotNormalMovement >= 0.0f) return false;

        // Calculate intersection point with portal plane
        float dotNormalPrevPos = glm::dot(normal, prevPos - position);
        float t = -dotNormalPrevPos / dotNormalMovement;

        // Check if intersection happens within movement segment
        if (t < 0.0f || t > 1.0f) return false;

        // Calculate intersection point
        glm::vec3 intersection = prevPos + t * movement;

        // Check if intersection is within portal rectangle
        glm::vec3 relativePos = intersection - position;
        float rightProj = glm::dot(relativePos, right);
        float upProj = glm::dot(relativePos, up);

        return (abs(rightProj) <= width / 2 + collisionRadius) &&
            (abs(upProj) <= height / 2 + collisionRadius);
    }

    // Transform a position through the portal
    glm::vec3 transformPosition(const glm::vec3& pos) const {
        if (!destination) return pos; // No destination portal

        // Vector from this portal to the position
        glm::vec3 relativePos = pos - position;

        // Project onto portal plane vectors
        float distanceFromPlane = glm::dot(relativePos, normal);
        float rightProj = glm::dot(relativePos, right);
        float upProj = glm::dot(relativePos, up);

        // Apply scale effect to the distance components
        rightProj *= destination->scaleEffect;
        upProj *= destination->scaleEffect;
        distanceFromPlane *= destination->scaleEffect;

        // Create mirrored position relative to destination portal
        glm::vec3 newRelativePos =
            -distanceFromPlane * destination->normal +
            rightProj * destination->right +
            upProj * destination->up;

        return destination->position + newRelativePos;
    }

    // Transform the camera when going through portal
    void transformCamera(Camera& camera) const {
        if (!destination) return;

        // Store original parameters
        float originalSpeed = camera.MovementSpeed;

        // Transform position
        glm::vec3 relativePos = camera.Position - position;

        // Project onto portal plane vectors
        float distanceFromPlane = glm::dot(relativePos, normal);
        float rightProj = glm::dot(relativePos, right);
        float upProj = glm::dot(relativePos, up);

        // Apply scale effect to the distance components
        rightProj *= destination->scaleEffect;
        upProj *= destination->scaleEffect;
        distanceFromPlane *= destination->scaleEffect;

        // Create mirrored position relative to destination portal
        glm::vec3 newRelativePos =
            -distanceFromPlane * destination->normal +
            rightProj * destination->right +
            upProj * destination->up;

        // Set new position
        camera.Position = destination->position + newRelativePos;

        // Transform orientation vectors
        glm::mat3 rotMat = glm::mat3(
            destination->right,
            destination->up,
            destination->normal
        ) * glm::mat3(
            right,
            up,
            normal
        );

        // Apply additional rotation effect
        if (glm::length(destination->rotationEffect) > 0.0001f) {
            glm::mat4 additionalRot = glm::mat4(1.0f);
            additionalRot = glm::rotate(additionalRot, destination->rotationEffect.x, glm::vec3(1.0f, 0.0f, 0.0f));
            additionalRot = glm::rotate(additionalRot, destination->rotationEffect.y, glm::vec3(0.0f, 1.0f, 0.0f));
            additionalRot = glm::rotate(additionalRot, destination->rotationEffect.z, glm::vec3(0.0f, 0.0f, 1.0f));

            // Apply additional rotation to camera direction
            glm::vec4 rotatedFront = additionalRot * glm::vec4(camera.Front, 0.0f);
            glm::vec4 rotatedRight = additionalRot * glm::vec4(camera.Right, 0.0f);
            glm::vec4 rotatedUp = additionalRot * glm::vec4(camera.Up, 0.0f);

            camera.Front = glm::normalize(rotMat * glm::vec3(rotatedFront));
            camera.Right = glm::normalize(rotMat * glm::vec3(rotatedRight));
            camera.Up = glm::normalize(rotMat * glm::vec3(rotatedUp));
        }
        else {
            camera.Front = glm::normalize(rotMat * camera.Front);
            camera.Right = glm::normalize(rotMat * camera.Right);
            camera.Up = glm::normalize(rotMat * camera.Up);
        }

        // Recalculate Euler angles
        camera.Pitch = glm::degrees(asin(camera.Front.y));
        camera.Yaw = glm::degrees(atan2(camera.Front.z, camera.Front.x));

        // Instead of changing FOV, adjust the player's movement speed and collision size
        // This effectively makes the player "feel" bigger or smaller
        camera.MovementSpeed = originalSpeed * destination->scaleEffect;

        // Store scaling factor for other game systems to use
        camera.currentScale = destination->scaleEffect;

        // Print info about the transformation for debugging
        std::cout << "Portal transit: Scale factor = " << destination->scaleEffect
            << ", New movement speed = " << camera.MovementSpeed << std::endl;
    }

    // Get the view matrix for rendering the scene from the portal's perspective

    glm::mat4 getPortalView(const Camera& camera) const {
        if (!destination) return camera.GetViewMatrix();

        // Calculate camera position relative to this portal
        glm::vec3 relativeCamPos = camera.Position - position;

        // Distance from camera to portal plane
        float distanceFromPlane = glm::dot(relativeCamPos, normal);

        // Project onto portal plane vectors to get portal-space coordinates
        float rightCoord = glm::dot(relativeCamPos, right);
        float upCoord = glm::dot(relativeCamPos, up);

        // For a realistic portal view, we need to account for the destination portal's scale
        // When looking through the portal, the world on the other side should appear larger or smaller

        // Calculate new camera position relative to destination portal
        glm::vec3 newRelativeCamPos =
            -distanceFromPlane * destination->normal +
            rightCoord * destination->right +
            upCoord * destination->up;

        // Apply the scale factor to the distance from the destination portal
        // This ensures that when the player looks through a portal that makes things bigger,
        // they'll see objects as appropriately closer in the portal view
        newRelativeCamPos *= destination->scaleEffect;

        glm::vec3 newCamPos = destination->position + newRelativeCamPos;

        // Calculate new camera orientation
        glm::mat3 rotMat = glm::mat3(
            destination->right,
            destination->up,
            destination->normal
        ) * glm::mat3(
            right,
            up,
            normal
        );

        // Apply rotation effects to the view direction
        glm::vec3 newCamFront = rotMat * camera.Front;
        glm::vec3 newCamUp = rotMat * camera.Up;

        if (glm::length(destination->rotationEffect) > 0.0001f) {
            // Apply additional rotation to view direction
            glm::mat4 additionalRot = glm::mat4(1.0f);
            additionalRot = glm::rotate(additionalRot, destination->rotationEffect.x, glm::vec3(1.0f, 0.0f, 0.0f));
            additionalRot = glm::rotate(additionalRot, destination->rotationEffect.y, glm::vec3(0.0f, 1.0f, 0.0f));
            additionalRot = glm::rotate(additionalRot, destination->rotationEffect.z, glm::vec3(0.0f, 0.0f, 1.0f));

            newCamFront = glm::vec3(additionalRot * glm::vec4(newCamFront, 0.0f));
            newCamUp = glm::vec3(additionalRot * glm::vec4(newCamUp, 0.0f));
        }

        // Create view matrix - this is what determines what's visible through the portal
        return glm::lookAt(newCamPos, newCamPos + newCamFront, newCamUp);
    }

    // Begin rendering to portal framebuffer
    void beginPortalRender() const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // End rendering to portal framebuffer
    void endPortalRender() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Get the portal's texture ID
    unsigned int getTextureID() const {
        return textureID;
    }

    // Get the VAO for rendering
    unsigned int getVAO() const {
        return portalVAO;
    }

    // Get the number of vertices
    int getVertexCount() const {
        return 6; // Two triangles
    }

    // Get the frame VAO for rendering
    unsigned int getFrameVAO() const {
        return frameVAO;
    }

    // Get the number of frame vertices
    int getFrameVertexCount() const {
        return frameVertices.size() / 8; // 8 floats per vertex
    }

    // Render the portal frame
    void renderPortalFrame(Shader& frameShader, float time) const {
        frameShader.use();

        // Set uniforms for the frame shader
        frameShader.setVec4("frameColor", edgeColor);
        frameShader.setFloat("time", time);

        glm::mat4 model = glm::mat4(1.0f);
        frameShader.setMat4("model", model);

        // Render the frame
        glBindVertexArray(frameVAO);
        glDrawArrays(GL_TRIANGLES, 0, getFrameVertexCount());
    }

    glm::mat4 getPortalProjection(const glm::mat4& originalProjection) const {
        if (!destination) return originalProjection;

        // For truly accurate portal rendering, we may need to adjust the projection matrix
        // For example, if the portal makes things bigger, the near and far planes might need adjustment

        // For now, we'll use the original projection matrix
        // In a more advanced implementation, you could extract near/far planes and adjust them

        return originalProjection;
    }

private:
    glm::vec3 right;          // Right vector (perpendicular to normal and up)
    std::vector<float> vertices;  // Vertices for rendering the portal
    std::vector<float> frameVertices; // Vertices for rendering the portal frame
    unsigned int portalVAO, portalVBO;
    unsigned int frameVAO, frameVBO;

    // Initialize portal vertices and VAO/VBO
    void initializeVertices() {
        // Calculate corners
        glm::vec3 halfWidth = right * (width / 2.0f);
        glm::vec3 halfHeight = up * (height / 2.0f);

        glm::vec3 topLeft = position - halfWidth + halfHeight;
        glm::vec3 topRight = position + halfWidth + halfHeight;
        glm::vec3 bottomLeft = position - halfWidth - halfHeight;
        glm::vec3 bottomRight = position + halfWidth - halfHeight;

        // Create vertices (position, normal, texCoord)
        vertices = {
            // First triangle
            topLeft.x, topLeft.y, topLeft.z, normal.x, normal.y, normal.z, 0.0f, 1.0f,
            bottomLeft.x, bottomLeft.y, bottomLeft.z, normal.x, normal.y, normal.z, 0.0f, 0.0f,
            topRight.x, topRight.y, topRight.z, normal.x, normal.y, normal.z, 1.0f, 1.0f,

            // Second triangle
            bottomLeft.x, bottomLeft.y, bottomLeft.z, normal.x, normal.y, normal.z, 0.0f, 0.0f,
            bottomRight.x, bottomRight.y, bottomRight.z, normal.x, normal.y, normal.z, 1.0f, 0.0f,
            topRight.x, topRight.y, topRight.z, normal.x, normal.y, normal.z, 1.0f, 1.0f,
        };

        // Create VAO/VBO
        glGenVertexArrays(1, &portalVAO);
        glGenBuffers(1, &portalVBO);

        glBindVertexArray(portalVAO);

        glBindBuffer(GL_ARRAY_BUFFER, portalVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Create frame vertices for the gate-like appearance
        createFrameVertices();
    }

    void createFrameSegment(glm::vec3 start, glm::vec3 end, float thickness, float depth, bool vertical) {
        glm::vec3 direction = glm::normalize(end - start);
        glm::vec3 sideDir;

        if (vertical) {
            sideDir = glm::cross(direction, normal);
        }
        else {
            sideDir = glm::cross(normal, direction);
        }

        sideDir = glm::normalize(sideDir);

        glm::vec3 halfThickness = sideDir * (thickness / 2.0f);
        glm::vec3 halfDepth = normal * (depth / 2.0f);

        // Create a box along the line from start to end
        glm::vec3 center = (start + end) / 2.0f;
        float length = glm::length(end - start);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, center);

        // Create rotation matrix to align with segment direction
        glm::vec3 defaultDir = vertical ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
        glm::vec3 rotAxis = glm::cross(defaultDir, direction);
        float rotAngle = acos(glm::dot(defaultDir, direction));

        if (glm::length(rotAxis) > 0.0001f) {
            model = glm::rotate(model, rotAngle, glm::normalize(rotAxis));
        }

        // Set dimensions based on segment orientation
        float width, height;
        if (vertical) {
            width = thickness;
            height = length;
        }
        else {
            width = length;
            height = thickness;
        }

        // Create box with specified dimensions
        addBoxVertices(center, width, height, depth);
    }

    void addDecorativeCorner(glm::vec3 position, glm::vec3 upPosition, float size, float depth) {
        // Add a decorative element at the corner
        glm::vec3 center = (position + upPosition) / 2.0f;

        // Create a small ornamental piece
        addBoxVertices(center, size, size, depth * 1.5f);
    }

    // Create frame vertices for the gate-like appearance
    void createFrameVertices() {
        // Create a more stylized frame - thinner with some decorative elements
        float frameThickness = 0.05f * width;  // Thinner frame
        float frameDepth = 0.1f;               // Less depth

        // Get basic coordinates
        glm::vec3 halfWidth = right * (width / 2.0f);
        glm::vec3 halfHeight = up * (height / 2.0f);

        // Clear previous vertices
        frameVertices.clear();

        // Number of segments for rounded corners and decorative elements
        int segments = 8;

        // Create an elegant frame with rounded corners
        createFrameSegment(position - halfWidth, position - halfWidth + halfHeight * 2.0f, frameThickness, frameDepth, true);  // Left vertical
        createFrameSegment(position + halfWidth, position + halfWidth + halfHeight * 2.0f, frameThickness, frameDepth, true);  // Right vertical
        createFrameSegment(position - halfWidth, position + halfWidth, frameThickness, frameDepth, false);                     // Bottom horizontal
        createFrameSegment(position - halfWidth + halfHeight * 2.0f, position + halfWidth + halfHeight * 2.0f, frameThickness, frameDepth, false); // Top horizontal

        // Add some decorative elements at the corners
        addDecorativeCorner(position - halfWidth, position - halfWidth + up * frameThickness * 3.0f, frameThickness * 1.5f, frameDepth); // Bottom left
        addDecorativeCorner(position + halfWidth, position + halfWidth + up * frameThickness * 3.0f, frameThickness * 1.5f, frameDepth); // Bottom right
        addDecorativeCorner(position - halfWidth + halfHeight * 2.0f, position - halfWidth + halfHeight * 2.0f + up * frameThickness * 3.0f, frameThickness * 1.5f, frameDepth); // Top left
        addDecorativeCorner(position + halfWidth + halfHeight * 2.0f, position + halfWidth + halfHeight * 2.0f + up * frameThickness * 3.0f, frameThickness * 1.5f, frameDepth); // Top right

        // Create VAO/VBO for frame
        glGenVertexArrays(1, &frameVAO);
        glGenBuffers(1, &frameVBO);

        glBindVertexArray(frameVAO);

        glBindBuffer(GL_ARRAY_BUFFER, frameVBO);
        glBufferData(GL_ARRAY_BUFFER, frameVertices.size() * sizeof(float), frameVertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Helper method to add box vertices to the frame
    void addBoxVertices(glm::vec3 center, float width, float height, float depth) {
        glm::vec3 halfDims(width / 2.0f, height / 2.0f, depth / 2.0f);

        // Adjust center position to account for the portal normal direction
        center = center - normal * (depth / 2.0f);

        // Create transformation matrix for oriented box
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, center);

        // Rotation matrix to align with portal orientation
        glm::mat3 rotation(right, up, normal);

        // Create box vertices in local space, then transform to world space
        // Front face
        addQuadVertices(
            model * glm::vec4(-halfDims.x, -halfDims.y, halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, -halfDims.y, halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, halfDims.y, halfDims.z, 1.0f),
            model * glm::vec4(-halfDims.x, halfDims.y, halfDims.z, 1.0f),
            rotation * glm::vec3(0.0f, 0.0f, 1.0f)
        );

        // Back face
        addQuadVertices(
            model * glm::vec4(halfDims.x, -halfDims.y, -halfDims.z, 1.0f),
            model * glm::vec4(-halfDims.x, -halfDims.y, -halfDims.z, 1.0f),
            model * glm::vec4(-halfDims.x, halfDims.y, -halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, halfDims.y, -halfDims.z, 1.0f),
            rotation * glm::vec3(0.0f, 0.0f, -1.0f)
        );

        // Top face
        addQuadVertices(
            model * glm::vec4(-halfDims.x, halfDims.y, halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, halfDims.y, halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, halfDims.y, -halfDims.z, 1.0f),
            model * glm::vec4(-halfDims.x, halfDims.y, -halfDims.z, 1.0f),
            rotation * glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Bottom face
        addQuadVertices(
            model * glm::vec4(-halfDims.x, -halfDims.y, -halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, -halfDims.y, -halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, -halfDims.y, halfDims.z, 1.0f),
            model * glm::vec4(-halfDims.x, -halfDims.y, halfDims.z, 1.0f),
            rotation * glm::vec3(0.0f, -1.0f, 0.0f)
        );

        // Left face
        addQuadVertices(
            model * glm::vec4(-halfDims.x, -halfDims.y, -halfDims.z, 1.0f),
            model * glm::vec4(-halfDims.x, -halfDims.y, halfDims.z, 1.0f),
            model * glm::vec4(-halfDims.x, halfDims.y, halfDims.z, 1.0f),
            model * glm::vec4(-halfDims.x, halfDims.y, -halfDims.z, 1.0f),
            rotation * glm::vec3(-1.0f, 0.0f, 0.0f)
        );

        // Right face
        addQuadVertices(
            model * glm::vec4(halfDims.x, -halfDims.y, halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, -halfDims.y, -halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, halfDims.y, -halfDims.z, 1.0f),
            model * glm::vec4(halfDims.x, halfDims.y, halfDims.z, 1.0f),
            rotation * glm::vec3(1.0f, 0.0f, 0.0f)
        );
    }

    // Helper to add a quad to the frame vertices
    void addQuadVertices(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3, glm::vec4 v4, glm::vec3 normal) {
        // Convert from vec4 to vec3
        glm::vec3 p1(v1.x, v1.y, v1.z);
        glm::vec3 p2(v2.x, v2.y, v2.z);
        glm::vec3 p3(v3.x, v3.y, v3.z);
        glm::vec3 p4(v4.x, v4.y, v4.z);

        // First triangle (p1, p2, p3)
        frameVertices.push_back(p1.x); frameVertices.push_back(p1.y); frameVertices.push_back(p1.z);
        frameVertices.push_back(normal.x); frameVertices.push_back(normal.y); frameVertices.push_back(normal.z);
        frameVertices.push_back(0.0f); frameVertices.push_back(0.0f);

        frameVertices.push_back(p2.x); frameVertices.push_back(p2.y); frameVertices.push_back(p2.z);
        frameVertices.push_back(normal.x); frameVertices.push_back(normal.y); frameVertices.push_back(normal.z);
        frameVertices.push_back(1.0f); frameVertices.push_back(0.0f);

        frameVertices.push_back(p3.x); frameVertices.push_back(p3.y); frameVertices.push_back(p3.z);
        frameVertices.push_back(normal.x); frameVertices.push_back(normal.y); frameVertices.push_back(normal.z);
        frameVertices.push_back(1.0f); frameVertices.push_back(1.0f);

        // Second triangle (p1, p3, p4)
        frameVertices.push_back(p1.x); frameVertices.push_back(p1.y); frameVertices.push_back(p1.z);
        frameVertices.push_back(normal.x); frameVertices.push_back(normal.y); frameVertices.push_back(normal.z);
        frameVertices.push_back(0.0f); frameVertices.push_back(0.0f);

        frameVertices.push_back(p3.x); frameVertices.push_back(p3.y); frameVertices.push_back(p3.z);
        frameVertices.push_back(normal.x); frameVertices.push_back(normal.y); frameVertices.push_back(normal.z);
        frameVertices.push_back(1.0f); frameVertices.push_back(1.0f);

        frameVertices.push_back(p4.x); frameVertices.push_back(p4.y); frameVertices.push_back(p4.z);
        frameVertices.push_back(normal.x); frameVertices.push_back(normal.y); frameVertices.push_back(normal.z);
        frameVertices.push_back(0.0f); frameVertices.push_back(1.0f);
    }

    // Create framebuffer for rendering portal view
    void createFramebuffer(unsigned int width, unsigned int height) {
        // Generate framebuffer
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // Create texture attachment
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

        // Create renderbuffer attachment for depth and stencil
        glGenRenderbuffers(1, &renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR: Framebuffer not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

#endif