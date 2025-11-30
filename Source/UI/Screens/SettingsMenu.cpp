#include "SettingsMenu.h"
#include "../../Game.h"
#include "../UIButton.h"
#include "../UIText.h"
#include "../../Renderer/Renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

SettingsMenu::SettingsMenu(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName)
    , mVolumeLevel(5)
    , mDifficultyLevel(1)
    , mIsFullscreen(true)
    , mLanguageIndex(0) {
    
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

    auto& lang = mGame->GetLanguage();

    // Detect current language index
    mLanguageIndex = (lang.GetCurrentLang() == "pt") ? 1 : 0;

    mTitleText = AddText(lang.Get("settings.title"), Vector2(centerX, 120.0f), 1.2f, 0.0f, 56, 900, 1010);
    mTitleText->SetTextColor(textHover);
    mTitleText->SetBackgroundColor(transparent);

    mVolumeText = AddText(lang.Get("settings.volume"), Vector2(centerX, centerY - 140.0f), 0.9f, 0.0f, 36, 900, 1011);
    mVolumeText->SetBackgroundColor(transparent);

    mFullscreenText = AddText(lang.Get("settings.fullscreen"), Vector2(centerX, centerY - 60.0f), 0.9f, 0.0f, 36, 900, 1012);
    mFullscreenText->SetBackgroundColor(transparent);

    mDifficultyText = AddText(lang.Get("settings.difficulty"), Vector2(centerX, centerY + 20.0f), 0.9f, 0.0f, 36, 900, 1013);
    mDifficultyText->SetBackgroundColor(transparent);

    mLanguageText = AddText(lang.Get("settings.language"), Vector2(centerX, centerY + 100.0f), 0.9f, 0.0f, 36, 900, 1014);
    mLanguageText->SetBackgroundColor(transparent);

    mBackButton = AddButton(lang.Get("settings.back"), [this]() {
        if (mGame->GetUIStack().size() > 1) {
             auto prevUI = mGame->GetUIStack()[mGame->GetUIStack().size() - 2];
             prevUI->SetState(UIState::Active);
        }
        Close();
    }, Vector2(centerX, centerY + 200.0f), 1.0f, 0.0f, 42, 900, 1015);

    mBackButton->SetTextColor(textMain);
    mBackButton->SetHoverColor(textHover);
    mBackButton->SetBackgroundColor(transparent);

    mHintText = AddText(lang.Get("settings.hint"), Vector2(centerX, Game::WINDOW_HEIGHT - 50.0f), 0.6f, 0.0f, 20, 900, 1020);
    mHintText->SetTextColor(Vector3(0.5f, 0.6f, 0.52f));
    mHintText->SetBackgroundColor(transparent);

    Uint32 flags = SDL_GetWindowFlags(mGame->GetRenderer()->GetWindow());
    mIsFullscreen = (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0;
    mFullscreenText->SetText(mIsFullscreen ? lang.Get("settings.fullscreen.on") : lang.Get("settings.fullscreen.off"));

    mSelectedButtonIndex = 0; 
    UpdateSelectColor();
    UpdateVolumeText();
    UpdateDifficultyText();
    UpdateFullscreenText();
    UpdateLanguageText();
}

SettingsMenu::~SettingsMenu() {
}

void SettingsMenu::UpdateSelectColor() {
    const Vector3 textMain(0.91f, 0.94f, 0.91f);   // #E8EFE8
    const Vector3 textHover(0.66f, 0.80f, 0.70f); // #A9CDB2

    mVolumeText->SetTextColor(textMain);
    mFullscreenText->SetTextColor(textMain);
    mDifficultyText->SetTextColor(textMain);
    mLanguageText->SetTextColor(textMain);
    mBackButton->SetHighlighted(false);

    switch (mSelectedButtonIndex) {
        case 0: mVolumeText->SetTextColor(textHover); break;
        case 1: mFullscreenText->SetTextColor(textHover); break;
        case 2: mDifficultyText->SetTextColor(textHover); break;
        case 3: mLanguageText->SetTextColor(textHover); break;
        case 4: mBackButton->SetHighlighted(true); break;
    }
}

void SettingsMenu::UpdateVolumeText() {
    auto& lang = mGame->GetLanguage();
    int percentage = mVolumeLevel * 10;
    std::string value = std::to_string(percentage);
    std::string tmpl = lang.Get("settings.volume");
    const std::string token = "{value}";
    size_t pos = tmpl.find(token);
    if (pos != std::string::npos) {
        tmpl.replace(pos, token.size(), value);
        mVolumeText->SetText(tmpl);
    } else {
        mVolumeText->SetText("Volume: " + value + "%");
    }
    Mix_VolumeMusic((MIX_MAX_VOLUME * mVolumeLevel) / 10);
}

void SettingsMenu::UpdateDifficultyText() {
    auto& lang = mGame->GetLanguage();
    std::string key = (mDifficultyLevel == 0) ? "settings.difficulty.easy" :
                      (mDifficultyLevel == 1) ? "settings.difficulty.normal" : "settings.difficulty.hard";
    std::string difficultyText = lang.Get(key);

    std::string tmpl = lang.Get("settings.difficulty");
    const std::string token = "{value}";
    size_t pos = tmpl.find(token);
    if (pos != std::string::npos) {
        tmpl.replace(pos, token.size(), difficultyText);
        mDifficultyText->SetText(tmpl);
    } else {
        mDifficultyText->SetText(difficultyText);
    }
}

void SettingsMenu::UpdateFullscreenText() {
    auto& lang = mGame->GetLanguage();
    mFullscreenText->SetText(mIsFullscreen ? lang.Get("settings.fullscreen.on") : lang.Get("settings.fullscreen.off"));

    SDL_Window* window = mGame->GetRenderer()->GetWindow();
    Uint32 targetFlag = mIsFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    if (SDL_SetWindowFullscreen(window, targetFlag) != 0) {
        mIsFullscreen = !mIsFullscreen;
        mFullscreenText->SetText(mIsFullscreen ? lang.Get("settings.fullscreen.on") : lang.Get("settings.fullscreen.off"));
        return;
    }

    if (!mIsFullscreen) {
        constexpr int windowedWidth = 1600;
        constexpr int windowedHeight = 900;
        SDL_SetWindowSize(window, windowedWidth, windowedHeight);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    mGame->GetRenderer()->UpdateViewportToWindow();
}

void SettingsMenu::UpdateLanguageText() {
    auto& lang = mGame->GetLanguage();
    std::string current = (mLanguageIndex == 0) ? lang.Get("settings.language.en") : lang.Get("settings.language.pt");
    std::string tmpl = lang.Get("settings.language");
    const std::string token = "{value}";
    size_t pos = tmpl.find(token);
    if (pos != std::string::npos) {
        tmpl.replace(pos, token.size(), current);
        mLanguageText->SetText(tmpl);
    } else {
        mLanguageText->SetText(current);
    }
}

void SettingsMenu::RefreshStaticTexts() {
    const std::string langCode = (mLanguageIndex == 0) ? "en" : "pt";
    auto& lang = mGame->GetLanguage();
    lang.Load(langCode, "../Assets/Lang");

    // Disparar rebuild das UIs na próxima atualização
    mGame->OnLanguageChanged();
}

void SettingsMenu::HandleKeyPress(int key) {
    const int MAX_ITEMS = 4; 

    switch (key) {
        case SDLK_DOWN:
        case SDLK_s:
            if (mSelectedButtonIndex < MAX_ITEMS) {
                mSelectedButtonIndex++;
                UpdateSelectColor();
                mGame->PlaySound(mGame->GetUIHoverSound());
            }
            break;

        case SDLK_UP:
        case SDLK_w:
            if (mSelectedButtonIndex > 0) {
                mSelectedButtonIndex--;
                UpdateSelectColor();
                mGame->PlaySound(mGame->GetUIHoverSound());
            }
            break;

        case SDLK_LEFT:
        case SDLK_a:
            if (mSelectedButtonIndex == 0 && mVolumeLevel > 0) {
                mVolumeLevel--;
            } else if (mSelectedButtonIndex == 1) {
                mIsFullscreen = !mIsFullscreen;
            } else if (mSelectedButtonIndex == 2 && mDifficultyLevel > 0) {
                mDifficultyLevel--;
            } else if (mSelectedButtonIndex == 3) {
                mLanguageIndex = (mLanguageIndex == 0) ? 1 : 0;
                RefreshStaticTexts();
            }
            UpdateVolumeText();
            UpdateFullscreenText();
            UpdateDifficultyText();
            UpdateLanguageText();
            mGame->PlaySound(mGame->GetUIHoverSound());
            break;

        case SDLK_RIGHT:
        case SDLK_d:
            if (mSelectedButtonIndex == 0 && mVolumeLevel < 10) {
                mVolumeLevel++;
            } else if (mSelectedButtonIndex == 1) {
                mIsFullscreen = !mIsFullscreen;
            } else if (mSelectedButtonIndex == 2 && mDifficultyLevel < 2) {
                mDifficultyLevel++;
            } else if (mSelectedButtonIndex == 3) {
                mLanguageIndex = (mLanguageIndex == 0) ? 1 : 0;
                RefreshStaticTexts();
            }
            UpdateVolumeText();
            UpdateFullscreenText();
            UpdateDifficultyText();
            UpdateLanguageText();
            mGame->PlaySound(mGame->GetUIHoverSound());
            break;

        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:
            if (mSelectedButtonIndex == 4) {
                mGame->PlaySound(mGame->GetUIBackSound());
                mBackButton->OnClick();
            } else if (mSelectedButtonIndex == 1) {
                mIsFullscreen = !mIsFullscreen;
                UpdateFullscreenText();
            } else if (mSelectedButtonIndex == 3) {
                mLanguageIndex = (mLanguageIndex == 0) ? 1 : 0;
                RefreshStaticTexts();
                UpdateVolumeText();
                UpdateFullscreenText();
                UpdateDifficultyText();
                UpdateLanguageText();
            }
            break;

        case SDLK_ESCAPE:
            mGame->PlaySound(mGame->GetUIBackSound());
            mBackButton->OnClick(); 
            break;
    }
}
