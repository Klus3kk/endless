#include "Room.h"
#include <iostream>

RoomManager::RoomManager() : currentRoom(0) {
    // Constructor initializes with room 0 (dev space)
}

RoomManager::~RoomManager() {
    // Destructor - cleanup if needed
}

void RoomManager::initializeRooms() {
    // Clear any existing rooms
    rooms.clear();

    // Room 0: Development Space (current environment)
    rooms.push_back({
        glm::vec3(0.0f, 1.0f, 5.0f),  // spawn position
        -90.0f,                        // yaw (facing -Z direction)
        0.0f,                          // pitch
        "Development Space",           // name
        1.0f,                          // non-Euclidean intensity
        glm::vec4(0.03f, 0.03f, 0.05f, 1.0f), // ambient color
        false                          // has gravity
        });

    // Room 1: Hyperbolic Chamber
    rooms.push_back({
        glm::vec3(100.0f, 1.0f, 100.0f),  // Separate location in world space
        -90.0f,
        0.0f,
        "Hyperbolic Chamber",
        1.5f,                         // Stronger non-Euclidean effects
        glm::vec4(0.02f, 0.05f, 0.1f, 1.0f), // Blue-ish ambient
        false
        });

    // Room 2: Impossible Architecture
    rooms.push_back({
        glm::vec3(200.0f, 1.0f, 100.0f),
        -90.0f,
        0.0f,
        "Impossible Architecture",
        1.2f,
        glm::vec4(0.1f, 0.05f, 0.02f, 1.0f), // Warm tones
        true // With gravity for walking around structures
        });

    // Room 3: Recursive Fractal Space
    rooms.push_back({
        glm::vec3(300.0f, 1.0f, 100.0f),
        -90.0f,
        0.0f,
        "Recursive Fractal Space",
        1.7f,
        glm::vec4(0.05f, 0.1f, 0.05f, 1.0f), // Green-ish ambient
        false
        });

    // Room 4: Klein Bottle Visualization
    rooms.push_back({
        glm::vec3(400.0f, 1.0f, 100.0f),
        -90.0f,
        0.0f,
        "Klein Bottle Space",
        1.3f,
        glm::vec4(0.1f, 0.02f, 0.1f, 1.0f), // Purple ambient
        false
        });

    // Room 5: M.C. Escher Tribute
    rooms.push_back({
        glm::vec3(500.0f, 1.0f, 100.0f),
        -90.0f,
        0.0f,
        "Escher's Playground",
        1.0f,
        glm::vec4(0.05f, 0.05f, 0.05f, 1.0f), // Monochromatic
        true
        });

    // Room 6: Psychedelic Vortex
    rooms.push_back({
        glm::vec3(600.0f, 1.0f, 100.0f),
        -90.0f,
        0.0f,
        "Psychedelic Vortex",
        1.8f,
        glm::vec4(0.08f, 0.02f, 0.08f, 1.0f), // Deep purple
        false
        });

    // Room 7: Rotating Hyperspace
    rooms.push_back({
        glm::vec3(700.0f, 1.0f, 100.0f),
        -90.0f,
        0.0f,
        "Rotating Hyperspace",
        1.4f,
        glm::vec4(0.02f, 0.02f, 0.08f, 1.0f), // Dark blue
        false
        });

    // Room 8: Spherical Geometry
    rooms.push_back({
        glm::vec3(800.0f, 1.0f, 100.0f),
        -90.0f,
        0.0f,
        "Spherical Geometry",
        1.2f,
        glm::vec4(0.1f, 0.1f, 0.02f, 1.0f), // Yellow tint
        true
        });

    // Room 9: Infinite Corridor
    rooms.push_back({
        glm::vec3(900.0f, 1.0f, 100.0f),
        -90.0f,
        0.0f,
        "Infinite Corridor",
        1.6f,
        glm::vec4(0.03f, 0.03f, 0.03f, 1.0f), // Almost black
        true
        });
}

void RoomManager::teleportToRoom(int roomIndex, Camera& camera, float& nonEuclideanFactor,
    bool& flightMode, float& verticalVelocity) {
    if (roomIndex >= 0 && roomIndex < rooms.size()) {
        // Update current room index
        currentRoom = roomIndex;
        const Room& room = rooms[currentRoom];

        // Move camera to new room's spawn point
        camera.Position = room.spawnPosition;
        camera.Yaw = room.spawnYaw;
        camera.Pitch = room.spawnPitch;
        camera.updateCameraVectors(); // Need to add this method as public in Camera class

        // Update room settings
        nonEuclideanFactor = room.nonEuclideanIntensity;
        flightMode = !room.hasGravity;
        verticalVelocity = 0.0f; // Reset vertical velocity

        // Print room info
        std::cout << "Teleported to Room " << currentRoom << ": "
            << room.name << std::endl;
        std::cout << "Non-Euclidean Factor: " << nonEuclideanFactor
            << ", Mode: " << (flightMode ? "Flying" : "Walking") << std::endl;
    }
}

int RoomManager::getCurrentRoomIndex() const {
    return currentRoom;
}

const Room& RoomManager::getRoom(int index) const {
    return rooms[index];
}

size_t RoomManager::getRoomCount() const {
    return rooms.size();
}

