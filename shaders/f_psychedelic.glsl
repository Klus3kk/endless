#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform float time;
uniform vec2 resolution;

// Function to create psychedelic patterns using sine waves
vec3 psychedelicPattern(vec2 uv, float time) {
    float scale = 10.0;

    // Create warping effect
    float warpX = sin(uv.x * scale + time) * 0.1;
    float warpY = cos(uv.y * scale + time) * 0.1;
    uv += vec2(warpY, warpX);

    // Generate complex pattern
    float r = sin(uv.x * 3.0 + time) * 0.5 + 0.5;
    float g = sin(uv.y * 3.7 + time * 1.1) * 0.5 + 0.5;
    float b = sin((uv.x + uv.y) * 2.3 + time * 0.7) * 0.5 + 0.5;

    // Add fractal-like patterns
    float fractalVal = 0.0;
    float amp = 1.0;
    float freq = 1.0;
    for (int i = 0; i < 5; i++) {
        float noiseX = sin(uv.x * freq + time * 0.2) * sin(uv.y * freq * 1.5 + time * 0.1);
        float noiseY = cos(uv.x * freq * 0.8 - time * 0.1) * cos(uv.y * freq * 1.2 + time * 0.2);
        fractalVal += (noiseX + noiseY) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }

    // Mix the pattern with the fractal
    vec3 color = vec3(r, g, b) + vec3(sin(fractalVal * 3.14)) * 0.2;

    return color;
}

void main()
{
    // Normalize UVs to (-1, 1) range
    vec2 uv = (TexCoord * 2.0 - 1.0);

    // Get view direction
    vec3 viewDir = normalize(viewPos - FragPos);

    // Get normal
    vec3 norm = normalize(Normal);

    // Use the dot product for lighting effect
    float diffuse = max(dot(norm, vec3(0.0, 1.0, 0.0)), 0.2);

    // Get psychedelic base color
    vec3 psychColor = psychedelicPattern(uv, time);

    // Add depth effect based on view direction
    float depthEffect = pow(abs(dot(viewDir, norm)), 2.0);
    psychColor *= (0.7 + depthEffect * 0.3);

    // Add glow effect
    float glowVal = sin(time * 2.0) * 0.5 + 0.5;
    psychColor += vec3(0.1, 0.05, 0.2) * glowVal;

    // Apply diffuse lighting
    vec3 finalColor = psychColor * diffuse;

    // Output color
    FragColor = vec4(finalColor, 1.0);
}