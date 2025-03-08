#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform float time;

void main()
{
    // Grid pattern for orientation
    vec2 grid = abs(fract(FragPos.xz * 0.5) - 0.5);
    float gridPattern = max(0.0, 1.0 - step(0.025, min(grid.x, grid.y)) * 0.8);

    // Grid color - vibrant blue
    vec3 gridColor = vec3(0.1, 0.4, 0.9);

    // Base floor color - dark
    vec3 baseColor = vec3(0.05, 0.05, 0.1);

    // Mix grid with base color
    vec3 floorColor = mix(baseColor, gridColor, gridPattern);

    // Psychedelic color patterns for cubes and walls
    if (FragPos.y > 0.1) {
        // Position-based coloring
        float r = sin(FragPos.x * 1.5 + time) * 0.5 + 0.5;
        float g = sin(FragPos.y * 2.1 + time * 0.7) * 0.5 + 0.5;
        float b = sin(FragPos.z * 1.7 + time * 0.9) * 0.5 + 0.5;

        // Wavy distortion
        float distortion = sin(FragPos.x * 5.0 + FragPos.y * 3.0 + time * 2.0) * 0.2 +
            cos(FragPos.z * 4.0 + FragPos.y * 2.0 + time * 1.5) * 0.2;

        // Apply distortion to color
        r = r * (1.0 + distortion * 0.3);
        g = g * (1.0 + distortion * 0.2);
        b = b * (1.0 + distortion * 0.5);

        // Create color bands
        float bands = sin(FragPos.x * 10.0 + FragPos.y * 8.0 + FragPos.z * 6.0 + time * 3.0) * 0.5 + 0.5;

        // Mix colors
        vec3 psychColor = vec3(r, g, b);
        vec3 bandColor = vec3(b, r, g); // Rotate colors for bands

        floorColor = mix(psychColor, bandColor, bands);
    }

    // Simple lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(norm, lightDir), 0.3);

    // Add subtle glow
    float glow = sin(time * 1.5) * 0.15 + 0.15;

    // Final color with lighting and glow
    vec3 finalColor = floorColor * diff + vec3(0.1, 0.05, 0.3) * glow;

    FragColor = vec4(finalColor, 1.0);
}