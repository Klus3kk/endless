#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform float time;
uniform int roomType;
uniform float roomIntensity;

// Noise functions for fractal generation
float hash(float n) {
    return fract(sin(n) * 43758.5453);
}

float noise(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);

    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
        mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
        mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
            mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

// Fractional Brownian Motion for more complex noise
float fbm(vec3 x) {
    float v = 0.0;
    float a = 0.5;
    vec3 shift = vec3(100);

    for (int i = 0; i < 5; ++i) {
        v += a * noise(x);
        x = x * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

// Distance estimator functions for ray marching fractals
float mandelbulbDE(vec3 pos, float power) {
    vec3 z = pos;
    float dr = 1.0;
    float r = 0.0;

    for (int i = 0; i < 7; i++) {
        r = length(z);
        if (r > 2.0) break;

        // Convert to polar coordinates with safety checks
        float theta = (r > 0.0001) ? acos(clamp(z.z / r, -1.0, 1.0)) : 0.0;
        float phi = atan(z.y, z.x);
        dr = pow(max(r, 0.0001), power - 1.0) * power * dr + 1.0;

        // Scale and rotate the point
        float zr = pow(r, power);
        theta = theta * power;
        phi = phi * power;

        // Convert back to cartesian coordinates
        z = zr * vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        z += pos; // Add the position to create a Julia set variation
    }
    return 0.5 * log(max(r, 0.0001)) * r / max(dr, 0.0001);
}

// Apply different effects based on room type
vec3 applyRoomEffect(vec3 position, vec3 normal, float t) {
    vec3 color = vec3(0.5);

    switch (roomType) {
    case 1: // Mandelbulb Fractal Space
    {
        // Fractal coloring based on position and time
        float dist = mandelbulbDE(position * 0.1, 8.0 + 4.0 * sin(t * 0.1));
        float pattern = fbm(position * 0.2 + t * 0.1);

        // Create iridescent colors
        color.r = 0.5 + 0.5 * sin(pattern * 5.0 + t + 0.0);
        color.g = 0.5 + 0.5 * sin(pattern * 5.0 + t + 2.0);
        color.b = 0.5 + 0.5 * sin(pattern * 5.0 + t + 4.0);

        // Add glow based on distance estimator
        float glow = clamp(1.0 / (max(dist, 0.0001) * 10.0), 0.0, 1.0);
        color += vec3(0.2, 0.5, 0.8) * glow;
    }
    break;

    case 2: // Escher's Impossible Architecture
    {
        // Create an impossible space effect with grid patterns
        vec3 gridPos = position;
        gridPos.y = mod(gridPos.y + gridPos.x * sin(t * 0.2) * 0.2, 4.0);

        vec3 grid = abs(fract(gridPos * 0.5) - 0.5);
        float gridPattern = step(0.05, min(min(grid.x, grid.y), grid.z));

        // Monochromatic color scheme with subtle shifts
        float shift = sin(gridPos.x * 0.2 + gridPos.y * 0.3 + gridPos.z * 0.1 + t * 0.2);
        color = mix(vec3(0.2), vec3(0.9), gridPattern) * (0.8 + 0.2 * shift);
    }
    break;

    case 3: // Hyperbolic Space
    {
        // Hyperbolic pattern - colors intensify toward edges
        float dist = length(position);
        float distortion = 1.0 / (1.0 + dist * 0.1);

        // Create a warped grid-like pattern
        vec3 warpedPos = position * distortion;
        vec3 pattern = 0.5 + 0.5 * sin(warpedPos.xyz * 2.0 + t * vec3(0.3, 0.2, 0.1));

        color = mix(vec3(0.1, 0.3, 0.6), vec3(0.5, 0.8, 1.0), pattern);
        color *= (1.0 - distortion) * 2.0; // Darken toward center
    }
    break;

    case 4: // Klein Bottle Folding Space
    {
        // Klein bottle-inspired colors and patterns
        vec3 p = position;

        // Create a twisting pattern that folds back on itself
        float fold = sin(p.x * 0.5 + t * 0.2) * cos(p.z * 0.5 - t * 0.1);
        p.y += fold * 0.5;

        // Striped pattern that shifts between inside and outside
        float stripe = sin(p.x * 2.0 + p.y * 3.0 + p.z * 1.0 + t * 0.5) *
            cos(p.x * 1.0 - p.y * 2.0 + p.z * 3.0 - t * 0.3);

        // Combine purples and blues for Klein bottle effect
        color = mix(
            vec3(0.6, 0.1, 0.8), // Purple
            vec3(0.2, 0.5, 0.9), // Blue
            0.5 + 0.5 * stripe
        );

        // Add highlights based on normal orientation
        float highlight = max(0.0, dot(normal, normalize(vec3(1.0, 2.0, 1.0))));
        color += vec3(highlight) * 0.3;
    }
    break;

    case 5: // Recursive Scaling Environment
    {
        // Create a recursive scaling pattern
        vec3 p = position;
        float scale = 2.0;
        float intensity = 0.0;

        for (int i = 0; i < 5; i++) {
            p = fract(p * scale) - 0.5;
            float d = length(p) * exp(-float(i));
            intensity += exp(-d * 8.0);
            scale *= 1.2;
        }

        // Create color bands that shift with the recursive pattern
        float r = 0.5 + 0.5 * sin(intensity * 3.0 + t * 0.5);
        float g = 0.5 + 0.5 * sin(intensity * 3.0 + t * 0.5 + 2.0);
        float b = 0.5 + 0.5 * sin(intensity * 3.0 + t * 0.5 + 4.0);

        color = vec3(r, g, b);
    }
    break;

    case 6: // Quantum Superposition Space
    {
        // Create a quantum-inspired effect with wave patterns
        vec3 p = position;

        // Wave interference patterns
        float wave1 = sin(p.x * 2.0 + t * 1.1) * sin(p.y * 2.5 + t * 0.5) * sin(p.z * 3.0 + t * 0.7);
        float wave2 = sin(p.x * 3.0 - t * 0.8) * sin(p.y * 2.0 - t * 1.2) * sin(p.z * 2.5 - t * 0.3);

        // Create interference effect
        float interference = wave1 * wave2;

        // Vibrant complementary colors
        color = vec3(
            0.5 + 0.5 * sin(interference * 5.0 + t),
            0.5 + 0.5 * sin(interference * 5.0 + t + 2.0),
            0.5 + 0.5 * sin(interference * 5.0 + t + 4.0)
        );

        // Add pulsating glow
        float pulse = 0.5 + 0.5 * sin(t * 3.0);
        color += vec3(0.2, 0.0, 0.3) * pulse * abs(interference);
    }
    break;

    case 7: // Möbius Topology
    {
        // Create a continuous twisting pattern
        vec3 p = position;

        // Calculate angle around an imaginary Möbius strip
        float angle = atan(p.z, p.x);
        float radius = length(p.xz);

        // Create twist based on angle
        float twist = sin(angle * 0.5 + t * 0.3) * cos(radius * 0.5 - t * 0.2);

        // Apply twist
        float pattern = sin(p.x * 2.0 + p.y * twist * 3.0 + p.z * 1.5 + t * 0.5);

        // Gradient colors that flow along the twist
        float hue = angle / (2.0 * 3.14159) + t * 0.1;

        // Convert HSV to RGB for smooth color cycling
        vec3 c = clamp(abs(mod(hue * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        color = c * (0.6 + 0.4 * pattern);
    }
    break;

    case 8: // Non-Commutative Rotation Space
    {
        // Create a space where rotation order matters
        vec3 p = position;

        // Different rotation matrices applied in sequence
        float a = t * 0.2;
        float b = t * 0.3;

        // First rotation around Y
        float s1 = sin(a);
        float c1 = cos(a);
        vec3 q = vec3(p.x * c1 - p.z * s1, p.y, p.x * s1 + p.z * c1);

        // Second rotation around X
        float s2 = sin(b);
        float c2 = cos(b);
        q = vec3(q.x, q.y * c2 - q.z * s2, q.y * s2 + q.z * c2);

        // Create a pattern based on the twice-rotated position
        float pattern = sin(q.x * 3.0 + q.y * 2.0 + q.z * 4.0 + t * 0.4);

        // Angular color patterns
        color = vec3(
            0.5 + 0.5 * sin(pattern * 3.0 + t * 0.7),
            0.5 + 0.5 * sin(pattern * 4.0 + t * 0.6),
            0.5 + 0.5 * sin(pattern * 5.0 + t * 0.5)
        );
    }
    break;

    case 9: // Infinite Regression Chamber
    {
        // Create an infinite regression effect
        vec3 p = position;

        // Scale coordinates logarithmically to create regression - with safety
        float dist = length(p);
        float scale = log(dist + 1.0) / (dist + 0.0001); // Added safety epsilon
        p *= scale;

        // Apply fractal noise
        float pattern = fbm(p * 1.0 + t * 0.1);

        // DMT-inspired color palette
        color = vec3(
            0.5 + 0.5 * sin(pattern * 5.0 + t * 0.3),
            0.5 + 0.5 * sin(pattern * 5.0 + t * 0.3 + 2.0),
            0.5 + 0.5 * sin(pattern * 5.0 + t * 0.3 + 4.0)
        );

        // Add pulsating intensity based on recursion level
        float pulse = 0.5 + 0.5 * sin(t * 2.0);
        float recursion = fract(log2(dist + 1.0) + t * 0.2);
        color *= 0.7 + 0.5 * smoothstep(0.4, 0.6, recursion) * pulse;
    }
    break;

    default: // Default room
        color = vec3(0.5); // Default gray
        break;
    }

    // Scale effect by room intensity
    color = mix(vec3(0.5), color, roomIntensity);

    return color;
}

void main()
{
    // Normalize the normal vector
    vec3 norm = normalize(Normal);

    // Get the view direction
    vec3 viewDir = normalize(viewPos - FragPos);

    // Base lighting calculation
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(norm, lightDir), 0.2);

    // Apply room-specific effect
    vec3 roomColor = applyRoomEffect(FragPos, norm, time);

    // Apply diffuse lighting
    vec3 finalColor = roomColor * diff;

    // Add specular highlights
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    finalColor += specularStrength * spec * vec3(1.0);

    // Output final color
    FragColor = vec4(finalColor, 1.0);
}