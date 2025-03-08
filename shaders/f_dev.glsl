#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;

void main()
{
    // Basic grid pattern
    vec2 grid = abs(fract(FragPos.xz * 0.5) - 0.5);
    float gridPattern = max(0.0, 1.0 - step(0.025, min(grid.x, grid.y)) * 0.8);

    // Grid color
    vec3 gridColor = vec3(0.1, 0.4, 0.7);

    // Base floor color - very dark
    vec3 baseColor = vec3(0.1, 0.1, 0.15);

    // Mix grid with base color
    vec3 floorColor = mix(baseColor, gridColor, gridPattern);

    // Simple lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(norm, lightDir), 0.3);

    // Final color with lighting
    vec3 finalColor = floorColor * diff;

    FragColor = vec4(finalColor, 1.0);
}