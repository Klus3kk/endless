#version 410 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform int roomType;
uniform float roomIntensity;

void main()
{
    // Original position
    vec3 position = aPos;

    // Apply warping based on room type and position
    if (position.y > 0.0) {
        // Calculate distance from origin in xz plane
        float dist = length(position.xz);

        // Basic warping effect
        float warpFactor = sin(dist * 0.5 - time * 0.8) * 0.1;
        position.y += warpFactor * position.y;

        // Rotation effect
        float angle = dist * 0.1 + time * 0.2;
        float sinA = sin(angle);
        float cosA = cos(angle);

        vec3 warpedPos = position;
        warpedPos.x = position.x * cosA - position.z * sinA * 0.2;
        warpedPos.z = position.z * cosA + position.x * sinA * 0.2;

        position = mix(position, warpedPos, min(1.0, dist * 0.05));
    }

    // Transform to world space
    FragPos = vec3(model * vec4(position, 1.0));

    // Transform normal to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pass texture coordinates
    TexCoord = aTexCoord;

    // Final position
    gl_Position = projection * view * vec4(FragPos, 1.0);
}