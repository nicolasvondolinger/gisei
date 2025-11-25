#include "ParallaxComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Texture.h"

ParallaxComponent::ParallaxComponent(class Actor* owner, int updateOrder)
    : Component(owner, updateOrder) {
}

ParallaxComponent::~ParallaxComponent() {
}

void ParallaxComponent::AddLayer(const std::string& texturePath, float scrollSpeed, float offsetY) {
    ParallaxLayer layer;
    layer.texture = GetGame()->GetRenderer()->GetTexture(texturePath, true);
    layer.scrollSpeed = scrollSpeed;
    layer.offsetY = offsetY;
    mLayers.push_back(layer);
}

void ParallaxComponent::Draw(class Renderer* renderer) {
    if (!mIsEnabled) return;

    Vector2 cameraPos = GetGame()->GetCameraPos();
    float screenWidth = Game::WINDOW_WIDTH;
    float screenHeight = Game::WINDOW_HEIGHT;

    for (const auto& layer : mLayers) {
        if (!layer.texture) continue;

        Vector2 parallaxCam = cameraPos * layer.scrollSpeed;
        float texWidth = static_cast<float>(layer.texture->GetWidth());
        float texHeight = static_cast<float>(layer.texture->GetHeight());
        
        float uOffset = parallaxCam.x / texWidth;
        float uScale = screenWidth / texWidth;
        float vScale = screenHeight / texHeight;

        Vector4 texRect(uOffset, 0.0f, uScale, vScale);
        Vector2 quadPos = cameraPos + Vector2(screenWidth / 2.0f, screenHeight / 2.0f + layer.offsetY);
        Vector2 quadSize(screenWidth, screenHeight);

        renderer->DrawTexture(
            quadPos,
            quadSize,
            0.0f,
            Vector3(1.0f, 1.0f, 1.0f),
            layer.texture,
            texRect,
            cameraPos,
            false,
            1.0f
        );
    }
}
