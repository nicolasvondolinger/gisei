#include "SettingsMenu.h"
#include "../../Game.h"
#include "../UIButton.h"
#include "../UIText.h"
#include "../../Renderer/Renderer.h"
#include <SDL2/SDL_mixer.h>

SettingsMenu::SettingsMenu(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName)
    , mVolumeLevel(5)
    , mDifficultyLevel(1)
    , mIsFullscreen(true) {
    
    mIsTransparent = false;
    
    float centerX = Game::WINDOW_WIDTH / 2.0f;
    float centerY = Game::WINDOW_HEIGHT / 2.0f;

    const Vector3 textMain(0.91f, 0.94f, 0.91f);      // #E8EFE8
    const Vector3 textHover(0.66f, 0.80f, 0.70f);    // #A9CDB2
    const Vector3 buttonBg(0.11f, 0.14f, 0.12f);     // #1C241F
    const Vector4 darkOverlay(0.11f, 0.14f, 0.12f, 0.95f);
    const Vector4 transparent(0.0f, 0.0f, 0.0f, 0.0f);

    AddRect(Vector2(centerX, centerY), Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), 1.0f, 0.0f, 1000);
    mRects[0]->SetColor(darkOverlay);

    UIText* title = AddText("SETTINGS", Vector2(centerX, 150.0f), 1.2f, 0.0f, 56, 900, 1010);
    title->SetTextColor(textHover);
    title->SetBackgroundColor(transparent);

    mVolumeText = AddText("Volume: 50%", Vector2(centerX, centerY - 80.0f), 0.9f, 0.0f, 36, 900, 1011);
    mVolumeText->SetBackgroundColor(transparent);

    mFullscreenText = AddText("Fullscreen: ON", Vector2(centerX, centerY), 0.9f, 0.0f, 36, 900, 1012);
    mFullscreenText->SetBackgroundColor(transparent);

    mDifficultyText = AddText("Difficulty: Normal", Vector2(centerX, centerY + 80.0f), 0.9f, 0.0f, 36, 900, 1013);
    mDifficultyText->SetBackgroundColor(transparent);

    mBackButton = AddButton("Back", [this]() {
        if (mGame->GetUIStack().size() > 1) {
             auto prevUI = mGame->GetUIStack()[mGame->GetUIStack().size() - 2];
             prevUI->SetState(UIState::Active);
        }
        Close();
    }, Vector2(centerX, centerY + 180.0f), 1.0f, 0.0f, 42, 900, 1014);

    mBackButton->SetTextColor(textMain);
    mBackButton->SetHoverColor(textHover);
    mBackButton->SetBackgroundColor(transparent);

    UIText* hint = AddText("Arrows to Change | Enter to Select", Vector2(centerX, Game::WINDOW_HEIGHT - 50.0f), 0.6f, 0.0f, 20, 900, 1015);
    hint->SetTextColor(Vector3(0.5f, 0.6f, 0.52f));
    hint->SetBackgroundColor(transparent);

    mSelectedButtonIndex = 0; 
    UpdateSelectColor();
}

SettingsMenu::~SettingsMenu() {
}

void SettingsMenu::UpdateSelectColor() {
    const Vector3 textMain(0.91f, 0.94f, 0.91f);   // #E8EFE8
    const Vector3 textHover(0.66f, 0.80f, 0.70f); // #A9CDB2

    mVolumeText->SetTextColor(textMain);
    mFullscreenText->SetTextColor(textMain);
    mDifficultyText->SetTextColor(textMain);
    mBackButton->SetHighlighted(false);

    switch (mSelectedButtonIndex) {
        case 0: mVolumeText->SetTextColor(textHover); break;
        case 1: mFullscreenText->SetTextColor(textHover); break;
        case 2: mDifficultyText->SetTextColor(textHover); break;
        case 3: mBackButton->SetHighlighted(true); break;
    }
}

void SettingsMenu::UpdateVolumeText() {
    int percentage = mVolumeLevel * 10;
    mVolumeText->SetText("Volume: " + std::to_string(percentage) + "%");
    Mix_VolumeMusic((MIX_MAX_VOLUME * mVolumeLevel) / 10);
}

void SettingsMenu::UpdateDifficultyText() {
    std::string difficulty = mDifficultyLevel == 0 ? "Easy" : (mDifficultyLevel == 1 ? "Normal" : "Hard");
    mDifficultyText->SetText("Difficulty: " + difficulty);
}

void SettingsMenu::UpdateFullscreenText() {
    mFullscreenText->SetText(mIsFullscreen ? "Fullscreen: ON" : "Fullscreen: OFF");
    SDL_SetWindowFullscreen(mGame->GetRenderer()->GetWindow(), mIsFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
}

void SettingsMenu::HandleKeyPress(int key) {
    const int MAX_ITEMS = 3; 

    switch (key) {
        case SDLK_DOWN:
        case SDLK_s:
            if (mSelectedButtonIndex < MAX_ITEMS) {
                mSelectedButtonIndex++;
                UpdateSelectColor();
                // PlaySound(MoveCursor); // Futuro
            }
            break;

        case SDLK_UP:
        case SDLK_w:
            if (mSelectedButtonIndex > 0) {
                mSelectedButtonIndex--;
                UpdateSelectColor();
            }
            break;

        case SDLK_LEFT:
        case SDLK_a:
            if (mSelectedButtonIndex == 0 && mVolumeLevel > 0) {
                mVolumeLevel--;
                UpdateVolumeText();
            } else if (mSelectedButtonIndex == 1) {
                mIsFullscreen = !mIsFullscreen;
                UpdateFullscreenText();
            } else if (mSelectedButtonIndex == 2 && mDifficultyLevel > 0) {
                mDifficultyLevel--;
                UpdateDifficultyText();
            }
            break;

        case SDLK_RIGHT:
        case SDLK_d:
            if (mSelectedButtonIndex == 0 && mVolumeLevel < 10) {
                mVolumeLevel++;
                UpdateVolumeText();
            } else if (mSelectedButtonIndex == 1) {
                mIsFullscreen = !mIsFullscreen;
                UpdateFullscreenText();
            } else if (mSelectedButtonIndex == 2 && mDifficultyLevel < 2) {
                mDifficultyLevel++;
                UpdateDifficultyText();
            }
            break;

        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:
            if (mSelectedButtonIndex == 3) {
                mBackButton->OnClick();
            } else if (mSelectedButtonIndex == 1) {
                mIsFullscreen = !mIsFullscreen;
                UpdateFullscreenText();
            }
            break;

        case SDLK_ESCAPE:
            mBackButton->OnClick(); 
            break;
    }
}