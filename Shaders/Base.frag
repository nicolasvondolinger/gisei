#version 330

out vec4 outColor;

// Textura e fator de mistura (0 = só cor, 1 = textura tintada)
uniform sampler2D uTexture;
uniform float uTextureFactor;

// Cor base (RGBA) vinda do C++
uniform vec4 uBaseColor;

in vec2 fragTexCoord;

void main()
{
    // Amostra a textura
    vec4 texColor = texture(uTexture, fragTexCoord);

    // Se estivermos desenhando texto (uTextureFactor 1.0),
    // queremos: CorDoTexto (uBaseColor) * FormatoDaLetra (texColor)
    vec4 finalTexColor = texColor * uBaseColor;

    // Se estivermos desenhando fundo (uTextureFactor 0.0),
    // queremos apenas uBaseColor.

    // mix faz a interpolação linear entre os dois
    outColor = mix(uBaseColor, finalTexColor, uTextureFactor);
}