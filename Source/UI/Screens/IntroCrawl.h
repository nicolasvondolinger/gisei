#pragma once

#include "UIScreen.h"
#include <vector>
#include <SDL2/SDL_mixer.h>

class UIText;
class UIRect;

class IntroCrawl : public UIScreen {
public:
    IntroCrawl(class Game* game, const std::string& fontName);
    ~IntroCrawl() override;

    void Update(float deltaTime) override;
    void HandleKeyPress(int key) override;

private:
    void Finish();
    struct Particle {
        UIRect* rect;
        Vector2 velocity;
        float lifetime;
        float maxLifetime;
        float phase;
        float driftAmplitude;
    };

    struct Streak {
        UIRect* rect;
        Vector2 velocity;
        float lifetime;
        float maxLifetime;
        float alpha;
    };

    vector<UIText*> mLines;
    vector<float> mLineDelays;
    vector<Particle> mParticles;
    vector<Streak> mStreaks;
    UIText* mTitle;
    UIText* mSubtitle;
    UIText* mHint;

    float mElapsed;
    float mScrollSpeed;
    float mTitlePulse;
    Vector2 mOverlayCenter;

    void LoadNarration();
    Mix_Music* mNarration;
};
