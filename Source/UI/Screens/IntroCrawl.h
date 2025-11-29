#pragma once

#include "UIScreen.h"
#include <vector>

class UIText;
class UIRect;

class IntroCrawl : public UIScreen {
public:
    IntroCrawl(class Game* game, const std::string& fontName);
    ~IntroCrawl() override = default;

    void Update(float deltaTime) override;
    void HandleKeyPress(int key) override;

private:
    void Finish();
    struct Particle {
        UIRect* rect;
        Vector2 velocity;
        float lifetime;
        float maxLifetime;
    };

    std::vector<UIText*> mLines;
    std::vector<float> mLineDelays;
    std::vector<Particle> mParticles;
    UIText* mTitle;
    UIText* mSubtitle;
    UIText* mHint;

    float mElapsed;
    float mScrollSpeed;
    float mTitlePulse;
    Vector2 mOverlayCenter;
};
