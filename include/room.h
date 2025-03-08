#pragma once
#ifndef ROOM_H
#define ROOM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "camera.h"
#include "shader.h"

// Structure to define a room's properties
struct Room {
    glm::vec3 spawnPosition;
    float spawnYaw;
    float spawnPitch;
    std::string name;
    float nonEuclideanIntensity;
    glm::vec4 ambientColor;
    bool hasGravity;
};

class RoomManager {
public:
    RoomManager();
    ~RoomManager();

    // Initialize rooms with their properties
    void initializeRooms();

    // Teleport player to a specific room
    void teleportToRoom(int roomIndex, Camera& camera, float& nonEuclideanFactor,
        bool& flightMode, float& verticalVelocity);

    // Get current room index
    int getCurrentRoomIndex() const;

    // Get room by index
    const Room& getRoom(int index) const;

    // Get total number of rooms
    size_t getRoomCount() const;

    // Room-specific rendering functions
    void renderRoomSpecificContent(int roomIndex, Shader& shader,
        unsigned int cubeVAO, float time);

    void setupRoomShader(Shader& shader, int roomIndex, float time);

private:
    std::vector<Room> rooms;
    int currentRoom;

    // Specialized rendering functions for each room type
    void renderHyperbolicRoom(Shader& shader, unsigned int cubeVAO, float time);
    void renderImpossibleArchitecture(Shader& shader, unsigned int cubeVAO, float time);
    void renderFractalSpace(Shader& shader, unsigned int cubeVAO, float time);
    void renderKleinBottleSpace(Shader& shader, unsigned int cubeVAO, float time);
    void renderEscherPlayground(Shader& shader, unsigned int cubeVAO, float time);
    void renderPsychedelicVortex(Shader& shader, unsigned int cubeVAO, float time);
    void renderRotatingHyperspace(Shader& shader, unsigned int cubeVAO, float time);
    void renderSphericalGeometry(Shader& shader, unsigned int cubeVAO, float time);
    void renderInfiniteCorridor(Shader& shader, unsigned int cubeVAO, float time);
    void renderMandelbulbFractalSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time);
    void renderEscherImpossibleArchitecture(Shader& shader, unsigned int cubeVAO, const Room& room, float time);
    void renderHyperbolicSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time);
    void renderKleinBottleSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time);
    void renderRecursiveScalingEnvironment(Shader& shader, unsigned int cubeVAO, const Room& room, float time);
    void renderQuantumSuperpositionSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time);
    void renderMobiusTopology(Shader& shader, unsigned int cubeVAO, const Room& room, float time);
    void renderNonCommutativeRotationSpace(Shader& shader, unsigned int cubeVAO, const Room& room, float time);
    void renderInfiniteRegressionChamber(Shader& shader, unsigned int cubeVAO, const Room& room, float time);
    void renderFractalStructure(Shader& shader, unsigned int cubeVAO, glm::vec3 center, float size, int depth, float time);
    void renderPortalFrame(Shader& shader, unsigned int cubeVAO, glm::vec3 position, float angle, float width, float height, float time);
    // Helper for fractal room
    void renderFractalCube(Shader& shader, unsigned int cubeVAO,
        glm::vec3 center, float size, int depth, float time);

    // Helper for psychedelic rooms
    void renderFloatingFractals(Shader& shader, unsigned int cubeVAO,
        const Room& room, float time);
};

#endif // ROOM_H