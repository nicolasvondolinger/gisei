#include "DashAfterimageComponent.h"
#include "../../Actors/Ninja.h"
#include "../../Game.h"

DashAfterimageComponent::DashAfterimageComponent(Actor* owner, int drawOrder)
    : DrawComponent(owner, drawOrder) {
}

void DashAfterimageComponent::Update(float deltaTime) {
}

void DashAfterimageComponent::Draw(Renderer* renderer) {
    Ninja* ninja = dynamic_cast<Ninja*>(mOwner);
    if (!ninja) return;
    
    const auto& particles = ninja->GetDashParticles();
    if (particles.empty()) return;
    
    Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();
    
    for (const auto& particle : particles) {
        float alpha = particle.lifetime / particle.maxLifetime;
        Vector3 color(0.9f, 0.9f, 0.95f);
        
        // Desenha 3 retângulos deslocados e de tamanhos diferentes
        float baseSize = particle.size;
        
        // Retângulo 1 - maior, no centro
        renderer->DrawRect(
            particle.position,
            Vector2(baseSize, baseSize),
            0.0f,
            color,
            cameraPos,
            RendererMode::TRIANGLES
        );
        
        // Retângulo 2 - médio, deslocado
        renderer->DrawRect(
            particle.position + Vector2(baseSize * 0.3f, -baseSize * 0.2f),
            Vector2(baseSize * 0.7f, baseSize * 0.8f),
            0.0f,
            color,
            cameraPos,
            RendererMode::TRIANGLES
        );
        
        // Retângulo 3 - menor, mais deslocado
        renderer->DrawRect(
            particle.position + Vector2(-baseSize * 0.4f, baseSize * 0.3f),
            Vector2(baseSize * 0.5f, baseSize * 0.6f),
            0.0f,
            color,
            cameraPos,
            RendererMode::TRIANGLES
        );
    }
}
