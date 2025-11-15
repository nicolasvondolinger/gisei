// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE.txt for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// This corresponds to the output color to the color buffer
out vec4 outColor;
uniform vec3 uColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

// This is used for texture blending
uniform float uTextureFactor;

// Tex coord input from vertex shader
in vec2 fragTexCoord;

void main()
{
    // 1. Amostra a cor da textura na coordenada interpolada
    vec4 texColor = texture(uTexture, fragTexCoord);

    // 2. Define a cor base (para geometria/sem textura)
    vec4 baseColor = vec4(uColor, 1.0);

    // 3. Define a cor final da textura (textura tintada pela cor uniforme)
    vec4 finalTexColor = texColor * baseColor;

    // 4. Interpola entre a cor base e a cor da textura
    // Se uTextureFactor = 0.0, usa baseColor
    // Se uTextureFactor = 1.0, usa finalTexColor
    outColor = mix(baseColor, finalTexColor, uTextureFactor);
}
