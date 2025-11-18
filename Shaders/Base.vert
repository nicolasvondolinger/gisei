#version 330

// ATENÇÃO: Deve ser vec3 para casar com o C++ (x, y, z)
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;

// Uniforms
uniform mat4 uWorldTransform;
uniform mat4 uOrthoProj;
uniform vec2 uCameraPos;
uniform vec4 uTexRect; // (u, v, largura, altura) da região da textura
uniform bool uIsUI;    // Flag para UI

// Saída
out vec2 fragTexCoord;

void main() {
    // 1. Calcula Posição no Mundo
    // Usamos inPosition (vec3) e adicionamos 1.0 para w
    vec4 worldPos = uWorldTransform * vec4(inPosition, 1.0);

    // 2. Aplica Câmera (se não for UI)
    if (!uIsUI)
    {
        worldPos.xy -= uCameraPos;
    }

    // 3. Projeção Ortográfica
    gl_Position = uOrthoProj * worldPos;

    // 4. Cálculo de Coordenada de Textura
    // uTexRect.xy = offset inicial, uTexRect.zw = escala (tamanho)
    fragTexCoord = uTexRect.xy + inTexCoord * uTexRect.zw;
}