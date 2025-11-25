#pragma once
#include "../Component.h"
#include "../../Math.h"
#include <vector>

struct SmokeParticle {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    float maxLifetime;
    float size;
    float initialSize;
};

class DashSmokeComponent : public Component {
public:
    DashSmokeComponent(class Actor* owner, int updateOrder = 10);
    
    void Update(float deltaTime) override;
    void Draw(class Renderer* renderer);
    
    void EmitDash(const Vector2& position);
    
private:
    std::vector<SmokeParticle> mParticles;
};
