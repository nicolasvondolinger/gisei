#include "DashSmokeComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Renderer.h"
#include "../../Random.h"

DashSmokeComponent::DashSmokeComponent(Actor* owner, int updateOrder)
    : Component(owner, updateOrder) {
}

void DashSmokeComponent::Update(float deltaTime) {
    const float DRAG = 12.0f;
    
    for (auto it = mParticles.begin(); it != mParticles.end();) {
        it->lifetime -= deltaTime;
        
        if (it->lifetime <= 0.0f || it->size <= 0.0f) {
            it = mParticles.erase(it);
            continue;
        }
        
        it->velocity *= (1.0f - DRAG * deltaTime);
        it->position += it->velocity * deltaTime;
        
        float progress = 1.0f - (it->lifetime / it->maxLifetime);
        it->size = it->initialSize * (1.0f - progress);
        
        ++it;
    }
}

void DashSmokeComponent::Draw(Renderer* renderer) {
    if (mParticles.empty()) return;
    
    Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();
    
    for (const auto& particle : mParticles) {
        float alpha = particle.lifetime / particle.maxLifetime;
        Vector3 color(0.85f, 0.85f, 0.9f);
        
        Vector2 screenPos = particle.position - cameraPos;
        
        renderer->DrawRect(
            screenPos,
            Vector2(particle.size, particle.size),
            0.0f,
            color,
            Vector2::Zero,
            RendererMode::TRIANGLES
        );
    }
}

void DashSmokeComponent::EmitDash(const Vector2& position) {
    const int NUM_PARTICLES = Random::GetIntRange(5, 8);
    const float SPEED = 300.0f;
    const float LIFETIME = 0.3f;
    
    for (int i = 0; i < NUM_PARTICLES; i++) {
        float angle = Random::GetFloatRange(0.0f, Math::TwoPi);
        float speed = Random::GetFloatRange(SPEED * 0.5f, SPEED);
        
        SmokeParticle particle;
        particle.position = position;
        particle.velocity = Vector2(Math::Cos(angle), Math::Sin(angle)) * speed;
        particle.lifetime = LIFETIME;
        particle.maxLifetime = LIFETIME;
        particle.size = Random::GetFloatRange(3.0f, 6.0f);
        particle.initialSize = particle.size;
        
        mParticles.push_back(particle);
    }
}
