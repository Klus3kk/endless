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