#pragma once
#include "DrawComponent.h"

class DashAfterimageComponent : public DrawComponent {
public:
    DashAfterimageComponent(class Actor* owner, int drawOrder = 95);
    
    void Draw(Renderer* renderer) override;
};
