#pragma once
#include "DrawComponent.h"
#include <map>

struct DashParticle;

class DashAfterimageComponent : public DrawComponent {
public:
    DashAfterimageComponent(class Actor* owner, int drawOrder = 95);
    
    void Update(float deltaTime) override;
    void Draw(Renderer* renderer) override;

private:
    class Texture* mMagicTexture;
    std::map<const DashParticle*, int> mParticleFrames;
};
