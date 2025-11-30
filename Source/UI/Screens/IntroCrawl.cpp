#include "IntroCrawl.h"
#include "../../Game.h"
#include "../UIText.h"
#include "../UIRect.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
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
    , mNarration(nullptr)
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

    auto& lang = mGame->GetLanguage();

    mSubtitle = AddText(lang.Get("intro.subtitle"), Vector2(centerX, 120.0f), 0.75f, 0.0f, 34, 2000, 1005);
    mSubtitle->SetTextColor(Vector3(0.7f, 0.76f, 0.7f));
    mSubtitle->SetBackgroundColor(transparent);

    mTitle = AddText(lang.Get("intro.title"), Vector2(centerX, 170.0f), 1.2f, 0.0f, 56, 2000, 1010);
    mTitle->SetTextColor(textHover);
    mTitle->SetBackgroundColor(transparent);

    float startY = Game::WINDOW_HEIGHT + 40.0f; // still rápido, mas com margem
    const float lineSpacing = 70.0f;
    const int pointSize = 42;
    const unsigned wrapLength = 4000; // avoid automatic wrapping
    int drawOrderBase = 1020;

    std::vector<std::string> lines = {
        lang.Get("intro.line1"),
        lang.Get("intro.line2"),
        lang.Get("intro.line3"),
        lang.Get("intro.line4"),
        lang.Get("intro.line5"),
        lang.Get("intro.line6"),
        lang.Get("intro.line7"),
        lang.Get("intro.line8")
    };

    for (size_t i = 0; i < lines.size(); ++i) {
        float y = startY + static_cast<float>(i) * lineSpacing;
        UIText* line = AddText(lines[i], Vector2(centerX, y), 1.0f, 0.0f, pointSize, wrapLength, drawOrderBase + static_cast<int>(i));
        line->SetTextColor(textMain);
        line->SetBackgroundColor(transparent);
        mLines.push_back(line);
    }

    mHint = AddText(lang.Get("intro.hint"), Vector2(centerX, Game::WINDOW_HEIGHT - 70.0f), 0.65f, 0.0f, 24, 900, 1100);
    mHint->SetTextColor(Vector3(0.5f, 0.6f, 0.52f));
    mHint->SetBackgroundColor(transparent);

    // Particles: dust + wind streaks
    const int particleCount = 120;
    for (int i = 0; i < particleCount; ++i) {
        float x = static_cast<float>(rand() % static_cast<int>(Game::WINDOW_WIDTH));
        float y = static_cast<float>(rand() % static_cast<int>(Game::WINDOW_HEIGHT));
        float size = 1.5f + static_cast<float>(rand() % 5);
        UIRect* rect = AddRect(Vector2(x, y), Vector2(size, size), 1.0f, 0.0f, 1090 + i);
        rect->SetColor(Vector4(0.7f, 0.8f, 0.75f, 0.15f));

        Particle p;
        p.rect = rect;
        p.velocity = Vector2(0.0f, -8.0f - static_cast<float>(rand() % 18)); // slow upward drift
        p.phase = static_cast<float>(rand() % 628) / 100.0f; // 0..2pi-ish
        p.driftAmplitude = 8.0f + static_cast<float>(rand() % 12); // subtle sideways drift
        p.lifetime = 2.5f + (rand() % 150) / 50.0f; // 2.5 - 5.5s
        p.maxLifetime = p.lifetime;
        mParticles.push_back(p);
    }

    // Wind streaks (long faint trails)
    const int streakCount = 24;
    for (int i = 0; i < streakCount; ++i) {
        float w = 60.0f + static_cast<float>(rand() % 80);
        float h = 2.0f + static_cast<float>(rand() % 3);
        float x = static_cast<float>(rand() % static_cast<int>(Game::WINDOW_WIDTH));
        float y = static_cast<float>(rand() % static_cast<int>(Game::WINDOW_HEIGHT));
        UIRect* rect = AddRect(Vector2(x, y), Vector2(w, h), 1.0f, 0.0f, 1150 + i);
        float baseAlpha = 0.06f + (rand() % 8) / 100.0f; // 0.06 - 0.14
        rect->SetColor(Vector4(0.7f, 0.9f, 0.85f, baseAlpha));

        Streak s;
        s.rect = rect;
        s.velocity = Vector2(-20.0f - static_cast<float>(rand() % 25), -10.0f - static_cast<float>(rand() % 20));
        s.lifetime = 2.0f + (rand() % 150) / 60.0f; // 2 - 4.5s
        s.maxLifetime = s.lifetime;
        s.alpha = baseAlpha;
        mStreaks.push_back(s);
    }

    LoadNarration();
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

    // Lines slide until they settle under the title (no stagger, no snap movement)
    const float lineSpacing = 70.0f;
    const float targetFirstY = 250.0f;
    float deltaY = mScrollSpeed * deltaTime;
    bool shouldMove = !mLines.empty() && mLines.front()->GetOffset().y > targetFirstY;

    if (shouldMove) {
        for (size_t i = 0; i < mLines.size(); ++i) {
            Vector2 pos = mLines[i]->GetOffset();
            float desiredY = targetFirstY + static_cast<float>(i) * lineSpacing;
            pos.y -= deltaY;
            if (pos.y < desiredY) pos.y = desiredY;
            mLines[i]->SetOffset(pos);
        }
    }

    // Particles update with gentle horizontal drift (vento)
    for (auto& p : mParticles) {
        Vector2 pos = p.rect->GetOffset();
        pos += p.velocity * deltaTime;
        pos.x += std::sin(p.phase + mElapsed * 1.2f) * p.driftAmplitude * deltaTime;
        p.lifetime -= deltaTime;

        if (p.lifetime <= 0.0f || pos.y < -20.0f) {
            pos.x = static_cast<float>(rand() % static_cast<int>(Game::WINDOW_WIDTH));
            pos.y = Game::WINDOW_HEIGHT + static_cast<float>(rand() % 80);
            p.velocity = Vector2(0.0f, -8.0f - static_cast<float>(rand() % 18));
            p.phase = static_cast<float>(rand() % 628) / 100.0f;
            p.driftAmplitude = 8.0f + static_cast<float>(rand() % 12);
            p.lifetime = 2.5f + (rand() % 150) / 50.0f;
            p.maxLifetime = p.lifetime;
        }

        p.rect->SetOffset(pos);
    }

    // Wind streaks update (long faint trails)
    for (auto& s : mStreaks) {
        Vector2 pos = s.rect->GetOffset();
        pos += s.velocity * deltaTime;
        s.lifetime -= deltaTime;

        float lifeRatio = (s.maxLifetime > 0.0f) ? std::max(0.0f, s.lifetime / s.maxLifetime) : 0.0f;
        Vector4 color = Vector4(0.7f, 0.9f, 0.85f, s.alpha * lifeRatio);
        s.rect->SetColor(color);

        if (s.lifetime <= 0.0f || pos.y < -30.0f || pos.x < -80.0f) {
            pos.x = Game::WINDOW_WIDTH + static_cast<float>(rand() % 120);
            pos.y = Game::WINDOW_HEIGHT * 0.3f + static_cast<float>(rand() % static_cast<int>(Game::WINDOW_HEIGHT * 0.7f));
            s.velocity = Vector2(-20.0f - static_cast<float>(rand() % 25), -10.0f - static_cast<float>(rand() % 20));
            s.maxLifetime = 2.0f + (rand() % 150) / 60.0f;
            s.lifetime = s.maxLifetime;
            s.alpha = 0.06f + (rand() % 8) / 100.0f;
            s.rect->SetColor(Vector4(0.7f, 0.9f, 0.85f, s.alpha));
        }

        s.rect->SetOffset(pos);
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
    if (mNarration) {
        Mix_HaltMusic();
        Mix_FreeMusic(mNarration);
        mNarration = nullptr;
    }
    mGame->SetScene(GameScene::Level1);
}

IntroCrawl::~IntroCrawl() {
    if (mNarration) {
        Mix_FreeMusic(mNarration);
        mNarration = nullptr;
    }
}

void IntroCrawl::LoadNarration() {
    std::string langCode = mGame->GetLanguage().GetCurrentLang();
    std::string file = (langCode == "pt")
        ? "../Assets/Sounds/Narration/intro_pt.mp3"
        : "../Assets/Sounds/Narration/intro_en.mp3";

    mNarration = Mix_LoadMUS(file.c_str());
    if (mNarration) {
        Mix_PlayMusic(mNarration, 0);
    }
}
