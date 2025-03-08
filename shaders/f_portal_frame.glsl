#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec4 frameColor;
uniform float time;
uniform vec3 viewPos;

void main()
{
    // Base color from the portal frame color
    vec4 baseColor = frameColor;

    // Calculate distance from edge for pulsing effect
    vec2 texCenter = abs(TexCoord - 0.5) * 2.0;
    float edgeFactor = max(texCenter.x, texCenter.y);

    // Subtle pulse effect
    float pulse = sin(time * 1.2) * 0.3 + 0.7;

    // Subtle energy flow effect
    float flow = sin(TexCoord.y * 10.0 + time * 2.0) * 0.5 + 0.5;
    flow *= sin(TexCoord.x * 8.0 - time * 1.5) * 0.5 + 0.5;

    // Create a more metallic, refined look
    vec3 finalColor = baseColor.rgb * pulse;
    finalColor += baseColor.rgb * flow * 0.15; // More subtle energy flow

    // Specular highlight for metallic effect
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 normal = normalize(Normal);
    vec3 reflectDir = reflect(-viewDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64); // Sharper specular
    float specIntensity = 0.4;

    finalColor += vec3(1.0, 1.0, 1.0) * spec * specIntensity;

    // Output with slight transparency
    FragColor = vec4(finalColor, 0.9);
}

// Updated f_portal.glsl - Remove aura and make completely see-through
#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D portalTexture;  // Texture from the other side of portal
uniform vec4 edgeColor;           // Portal edge color
uniform float time;
uniform vec3 viewPos;

void main()
{
    // Sample the portal texture (view from the other side)
    vec4 portalView = texture(portalTexture, TexCoord);

    // Calculate distance from edge - will be used to create a very subtle edge effect
    vec2 texCenter = abs(TexCoord - 0.5) * 2.0;
    float distFromEdge = max(texCenter.x, texCenter.y);

    // Create a very subtle distortion effect at the edges
    float edgeWidth = 0.05;
    float edgeIntensity = smoothstep(1.0 - edgeWidth, 1.0, distFromEdge) * 0.1;

    // Slightly tint the edges with the portal color, but keep it mostly transparent
    vec4 finalColor = mix(portalView, edgeColor, edgeIntensity);

    // Make the portal completely transparent except for the very subtle edge
    FragColor = finalColor;
}