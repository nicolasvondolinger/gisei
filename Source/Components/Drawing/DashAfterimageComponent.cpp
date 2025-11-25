#include "DashAfterimageComponent.h"
#include "AnimatorComponent.h"
#include "../../Actors/Samurai.h"
#include "../../Game.h"
#include "../../Renderer/Shader.h"

DashAfterimageComponent::DashAfterimageComponent(Actor* owner, int drawOrder)
    : DrawComponent(owner, drawOrder) {
}

void DashAfterimageComponent::Draw(Renderer* renderer) {
    Samurai* samurai = dynamic_cast<Samurai*>(mOwner);
    if (!samurai) return;
    
    const auto& particles = samurai->GetDashParticles();
    if (particles.empty()) return;
    
    Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();
    
    for (const auto& particle : particles) {
        float alpha = particle.lifetime / particle.maxLifetime;
        Vector3 color(0.4f + alpha * 0.4f, 0.7f + alpha * 0.3f, 1.0f);
        
        Vector2 size(particle.size, particle.size);
        
        renderer->DrawRect(
            particle.position,
            size,
            0.0f,
            color,
            cameraPos,
            RendererMode::TRIANGLES
        );
    }
}
