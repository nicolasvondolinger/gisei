#include "IntroCrawl.h"
#include "../../Game.h"
#include "../UIText.h"
#include "../UIRect.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>

IntroCrawl::IntroCrawl(class Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
    , mTitle(nullptr)
    , mSubtitle(nullptr)
    , mHint(nullptr)
    , mElapsed(0.0f)
    , mScrollSpeed(26.0f)
    , mTitlePulse(0.0f)
    , mOverlayCenter(Vector2(Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f))
{
    mIsTransparent = false;

    const float centerX = Game::WINDOW_WIDTH / 2.0f;
    const float centerY = Game::WINDOW_HEIGHT / 2.0f;

    const Vector3 textMain(0.91f, 0.94f, 0.91f);      // #E8EFE8
    const Vector3 textHover(0.66f, 0.80f, 0.70f);    // #A9CDB2
    const Vector4 darkOverlay(0.11f, 0.14f, 0.12f, 0.95f);
    const Vector4 transparent(0.0f, 0.0f, 0.0f, 0.0f);

    AddRect(mOverlayCenter, Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), 1.0f, 0.0f, 1000);
    mRects[0]->SetColor(darkOverlay);

    mSubtitle = AddText("Prologue", Vector2(centerX, 120.0f), 0.75f, 0.0f, 34, 2000, 1005);
    mSubtitle->SetTextColor(Vector3(0.7f, 0.76f, 0.7f));
    mSubtitle->SetBackgroundColor(transparent);

    mTitle = AddText("GISEI - THE SHATTERED BLADE", Vector2(centerX, 170.0f), 1.2f, 0.0f, 56, 2000, 1010);
    mTitle->SetTextColor(textHover);
    mTitle->SetBackgroundColor(transparent);

    float startY = Game::WINDOW_HEIGHT - 20.0f; // closer so first line appears quickly
    const float lineSpacing = 90.0f;
    const int pointSize = 42;
    const unsigned wrapLength = 4000; // avoid automatic wrapping
    int drawOrderBase = 1020;

    std::vector<std::string> lines = {
        "In the temple haze, silent monks forged a body of bamboo and steel.",
        "Into its chest, they sealed a warrior's fragment: Kenshi, the Ara-Hitogami.",
        "They promised him memory and glory at the Celestial Pinnacle.",
        "They called his journey a divine trial, a sacred path of sacrifice.",
        "But echoes of those before him whisper: no return, only offering.",
        "Demons lurk. The spirit blight devours lands and souls.",
        "To the order, the ninja is harvest - fuel for the forge, food for a sleeping god.",
        "If Kenshi reaches the Pinnacle, he must choose: revolt or obedience. Today, the blade awakens."
    };

    mLineDelays.resize(lines.size(), 0.0f);
    const float delayStep = 0.35f;

    for (size_t i = 0; i < lines.size(); ++i) {
        float y = startY + static_cast<float>(i) * lineSpacing;
        UIText* line = AddText(lines[i], Vector2(centerX, y), 1.0f, 0.0f, pointSize, wrapLength, drawOrderBase + static_cast<int>(i));
        line->SetTextColor(textMain);
        line->SetBackgroundColor(transparent);
        mLines.push_back(line);
        mLineDelays[i] = delayStep * static_cast<float>(i);
    }

    mHint = AddText("Press SPACE to begin", Vector2(centerX, Game::WINDOW_HEIGHT - 70.0f), 0.65f, 0.0f, 24, 900, 1100);
    mHint->SetTextColor(Vector3(0.5f, 0.6f, 0.52f));
    mHint->SetBackgroundColor(transparent);

    // Particles: subtle upward dust
    const int particleCount = 30;
    for (int i = 0; i < particleCount; ++i) {
        float x = static_cast<float>(rand() % static_cast<int>(Game::WINDOW_WIDTH));
        float y = static_cast<float>(rand() % static_cast<int>(Game::WINDOW_HEIGHT));
        float size = 2.0f + static_cast<float>(rand() % 4);
        UIRect* rect = AddRect(Vector2(x, y), Vector2(size, size), 1.0f, 0.0f, 1090 + i);
        rect->SetColor(Vector4(0.7f, 0.8f, 0.75f, 0.2f));

        Particle p;
        p.rect = rect;
        p.velocity = Vector2(0.0f, -8.0f - static_cast<float>(rand() % 18)); // slow upward drift
        p.lifetime = 2.5f + (rand() % 150) / 50.0f; // 2.5 - 5.5s
        p.maxLifetime = p.lifetime;
        mParticles.push_back(p);
    }
}

void IntroCrawl::Update(float deltaTime) {
    if (mState != UIState::Active) return;

    mElapsed += deltaTime;

    // Keep overlay fixed to avoid revealing the background
    mRects[0]->SetOffset(mOverlayCenter);

    // Title gentle pulse
    mTitlePulse += deltaTime;
    float pulse = 0.15f * std::sin(mTitlePulse * 1.2f);
    Vector3 baseTitle = Vector3(0.66f, 0.80f, 0.70f);
    Vector3 pulsed = baseTitle * (1.0f + pulse);
    mTitle->SetTextColor(pulsed);

    // Lines slide until they settle under the title, with staggered delay
    const float lineSpacing = 90.0f;
    const float targetFirstY = 260.0f;
    float deltaY = mScrollSpeed * deltaTime;

    for (size_t i = 0; i < mLines.size(); ++i) {
        if (mElapsed < mLineDelays[i]) continue;

        Vector2 pos = mLines[i]->GetOffset();
        float desiredY = targetFirstY + static_cast<float>(i) * lineSpacing;
        if (mLines.front()->GetOffset().y > targetFirstY) {
            pos.y -= deltaY;
            if (pos.y < desiredY) pos.y = desiredY;
            mLines[i]->SetOffset(pos);
        } else {
            mLines[i]->SetOffset(Vector2(pos.x, desiredY));
        }
    }

    // Particles update
    for (auto& p : mParticles) {
        Vector2 pos = p.rect->GetOffset();
        pos += p.velocity * deltaTime;
        p.lifetime -= deltaTime;

        if (p.lifetime <= 0.0f || pos.y < -20.0f) {
            pos.x = static_cast<float>(rand() % static_cast<int>(Game::WINDOW_WIDTH));
            pos.y = Game::WINDOW_HEIGHT + static_cast<float>(rand() % 80);
            p.velocity = Vector2(0.0f, -8.0f - static_cast<float>(rand() % 18));
            p.lifetime = 2.5f + (rand() % 150) / 50.0f;
            p.maxLifetime = p.lifetime;
        }

        p.rect->SetOffset(pos);
    }
}

void IntroCrawl::HandleKeyPress(int key) {
    switch (key) {
        case SDLK_SPACE:
            Finish();
            break;
    }
}

void IntroCrawl::Finish() {
    if (mState != UIState::Active) return;
    mState = UIState::Closing;
    mGame->SetScene(GameScene::Level1);
}