void RoomManager::renderFloatingFractals(Shader& shader, unsigned int cubeVAO,
    const Room& room, float time) {
    const int numObjects = 30;

    for (int i = 0; i < numObjects; i++) {
        float t = float(i) / numObjects;
        float angle = t * 20.0f * 3.14159f + time * 0.1f;
        float height = sin(angle * 0.5f) * 10.0f;
        float radius = 10.0f + 5.0f * cos(angle * 0.7f);

        glm::vec3 position = room.spawnPosition + glm::vec3(
            cos(angle) * radius,
            height,
            sin(angle) * radius
        );

        // Create a twisted, morphing shape
        float scale = 0.5f + 0.3f * sin(time * 0.5f + t * 10.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, time * 0.5f + i,
            glm::normalize(glm::vec3(sin(t * 5.0f), cos(t * 7.0f), sin(t * 3.0f))));
        model = glm::scale(model, glm::vec3(scale));

        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void RoomManager::renderRoomSpecificContent(int roomIndex, Shader& shader,
    unsigned int cubeVAO, float time) {
    // Set room-specific shader parameters
    setupRoomShader(shader, roomIndex, time);

    const Room& room = rooms[roomIndex];

    // Call the specific rendering function for the current room
    switch (roomIndex) {
    case 1: renderMandelbulbFractalSpace(shader, cubeVAO, room, time); break;
    case 2: renderEscherImpossibleArchitecture(shader, cubeVAO, room, time); break;
    case 3: renderHyperbolicSpace(shader, cubeVAO, room, time); break;
    case 4: renderKleinBottleSpace(shader, cubeVAO, room, time); break;
    case 5: renderRecursiveScalingEnvironment(shader, cubeVAO, room, time); break;
    case 6: renderQuantumSuperpositionSpace(shader, cubeVAO, room, time); break;
    case 7: renderMobiusTopology(shader, cubeVAO, room, time); break;
    case 8: renderNonCommutativeRotationSpace(shader, cubeVAO, room, time); break;
    case 9: renderInfiniteRegressionChamber(shader, cubeVAO, room, time); break;
    }
}

// 1. Mandelbulb Fractal Space
void RoomManager::renderMandelbulbFractalSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time) {
    // Create a recursive fractal structure
    renderFractalStructure(shader, cubeVAO, room.spawnPosition, 15.0f, 3, time);

    // Create floating orbital structures
    const int orbitCount = 5;
    const float orbitRadius = 25.0f;

    for (int orbit = 0; orbit < orbitCount; orbit++) {
        float orbitHeight = -10.0f + orbit * 8.0f;
        float orbitPhase = orbit * 0.5f + time * 0.2f;
        int cubesInOrbit = 10 + orbit * 5;

        for (int i = 0; i < cubesInOrbit; i++) {
            float angle = i * (2.0f * 3.14159f / cubesInOrbit) + orbitPhase;
            float x = cos(angle) * orbitRadius;
            float z = sin(angle) * orbitRadius;

            glm::vec3 position = room.spawnPosition + glm::vec3(x, orbitHeight, z);
            float scale = 0.5f + 0.3f * sin(time * 0.5f + i * 0.2f);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            model = glm::rotate(model, time * 0.5f + i * 0.1f,
                glm::vec3(sin(i * 0.1f), 1.0f, cos(i * 0.1f)));
            model = glm::scale(model, glm::vec3(scale));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Create special portal-like structures
    for (int i = 0; i < 4; i++) {
        float angle = i * (2.0f * 3.14159f / 4.0f);
        float distance = 20.0f;
        glm::vec3 portalPos = room.spawnPosition + glm::vec3(cos(angle) * distance, 0.0f, sin(angle) * distance);

        // Create frame
        renderPortalFrame(shader, cubeVAO, portalPos, angle + 3.14159f * 0.5f, 5.0f, 8.0f, time);
    }
}

// Helper for creating fractal structures
void RoomManager::renderFractalStructure(Shader& shader, unsigned int cubeVAO,
    glm::vec3 center, float size, int depth, float time) {
    if (depth <= 0) return;

    // Center cube
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, center);
    model = glm::rotate(model, time * (4 - depth) * 0.1f,
        glm::vec3(sin(time * 0.3f), cos(time * 0.2f), sin(time * 0.1f)));
    model = glm::scale(model, glm::vec3(size));

    shader.setMat4("model", model);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    if (depth > 1) {
        float newSize = size * 0.3f;
        float offset = size * 0.7f;

        for (int i = 0; i < 8; i++) {
            float xDir = (i & 1) ? 1.0f : -1.0f;
            float yDir = (i & 2) ? 1.0f : -1.0f;
            float zDir = (i & 4) ? 1.0f : -1.0f;

            glm::vec3 newCenter = center + glm::vec3(xDir * offset, yDir * offset, zDir * offset);
            renderFractalStructure(shader, cubeVAO, newCenter, newSize, depth - 1, time);
        }
    }
}

// 2. Escher's Impossible Architecture
void RoomManager::renderEscherImpossibleArchitecture(Shader& shader, unsigned int cubeVAO, const Room& room, float time) {
    // Create an impossible staircase
    const int numSteps = 40;
    for (int i = 0; i < numSteps; i++) {
        float t = (float)i / numSteps;
        float angle = t * 2.0f * 3.14159f;
        float height = (i % numSteps) * 0.5f;

        // Reset height to create the "impossible" loop
        if (i == numSteps - 1) height = 0;

        glm::vec3 stairPos = room.spawnPosition + glm::vec3(
            cos(angle) * 10.0f,
            height,
            sin(angle) * 10.0f
        );

        // Create stair step
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, stairPos);
        model = glm::rotate(model, angle + 3.14159f * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(3.0f, 0.25f, 1.0f));

        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Create stair support
        model = glm::mat4(1.0f);
        model = glm::translate(model, stairPos + glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, angle + 3.14159f * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.25f, 2.0f, 0.25f));

        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Create an "impossible triangle" structure
    for (int side = 0; side < 3; side++) {
        float sideAngle = side * (2.0f * 3.14159f / 3.0f);

        for (int i = 0; i < 10; i++) {
            float t = (float)i / 10.0f;

            // Create vertical displacement that makes it seem impossible
            float height = 5.0f + (side == 0 ? t * 5.0f :
                (side == 1 ? (1.0f - t) * 5.0f : 0.0f));

            glm::vec3 pos = room.spawnPosition + glm::vec3(
                cos(sideAngle + t * (2.0f * 3.14159f / 3.0f)) * 20.0f,
                height,
                sin(sideAngle + t * (2.0f * 3.14159f / 3.0f)) * 20.0f
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, sideAngle + t * (2.0f * 3.14159f / 3.0f) + 3.14159f * 0.5f,
                glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

// 3. Hyperbolic Space
void RoomManager::renderHyperbolicSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time) {
    // In hyperbolic space, parallel lines diverge and there's exponentially more space 
    // as you move away from a point

    // Create radial lines that appear to diverge
    const int radialLines = 12;
    const int segmentsPerLine = 20;
    const float maxRadius = 50.0f;

    for (int line = 0; line < radialLines; line++) {
        float angle = line * (2.0f * 3.14159f / radialLines);

        for (int seg = 0; seg < segmentsPerLine; seg++) {
            float t = (float)seg / segmentsPerLine;

            // In hyperbolic space, objects get smaller as they move away from center
            float radius = t * maxRadius;
            float scale = 1.0f / (1.0f + radius * 0.1f);

            // Position along the radial line
            glm::vec3 pos = room.spawnPosition + glm::vec3(
                cos(angle) * radius,
                sin(time * 0.2f + line * 0.5f + seg * 0.1f) * 2.0f,
                sin(angle) * radius
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, time * 0.2f + seg * 0.1f,
                glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(scale * 1.5f));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Create circular hoops that demonstrate the exponential nature of hyperbolic space
    const int numHoops = 8;

    for (int h = 0; h < numHoops; h++) {
        float hoopRadius = 5.0f + h * 5.0f;
        int segmentsInHoop = 16 + h * 8; // More segments needed for larger hoops

        for (int i = 0; i < segmentsInHoop; i++) {
            float angle = i * (2.0f * 3.14159f / segmentsInHoop);

            // In hyperbolic space, as radius increases, the circumference grows exponentially
            float scale = 1.0f / (1.0f + hoopRadius * 0.05f);

            glm::vec3 pos = room.spawnPosition + glm::vec3(
                cos(angle) * hoopRadius,
                sin(time * 0.1f + h * 0.2f) * 2.0f + h * 0.5f,
                sin(angle) * hoopRadius
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, angle + 3.14159f * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(scale * 1.0f, scale * 0.5f, scale * 2.0f));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

// 4. Klein Bottle Space
void RoomManager::renderKleinBottleSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time) {
    // Create a visual representation of a Klein bottle
    // A Klein bottle is a surface that has no inside or outside

    const int uSegments = 24;
    const int vSegments = 12;
    const float kleinRadius = 15.0f;
    const float tubeRadius = 3.0f;

    for (int u = 0; u < uSegments; u++) {
        float uT = (float)u / uSegments;
        float uAngle = uT * 2.0f * 3.14159f;

        for (int v = 0; v < vSegments; v++) {
            float vT = (float)v / vSegments;
            float vAngle = vT * 2.0f * 3.14159f;

            // Klein bottle parametric equations (simplified for visualization)
            glm::vec3 pos;
            if (uT < 0.5f) {
                // First half (standard torus)
                pos = room.spawnPosition + glm::vec3(
                    (kleinRadius + tubeRadius * cos(vAngle)) * cos(uAngle),
                    tubeRadius * sin(vAngle),
                    (kleinRadius + tubeRadius * cos(vAngle)) * sin(uAngle)
                );
            }
            else {
                // Second half (bottle neck that passes through itself)
                float newUT = (uT - 0.5f) * 2.0f; // Remap to 0-1
                pos = room.spawnPosition + glm::vec3(
                    (kleinRadius - tubeRadius * cos(vAngle)) * cos(uAngle),
                    tubeRadius * sin(vAngle),
                    (kleinRadius - tubeRadius * cos(vAngle)) * sin(uAngle)
                );

                // Apply twist based on v to create the self-intersection
                if (vT > 0.25f && vT < 0.75f) {
                    float twist = (vT - 0.25f) * 2.0f; // 0 to 1
                    twist = sin(twist * 3.14159f);
                    pos.x += twist * 5.0f * sin(time * 0.2f);
                    pos.z -= twist * 5.0f * cos(time * 0.2f);
                }
            }

            // Add time-based movement
            pos.y += sin(uAngle * 3.0f + time * 0.5f) * 1.0f;

            // Draw cube at point
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, uAngle, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, vAngle, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Add special portal pairs that connect in a way that inverts orientation
    for (int i = 0; i < 2; i++) {
        float angle = i * 3.14159f;
        glm::vec3 portalPos = room.spawnPosition + glm::vec3(cos(angle) * 25.0f, 0.0f, sin(angle) * 25.0f);

        renderPortalFrame(shader, cubeVAO, portalPos, angle + 3.14159f, 6.0f, 10.0f, time);
    }
}

// 5. Recursive Scaling Environment
void RoomManager::renderRecursiveScalingEnvironment(Shader& shader, unsigned int cubeVAO, const Room& room, float time) {
    // Create nested structures of different scales

    // Central structure - series of nested cubes
    for (int i = 0; i < 10; i++) {
        float scale = 10.0f * pow(0.8f, i);
        float rotation = time * (0.1f + i * 0.05f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, room.spawnPosition);
        model = glm::rotate(model, rotation, glm::vec3(sin(i * 0.1f), 1.0f, cos(i * 0.1f)));
        model = glm::scale(model, glm::vec3(scale));

        shader.setMat4("model", model);

        // Use wireframe for better visualization of nesting
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Create scale-recursion portals
    for (int i = 0; i < 4; i++) {
        float angle = i * (2.0f * 3.14159f / 4.0f);

        // Each portal leads to a scaled version of the same space
        glm::vec3 portalPos = room.spawnPosition + glm::vec3(
            cos(angle) * 20.0f,
            sin(time * 0.2f + i) * 2.0f,
            sin(angle) * 20.0f
        );

        // Create portal frame
        renderPortalFrame(shader, cubeVAO, portalPos, angle + 3.14159f, 5.0f, 8.0f, time);

        // Create a visual hint of what's through each portal by showing
        // a scaled preview structure
        float previewScale = 0.3f * (i + 1);

        glm::vec3 previewPos = portalPos + glm::vec3(
            cos(angle + 3.14159f) * 2.0f,
            0.0f,
            sin(angle + 3.14159f) * 2.0f
        );

        for (int j = 0; j < 3; j++) {
            float subScale = previewScale * pow(0.7f, j);

            glm::mat4 previewModel = glm::mat4(1.0f);
            previewModel = glm::translate(previewModel, previewPos);
            previewModel = glm::rotate(previewModel, time * 0.5f + j * 0.2f,
                glm::vec3(0.0f, 1.0f, 0.0f));
            previewModel = glm::scale(previewModel, glm::vec3(subScale));

            shader.setMat4("model", previewModel);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

// 6. Quantum Superposition Space
void RoomManager::renderQuantumSuperpositionSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time) {
    // Create objects that exist in multiple states simultaneously

    // Wave function visualization
    const int gridSize = 10;
    const float spacing = 5.0f;

    for (int x = -gridSize; x <= gridSize; x++) {
        for (int z = -gridSize; z <= gridSize; z++) {
            float dist = sqrt(x * x + z * z);
            if (dist > gridSize) continue;

            // Wave function parameters
            float phase = dist * 0.5f - time * 1.0f;
            float amplitude = sin(phase) * 0.5f + 0.5f;

            // Probability collapse - objects flicker in and out of existence
            float existence = sin(time * 2.0f + x * 0.1f + z * 0.1f) * 0.5f + 0.5f;
            float height = amplitude * 3.0f;

            // Skip rendering some cubes based on quantum probability
            if (existence < 0.3f) continue;

            glm::vec3 pos = room.spawnPosition + glm::vec3(
                x * spacing,
                height,
                z * spacing
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, time * existence,
                glm::vec3(sin(x * 0.1f), cos(z * 0.1f), sin(time * 0.3f)));
            model = glm::scale(model, glm::vec3(existence));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Entangled portals - pairs of portals that show quantum entanglement
    for (int i = 0; i < 3; i++) {
        float angle1 = i * (2.0f * 3.14159f / 3.0f);
        float angle2 = angle1 + 3.14159f;

        glm::vec3 portal1Pos = room.spawnPosition + glm::vec3(
            cos(angle1) * 30.0f,
            sin(time * 0.3f + i) * 2.0f,
            sin(angle1) * 30.0f
        );

        glm::vec3 portal2Pos = room.spawnPosition + glm::vec3(
            cos(angle2) * 30.0f,
            sin(time * 0.3f + i + 3.14159f) * 2.0f, // Phase offset for entanglement
            sin(angle2) * 30.0f
        );

        // Render first portal
        renderPortalFrame(shader, cubeVAO, portal1Pos, angle1 + 3.14159f, 5.0f, 8.0f, time);

        // Render second portal (entangled)
        renderPortalFrame(shader, cubeVAO, portal2Pos, angle2 + 3.14159f, 5.0f, 8.0f, time);

        // Visualize entanglement with particle stream between portals
        const int particleCount = 20;
        for (int p = 0; p < particleCount; p++) {
            float t = (float)p / particleCount;
            float particlePhase = t + time * 0.5f;

            // Particles follow a curved path between portals
            float heightOffset = sin(particlePhase * 3.14159f) * 5.0f;
            float radialOffset = sin(particlePhase * 3.14159f * 2.0f) * 3.0f;

            glm::vec3 particlePos = glm::mix(portal1Pos, portal2Pos, t);
            particlePos.y += heightOffset;

            // Add radial offset to particle path
            glm::vec3 toCenter = glm::normalize(room.spawnPosition - particlePos);
            glm::vec3 radialDir = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), toCenter);
            particlePos += radialDir * radialOffset;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, particlePos);
            model = glm::scale(model, glm::vec3(0.2f));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

// 7. Möbius Topology
void RoomManager::renderMobiusTopology(Shader& shader, unsigned int cubeVAO, const Room& room, float time) {
    // Create a Möbius strip structure
    const int segmentsAround = 40;
    const int segmentsAcross = 8;
    const float mobiusRadius = 20.0f;
    const float stripWidth = 4.0f;

    for (int i = 0; i < segmentsAround; i++) {
        float t = (float)i / segmentsAround;
        float angle = t * 2.0f * 3.14159f;

        for (int j = 0; j < segmentsAcross; j++) {
            float s = (float)j / (segmentsAcross - 1) - 0.5f;

            // Möbius strip parametric equations
            float twistAngle = angle * 0.5f; // Half twist

            glm::vec3 pos = room.spawnPosition + glm::vec3(
                (mobiusRadius + s * stripWidth * cos(twistAngle)) * cos(angle),
                s * stripWidth * sin(twistAngle) + sin(time * 0.2f + t * 5.0f) * 1.0f, // Add wave
                (mobiusRadius + s * stripWidth * cos(twistAngle)) * sin(angle)
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);

            // Orient cubes to follow the strip
            glm::vec3 tangent = glm::normalize(glm::vec3(
                -sin(angle),
                0.0f,
                cos(angle)
            ));

            glm::vec3 normal = glm::normalize(glm::vec3(
                cos(angle) * sin(twistAngle),
                cos(twistAngle),
                sin(angle) * sin(twistAngle)
            ));

            glm::vec3 binormal = glm::cross(tangent, normal);

            glm::mat4 rotation = glm::mat4(
                glm::vec4(tangent, 0.0f),
                glm::vec4(normal, 0.0f),
                glm::vec4(binormal, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
            );

            model = model * rotation;
            model = glm::scale(model, glm::vec3(0.5f));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Create portal pair with Möbius twist
    // When going through this portal, you come out flipped
    glm::vec3 portalPos = room.spawnPosition + glm::vec3(0.0f, 0.0f, -30.0f);
    renderPortalFrame(shader, cubeVAO, portalPos, 0.0f, 6.0f, 10.0f, time);

    // Visual indicator of the twist - particle stream that twists
    const int particleCount = 50;
    for (int p = 0; p < particleCount; p++) {
        float t = (float)p / particleCount;
        float particleAngle = t * 4.0f * 3.14159f + time * 0.5f;

        glm::vec3 particlePos = portalPos + glm::vec3(
            3.0f * cos(particleAngle),
            3.0f * sin(particleAngle),
            sin(time * 0.3f + t * 5.0f) * 2.0f
        );

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, particlePos);
        model = glm::scale(model, glm::vec3(0.2f));

        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

// 8. Non-Commutative Rotation Space (continued)
void RoomManager::renderNonCommutativeRotationSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time) {
    // In this space, order of rotations matters - rotating X then Y is not the same as Y then X

    // Create grid of objects demonstrating rotational asymmetry
    const int gridSize = 5;
    const float spacing = 8.0f;

    for (int x = -gridSize; x <= gridSize; x++) {
        for (int y = -gridSize; y <= gridSize; y++) {
            // Skip some grid positions to create more interesting patterns
            if ((x + y) % 3 == 0) continue;

            glm::vec3 basePos = room.spawnPosition + glm::vec3(
                x * spacing,
                0.0f,
                y * spacing
            );

            // Apply two different rotation sequences based on position
            float rotX = time * 0.3f + x * 0.1f;
            float rotY = time * 0.2f + y * 0.1f;

            // First rotation sequence: X then Y
            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model1, basePos + glm::vec3(-2.0f, 3.0f, 0.0f));
            model1 = glm::rotate(model1, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
            model1 = glm::rotate(model1, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
            model1 = glm::scale(model1, glm::vec3(1.0f, 3.0f, 1.0f)); // Elongated to show orientation

            shader.setMat4("model", model1);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Second rotation sequence: Y then X
            glm::mat4 model2 = glm::mat4(1.0f);
            model2 = glm::translate(model2, basePos + glm::vec3(2.0f, 3.0f, 0.0f));
            model2 = glm::rotate(model2, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
            model2 = glm::rotate(model2, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
            model2 = glm::scale(model2, glm::vec3(1.0f, 3.0f, 1.0f));

            shader.setMat4("model", model2);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Connection beam to show they're related
            glm::mat4 connector = glm::mat4(1.0f);
            connector = glm::translate(connector, basePos + glm::vec3(0.0f, 3.0f, 0.0f));
            connector = glm::scale(connector, glm::vec3(4.5f, 0.2f, 0.2f));

            shader.setMat4("model", connector);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Create portals that apply different rotation sequences
    for (int i = 0; i < 4; i++) {
        float angle = i * (2.0f * 3.14159f / 4.0f) + time * 0.1f;

        glm::vec3 portalPos = room.spawnPosition + glm::vec3(
            cos(angle) * 25.0f,
            sin(time * 0.2f + i) * 2.0f,
            sin(angle) * 25.0f
        );

        // Create portal frame with rotation indicator
        renderPortalFrame(shader, cubeVAO, portalPos, angle + 3.14159f, 5.0f, 8.0f, time);

        // Add rotation indicators
        const int indicatorCount = 3;
        for (int j = 0; j < indicatorCount; j++) {
            float indicatorAngle = j * (2.0f * 3.14159f / indicatorCount) + time * 0.5f;

            glm::vec3 indicatorPos = portalPos + glm::vec3(
                cos(indicatorAngle) * 3.0f,
                0.0f,
                sin(indicatorAngle) * 3.0f
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, indicatorPos);
            model = glm::rotate(model, indicatorAngle + time * (0.5f + i * 0.2f),
                glm::vec3(i % 2, (i + 1) % 2, (i + 2) % 2));
            model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

// 9. Infinite Regression Chamber
void RoomManager::renderInfiniteRegressionChamber(Shader& shader, unsigned int cubeVAO, const Room& room, float time) {
    // Create an environment where structures repeat at different scales inward/outward

    // Create nested spheres of cubes, getting denser as they get smaller
    const int numLayers = 10;

    for (int layer = 0; layer < numLayers; layer++) {
        float scale = pow(0.7f, layer); // Each layer is 70% size of previous
        float radius = 30.0f * scale;

        // More points for outer layers, fewer for inner
        int pointsInLayer = 150 * scale + 10;

        for (int i = 0; i < pointsInLayer; i++) {
            // Fibonacci sphere distribution for uniform points
            float phi = 3.14159f * (3.0f - sqrt(5.0f)); // Golden angle
            float y = 1.0f - (i / (float)(pointsInLayer - 1)) * 2.0f; // -1 to 1
            float radiusAtY = sqrt(1.0f - y * y); // Radius at y position

            float theta = phi * i; // Golden angle increment

            float x = cos(theta) * radiusAtY;
            float z = sin(theta) * radiusAtY;

            // Add time-based animation - inner layers rotate faster
            float layerTime = time * (1.0f + layer * 0.5f);
            float rotX = sin(layerTime * 0.3f) * 3.14159f;
            float rotY = layerTime * 0.2f;
            float rotZ = cos(layerTime * 0.4f) * 3.14159f;

            // Rotate the point
            glm::vec4 rotatedPoint = glm::vec4(x, y, z, 1.0f);
            glm::mat4 rotation = glm::mat4(1.0f);
            rotation = glm::rotate(rotation, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
            rotation = glm::rotate(rotation, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
            rotation = glm::rotate(rotation, rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
            rotatedPoint = rotation * rotatedPoint;

            glm::vec3 pos = room.spawnPosition + glm::vec3(
                rotatedPoint.x * radius,
                rotatedPoint.y * radius,
                rotatedPoint.z * radius
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, layerTime + i,
                glm::vec3(x, y, z));

            // Scale inversely to layer - creates infinite regression feeling
            float cubeScale = 0.3f * scale;
            model = glm::scale(model, glm::vec3(cubeScale));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Create recursion portals - visually these should appear to lead to smaller versions of the same space
    for (int i = 0; i < 6; i++) {
        // Position at vertices of octahedron
        float theta = i < 4 ? (i * 3.14159f / 2.0f) : 0.0f;
        float phi = i < 4 ? 0.0f : (i == 4 ? 3.14159f / 2.0f : -3.14159f / 2.0f);

        glm::vec3 direction(
            cos(phi) * cos(theta),
            sin(phi),
            cos(phi) * sin(theta)
        );

        glm::vec3 portalPos = room.spawnPosition + direction * 20.0f;

        // Create portal looking inward
        float angle = atan2(direction.z, direction.x) + 3.14159f;
        float pitch = asin(direction.y);

        // Create portal frame with size suggesting recursion
        renderPortalFrame(shader, cubeVAO, portalPos, angle, 5.0f, 8.0f, time);

        // Add visual hint of recursion through the portal
        for (int j = 0; j < 3; j++) {
            float previewScale = 0.3f * pow(0.6f, j);
            float previewDist = 3.0f * (j + 1);

            glm::vec3 previewPos = portalPos - direction * previewDist;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, previewPos);
            model = glm::rotate(model, time * (0.5f + j * 0.2f), direction);
            model = glm::scale(model, glm::vec3(previewScale));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

void RoomManager::renderPortalFrame(Shader& shader, unsigned int cubeVAO,
    glm::vec3 position, float angle,
    float width, float height, float time) {
    // Create a portal frame with animation
    const int segments = 20;
    float thickness = 0.3f;

    glm::vec3 right(cos(angle), 0.0f, sin(angle));
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    // Top and bottom segments
    for (int i = 0; i < segments; i++) {
        float t = (float)i / (segments - 1);
        float xOffset = (t - 0.5f) * width;

        // Top frame piece
        glm::vec3 topPos = position + right * xOffset + up * (height / 2.0f);
        float topScale = 0.5f + 0.2f * sin(time * 2.0f + t * 10.0f);

        glm::mat4 topModel = glm::mat4(1.0f);
        topModel = glm::translate(topModel, topPos);
        topModel = glm::rotate(topModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        topModel = glm::scale(topModel, glm::vec3(thickness * topScale, thickness, thickness * topScale));

        shader.setMat4("model", topModel);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Bottom frame piece
        glm::vec3 bottomPos = position + right * xOffset - up * (height / 2.0f);
        float bottomScale = 0.5f + 0.2f * sin(time * 2.0f + t * 10.0f + 3.14159f);

        glm::mat4 bottomModel = glm::mat4(1.0f);
        bottomModel = glm::translate(bottomModel, bottomPos);
        bottomModel = glm::rotate(bottomModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        bottomModel = glm::scale(bottomModel, glm::vec3(thickness * bottomScale, thickness, thickness * bottomScale));

        shader.setMat4("model", bottomModel);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Left and right segments
    for (int i = 0; i < segments; i++) {
        float t = (float)i / (segments - 1);
        float yOffset = (t - 0.5f) * height;

        // Left frame piece
        glm::vec3 leftPos = position - right * (width / 2.0f) + up * yOffset;
        float leftScale = 0.5f + 0.2f * sin(time * 2.0f + t * 10.0f + 1.57f);

        glm::mat4 leftModel = glm::mat4(1.0f);
        leftModel = glm::translate(leftModel, leftPos);
        leftModel = glm::rotate(leftModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        leftModel = glm::scale(leftModel, glm::vec3(thickness * leftScale, thickness, thickness * leftScale));

        shader.setMat4("model", leftModel);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Right frame piece
        glm::vec3 rightPos = position + right * (width / 2.0f) + up * yOffset;
        float rightScale = 0.5f + 0.2f * sin(time * 2.0f + t * 10.0f + 4.71f);

        glm::mat4 rightModel = glm::mat4(1.0f);
        rightModel = glm::translate(rightModel, rightPos);
        rightModel = glm::rotate(rightModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        rightModel = glm::scale(rightModel, glm::vec3(thickness * rightScale, thickness, thickness * rightScale));

        shader.setMat4("model", rightModel);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

// Implementation of room-specific rendering functions

void RoomManager::renderHyperbolicRoom(Shader& shader, unsigned int cubeVAO, float time) {
    // Create a circular arrangement of pillars with hyperbolic distortion
    const int numPillars = 16;
    const float radius = 15.0f;
    const Room& room = rooms[1]; // Get room properties

    for (int i = 0; i < numPillars; i++) {
        float angle = i * (2.0f * 3.14159f / numPillars);

        // In hyperbolic space, apparent distance increases exponentially with actual distance
        // We simulate this by placing pillars at exponentially increasing distances
        float distortedRadius = radius * pow(1.2f, sin(time * 0.1f) * 2.0f + 2.0f);

        glm::vec3 basePos(
            room.spawnPosition.x + cos(angle) * distortedRadius,
            room.spawnPosition.y,
            room.spawnPosition.z + sin(angle) * distortedRadius
        );

        // Pillars appear to curve as you look around the circle
        float heightDistortion = 8.0f + sin(angle * 3.0f + time * 0.3f) * 4.0f;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, basePos);
        model = glm::scale(model, glm::vec3(1.0f, heightDistortion, 1.0f));

        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Add connecting arches between pillars
        float nextAngle = (i + 1) % numPillars * (2.0f * 3.14159f / numPillars);
        glm::vec3 nextPos(
            room.spawnPosition.x + cos(nextAngle) * distortedRadius,
            room.spawnPosition.y + heightDistortion,
            room.spawnPosition.z + sin(nextAngle) * distortedRadius
        );

        // Create curved arch between pillars
        const int archSegments = 8;
        for (int j = 0; j < archSegments; j++) {
            float t = (float)j / (archSegments - 1);
            glm::vec3 archPos = glm::mix(
                basePos + glm::vec3(0, heightDistortion, 0),
                nextPos,
                t
            );

            // Add curve to arch
            archPos.y += sin(t * 3.14159f) * 2.0f;

            model = glm::mat4(1.0f);
            model = glm::translate(model, archPos);
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Add central structure
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, room.spawnPosition + glm::vec3(0.0f, 5.0f, 0.0f));
    model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
    model = glm::rotate(model, time * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RoomManager::renderImpossibleArchitecture(Shader& shader, unsigned int cubeVAO, float time) {
    // Get room properties
    const Room& room = rooms[2];

    // Create Penrose-like stairs that loop impossibly
    const int numSteps = 20;
    const float radius = 10.0f;

    for (int i = 0; i < numSteps * 4; i++) {
        float angle = i * (2.0f * 3.14159f / (numSteps * 4));
        float height = (i % numSteps) * 0.5f;

        // Create an impossible staircase that loops back on itself
        glm::vec3 stairPos(
            room.spawnPosition.x + cos(angle) * radius,
            room.spawnPosition.y + height,
            room.spawnPosition.z + sin(angle) * radius
        );

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, stairPos);
        model = glm::scale(model, glm::vec3(2.0f, 0.25f, 1.0f));
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Create walls that bend in impossible ways
    const int wallSegments = 15;
    for (int i = 0; i < wallSegments; i++) {
        float t = (float)i / (wallSegments - 1);
        float angle = t * 3.14159f * 2.0f;

        // Wall position forms a "twisted" circular corridor
        glm::vec3 wallPos(
            room.spawnPosition.x + cos(angle) * 20.0f,
            room.spawnPosition.y + sin(angle * 2.0f + time * 0.2f) * 5.0f,
            room.spawnPosition.z + sin(angle) * 20.0f
        );

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, wallPos);
        model = glm::scale(model, glm::vec3(3.0f, 5.0f, 0.2f));
        model = glm::rotate(model, angle + 3.14159f * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void RoomManager::renderFractalSpace(Shader& shader, unsigned int cubeVAO, float time) {
    // Get room properties
    const Room& room = rooms[3];

    // Render a recursive structure
    renderFractalCube(shader, cubeVAO, room.spawnPosition, 10.0f, 3, time);
}

void RoomManager::renderFractalCube(Shader& shader, unsigned int cubeVAO,
    glm::vec3 center, float size, int depth, float time) {
    if (depth <= 0) return;

    // Render center cube
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, center);
    model = glm::scale(model, glm::vec3(size));
    model = glm::rotate(model, time * (4 - depth) * 0.1f,
        glm::vec3(sin(time * 0.3f), cos(time * 0.2f), sin(time * 0.1f)));

    shader.setMat4("model", model);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Recursively add smaller cubes at corners if depth > 1
    if (depth > 1) {
        float newSize = size * 0.4f;
        float offset = size * 0.75f;

        // Scale factor that makes the structure more "alive"
        float scaleFactor = 0.5f + 0.2f * sin(time * 0.5f * depth);

        // Generate 8 corner cubes
        for (int i = 0; i < 8; i++) {
            float xDir = (i & 1) ? 1.0f : -1.0f;
            float yDir = (i & 2) ? 1.0f : -1.0f;
            float zDir = (i & 4) ? 1.0f : -1.0f;

            glm::vec3 newCenter = center + glm::vec3(
                xDir * offset,
                yDir * offset,
                zDir * offset
            );

            // Recursively render smaller cube
            renderFractalCube(shader, cubeVAO, newCenter, newSize * scaleFactor,
                depth - 1, time);
        }
    }
}

// Implement the remaining room-specific rendering functions
void RoomManager::renderKleinBottleSpace(Shader& shader, unsigned int cubeVAO, float time) {
    const Room& room = rooms[4];

    // Create a Klein bottle-inspired space where paths loop back upon themselves
    // (simplified representation using cube arrangement)
    const int numSections = 24;
    const float pathRadius = 15.0f;
    const float tubeRadius = 3.0f;

    for (int i = 0; i < numSections; i++) {
        float t = (float)i / numSections;
        float angle = t * 4.0f * 3.14159f; // Two full rotations

        // Position along the main circle
        glm::vec3 center(
            room.spawnPosition.x + cos(angle) * pathRadius,
            room.spawnPosition.y,
            room.spawnPosition.z + sin(angle) * pathRadius
        );

        // Klein bottle effect - after halfway, we start twisting and inverting
        if (t > 0.5f) {
            // Twist the tube
            float twistAngle = (t - 0.5f) * 2.0f * 3.14159f;
            center.y += 5.0f * sin((t - 0.5f) * 2.0f * 3.14159f);

            // Create tube cross-section that goes "through itself"
            const int crossSectionPoints = 8;
            for (int j = 0; j < crossSectionPoints; j++) {
                float crossAngle = j * (2.0f * 3.14159f / crossSectionPoints);
                float crossRadius = tubeRadius * (1.0f - 0.5f * (t - 0.5f) * 2.0f);

                glm::vec3 offset(
                    cos(crossAngle + twistAngle) * crossRadius,
                    sin(crossAngle) * crossRadius,
                    0.0f
                );

                // Rotate offset to follow the circle
                glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec3 rotatedOffset = glm::vec3(rotMat * glm::vec4(offset, 0.0f));

                glm::vec3 cubePos = center + rotatedOffset;

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePos);
                model = glm::scale(model, glm::vec3(0.5f + 0.2f * sin(time * 0.5f + t * 10.0f)));

                shader.setMat4("model", model);
                glBindVertexArray(cubeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        else {
            // Create tube cross-section (first half of Klein bottle)
            const int crossSectionPoints = 8;
            for (int j = 0; j < crossSectionPoints; j++) {
                float crossAngle = j * (2.0f * 3.14159f / crossSectionPoints);

                glm::vec3 offset(
                    cos(crossAngle) * tubeRadius,
                    sin(crossAngle) * tubeRadius,
                    0.0f
                );

                // Rotate offset to follow the circle
                glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec3 rotatedOffset = glm::vec3(rotMat * glm::vec4(offset, 0.0f));

                glm::vec3 cubePos = center + rotatedOffset;

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePos);
                model = glm::scale(model, glm::vec3(0.5f + 0.2f * sin(time * 0.5f + t * 10.0f)));

                shader.setMat4("model", model);
                glBindVertexArray(cubeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }
}

void RoomManager::renderEscherPlayground(Shader& shader, unsigned int cubeVAO, float time) {
    const Room& room = rooms[5];

    // Create an M.C. Escher-inspired space with impossible connections
    // Waterfall that flows upward in a loop
    const int numSegments = 30;
    const float pathRadius = 15.0f;

    for (int i = 0; i < numSegments; i++) {
        float t = (float)i / numSegments;

        // Create a rectangular path that seemingly goes uphill all the way around
        float x, y, z;

        if (t < 0.25f) {
            // First segment (appears to go uphill to the right)
            x = room.spawnPosition.x - pathRadius + t * 4.0f * pathRadius;
            y = room.spawnPosition.y + t * 4.0f;
            z = room.spawnPosition.z - pathRadius;
        }
        else if (t < 0.5f) {
            // Second segment (appears to go uphill to the back)
            x = room.spawnPosition.x + pathRadius;
            y = room.spawnPosition.y + 1.0f + (t - 0.25f) * 4.0f;
            z = room.spawnPosition.z - pathRadius + (t - 0.25f) * 4.0f * pathRadius;
        }
        else if (t < 0.75f) {
            // Third segment (appears to go uphill to the left)
            x = room.spawnPosition.x + pathRadius - (t - 0.5f) * 4.0f * pathRadius;
            y = room.spawnPosition.y + 2.0f + (t - 0.5f) * 4.0f;
            z = room.spawnPosition.z + pathRadius;
        }
        else {
            // Fourth segment (appears to go uphill to the front, connecting back to start)
            x = room.spawnPosition.x - pathRadius;
            y = room.spawnPosition.y + 3.0f + (t - 0.75f) * 4.0f;
            z = room.spawnPosition.z + pathRadius - (t - 0.75f) * 4.0f * pathRadius;
        }

        // Create water/path blocks
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(2.0f, 0.2f, 2.0f));

        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Add some flowing "water" particles
        float flowT = fmod(t + time * 0.1f, 1.0f);
        float flowX, flowY, flowZ;

        if (flowT < 0.25f) {
            flowX = room.spawnPosition.x - pathRadius + flowT * 4.0f * pathRadius;
            flowY = room.spawnPosition.y + flowT * 4.0f + 0.3f;
            flowZ = room.spawnPosition.z - pathRadius;
        }
        else if (flowT < 0.5f) {
            flowX = room.spawnPosition.x + pathRadius;
            flowY = room.spawnPosition.y + 1.0f + (flowT - 0.25f) * 4.0f + 0.3f;
            flowZ = room.spawnPosition.z - pathRadius + (flowT - 0.25f) * 4.0f * pathRadius;
        }
        else if (flowT < 0.75f) {
            flowX = room.spawnPosition.x + pathRadius - (flowT - 0.5f) * 4.0f * pathRadius;
            flowY = room.spawnPosition.y + 2.0f + (flowT - 0.5f) * 4.0f + 0.3f;
            flowZ = room.spawnPosition.z + pathRadius;
        }
        else {
            flowX = room.spawnPosition.x - pathRadius;
            flowY = room.spawnPosition.y + 3.0f + (flowT - 0.75f) * 4.0f + 0.3f;
            flowZ = room.spawnPosition.z + pathRadius - (flowT - 0.75f) * 4.0f * pathRadius;
        }

        // Render water particles
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(flowX, flowY, flowZ));
        model = glm::scale(model, glm::vec3(0.3f));

        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void RoomManager::renderPsychedelicVortex(Shader& shader, unsigned int cubeVAO, float time) {
    const Room& room = rooms[6];

    // Create a spiraling vortex of cubes with intense color shifts
    const int spiralArms = 3;
    const int cubesPerArm = 50;
    const float maxRadius = 30.0f;

    for (int arm = 0; arm < spiralArms; arm++) {
        float armOffset = arm * (2.0f * 3.14159f / spiralArms);

        for (int i = 0; i < cubesPerArm; i++) {
            float t = (float)i / cubesPerArm;
            float angle = t * 10.0f * 3.14159f + armOffset + time * 0.5f;
            float radius = t * maxRadius;

            // Spiral inward
            glm::vec3 cubePos(
                room.spawnPosition.x + cos(angle) * radius,
                room.spawnPosition.y + sin(time * 0.3f + t * 10.0f) * 5.0f - 10.0f + t * 40.0f,
                room.spawnPosition.z + sin(angle) * radius
            );

            // Scale based on distance from center
            float scale = 0.2f + t * 1.0f;

            // Rotation based on time
            float rotSpeed = (1.0f - t) * 5.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePos);
            model = glm::rotate(model, time * rotSpeed,
                glm::vec3(sin(time + t), cos(time * 0.7f), sin(time * 0.5f)));
            model = glm::scale(model, glm::vec3(scale));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Add central vortex "eye"
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, room.spawnPosition + glm::vec3(0.0f, 10.0f, 0.0f));
    model = glm::rotate(model, time, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f + sin(time * 2.0f) * 1.0f));

    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RoomManager::renderRotatingHyperspace(Shader& shader, unsigned int cubeVAO, float time) {
    const Room& room = rooms[7];

    // Create a 4D-inspired space with objects that seem to rotate through higher dimensions
    const int numLayers = 5;
    const int objectsPerLayer = 12;

    for (int layer = 0; layer < numLayers; layer++) {
        float layerHeight = layer * 5.0f - 10.0f;
        float layerRotation = time * (0.2f + layer * 0.1f);

        for (int i = 0; i < objectsPerLayer; i++) {
            float angle = i * (2.0f * 3.14159f / objectsPerLayer) + layerRotation;
            float radius = 15.0f + layer * 3.0f;

            // Base position in circle
            glm::vec3 basePos(
                room.spawnPosition.x + cos(angle) * radius,
                room.spawnPosition.y + layerHeight,
                room.spawnPosition.z + sin(angle) * radius
            );

            // 4D rotation effect - objects seem to distort and move in impossible ways
            // We simulate this by applying oscillating transformations
            float w = sin(time * 0.5f + i * 0.2f + layer * 0.7f); // 4th dimension

            // Position distortion based on 4D component
            glm::vec3 distortedPos = basePos + glm::vec3(
                sin(w * 3.14159f) * 2.0f,
                cos(w * 2.0f * 3.14159f) * 2.0f,
                sin(w * 4.0f * 3.14159f) * 2.0f
            );

            // Create a shape that seems to change between cube and other forms
            float morph = 0.5f + 0.5f * sin(time + i * 0.3f + layer * 0.5f);
            glm::vec3 scale(
                1.0f + morph * 0.5f * sin(time * 0.7f + i),
                1.0f + morph * 0.5f * cos(time * 0.8f + i),
                1.0f + morph * 0.5f * sin(time * 0.9f + i)
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, distortedPos);

            // Complex rotation that simulates 4D rotation projection
            model = glm::rotate(model, time * (0.5f + 0.2f * i),
                glm::normalize(glm::vec3(sin(w), cos(w), 0.5f)));
            model = glm::rotate(model, time * 0.7f * (1.0f - w),
                glm::normalize(glm::vec3(0.0f, sin(w), cos(w))));

            model = glm::scale(model, scale);

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Central "hypercube" representation
    const int numFaces = 8; // 8 "faces" of a hypercube (actually cubes themselves)
    for (int i = 0; i < numFaces; i++) {
        float t = i / (float)numFaces;
        float angle = t * 2.0f * 3.14159f;

        // Calculate 4D coordinates (w is 4th dimension)
        float w = sin(time * 0.5f + i * 3.14159f / 4.0f);

        // Project 4D hypercube "cell" to 3D
        glm::vec3 cellPos = room.spawnPosition + glm::vec3(
            cos(angle + time * 0.3f) * 5.0f * (1.0f + 0.5f * w),
            sin(time * 0.5f + i) * 3.0f * (1.0f + 0.3f * w),
            sin(angle + time * 0.3f) * 5.0f * (1.0f + 0.5f * w)
        );

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cellPos);
        model = glm::rotate(model, time + i * 0.5f,
            glm::vec3(cos(i), sin(i), 0.5f));
        model = glm::scale(model, glm::vec3(1.5f * (0.7f + 0.3f * w)));

        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void RoomManager::renderSphericalGeometry(Shader& shader, unsigned int cubeVAO, float time) {
    const Room& room = rooms[8];

    // Create a spherical geometry space - where parallel lines converge
    const int latSegments = 12;
    const int longSegments = 24;
    const float radius = 20.0f;

    // Create a sphere of cubes to represent the "surface" of spherical space
    for (int lat = 0; lat < latSegments; lat++) {
        float phi = lat * 3.14159f / latSegments;
        float y = radius * cos(phi);
        float sliceRadius = radius * sin(phi);

        for (int lon = 0; lon < longSegments; lon++) {
            float theta = lon * 2.0f * 3.14159f / longSegments;
            float x = sliceRadius * cos(theta);
            float z = sliceRadius * sin(theta);

            glm::vec3 cubePos = room.spawnPosition + glm::vec3(x, y, z);

            // Make the sphere "breathe"
            float breatheFactor = 1.0f + 0.1f * sin(time * 0.5f);
            cubePos = room.spawnPosition + (cubePos - room.spawnPosition) * breatheFactor;

            // Orient cubes to "face" outward
            glm::vec3 normal = glm::normalize(cubePos - room.spawnPosition);
            glm::vec3 up = (glm::abs(normal.y) > 0.99f) ?
                glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 right = glm::normalize(glm::cross(up, normal));
            up = glm::cross(normal, right);

            glm::mat4 rotationMatrix(
                glm::vec4(right, 0.0f),
                glm::vec4(up, 0.0f),
                glm::vec4(normal, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePos);
            model = model * rotationMatrix;
            model = glm::scale(model, glm::vec3(0.5f + 0.3f * sin(time + lat * 0.2f + lon * 0.1f)));

            shader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Add "meridian" lines to illustrate spherical geometry
    for (int i = 0; i < 3; i++) {
        float angle = i * 3.14159f / 3.0f + time * 0.1f;
        glm::vec3 axis(cos(angle), 0.0f, sin(angle));

        const int lineSegments = 30;
        for (int j = 0; j < lineSegments; j++) {
            float t = j * 3.14159f / lineSegments;

            // Point on great circle
            glm::vec3 greatCirclePos = room.spawnPosition + radius * glm::normalize(
                glm::vec3(
                    axis.x * cos(t),
                    sin(t),
                    axis.z * cos(t)
                )
            );

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, greatCirclePos);
            model = glm::scale(model, glm::vec3(0.3f));

            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

void RoomManager::renderInfiniteCorridor(Shader& shader, unsigned int cubeVAO, float time) {
    const Room& room = rooms[9];

    // Create a corridor that appears to extend infinitely
    const int corridorSegments = 30;
    const float segmentLength = 5.0f;
    const float corridorWidth = 5.0f;
    const float corridorHeight = 5.0f;

    // Calculate base position (far behind player)
    glm::vec3 corridorStart = room.spawnPosition - glm::vec3(0.0f, 0.0f, 50.0f);

    // Create floor, ceiling and walls of corridor segments
    for (int i = 0; i < corridorSegments; i++) {
        float distanceScale = pow(1.5f, -(corridorSegments - i - 1) * 0.1f);
        float segZ = i * segmentLength * distanceScale;

        // Actual position with non-linear scaling to create illusion of infinite distance
        glm::vec3 segmentPos = corridorStart + glm::vec3(0.0f, 0.0f, segZ);

        // Apply a subtle warp effect
        float warpFactor = sin(time * 0.5f + i * 0.2f) * 0.1f * i / corridorSegments;
        segmentPos.x += warpFactor * corridorWidth;

        // Current segment width and height (shrink with distance to enhance infinite effect)
        float currentWidth = corridorWidth * distanceScale;
        float currentHeight = corridorHeight * distanceScale;

        // Floor
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, segmentPos + glm::vec3(0.0f, -currentHeight / 2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(currentWidth, 0.1f * distanceScale, segmentLength * distanceScale));
        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Ceiling
        model = glm::mat4(1.0f);
        model = glm::translate(model, segmentPos + glm::vec3(0.0f, currentHeight / 2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(currentWidth, 0.1f * distanceScale, segmentLength * distanceScale));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Left wall
        model = glm::mat4(1.0f);
        model = glm::translate(model, segmentPos + glm::vec3(-currentWidth / 2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f * distanceScale, currentHeight, segmentLength * distanceScale));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Right wall
        model = glm::mat4(1.0f);
        model = glm::translate(model, segmentPos + glm::vec3(currentWidth / 2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f * distanceScale, currentHeight, segmentLength * distanceScale));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Add some decorative elements that highlight the infinite nature
        if (i % 2 == 0) {
            // Floating cubes that get smaller with distance
            float cubeSize = 0.5f * distanceScale;
            float hoverHeight = sin(time + i) * 0.5f;

            model = glm::mat4(1.0f);
            model = glm::translate(model, segmentPos + glm::vec3(0.0f, hoverHeight, 0.0f));
            model = glm::rotate(model, time + i * 0.2f, glm::vec3(0.3f, 1.0f, 0.7f));
            model = glm::scale(model, glm::vec3(cubeSize));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Add an "unreachable" end of corridor
    // This always stays ahead of the player no matter how far they go
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, corridorStart + glm::vec3(0.0f, 0.0f, corridorSegments * segmentLength));
    model = glm::scale(model, glm::vec3(corridorWidth * 0.1f, corridorHeight * 0.1f, 0.1f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RoomManager::setupRoomShader(Shader& shader, int roomIndex, float time) {
    // Set room type and intensity
    shader.setInt("roomType", roomIndex);
    shader.setFloat("roomIntensity", rooms[roomIndex].nonEuclideanIntensity);
    shader.setFloat("time", time);
}