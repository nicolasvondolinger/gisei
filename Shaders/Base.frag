#version 330

out vec4 outColor;

uniform sampler2D uTexture;
uniform float uTextureFactor;
uniform vec4 uBaseColor;
uniform float uWhiteAura;

in vec2 fragTexCoord;

void main()
{
    vec4 texColor = texture(uTexture, fragTexCoord);
    vec4 finalTexColor = texColor * uBaseColor;
    vec4 result = mix(uBaseColor, finalTexColor, uTextureFactor);
    
    // Aura branca
    if (uWhiteAura > 0.0 && result.a > 0.1) {
        result.rgb = mix(result.rgb, vec3(1.0), uWhiteAura);
    }
    
    outColor = result;
}