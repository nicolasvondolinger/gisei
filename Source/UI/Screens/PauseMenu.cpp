#include "PauseMenu.h"
#include "SettingsMenu.h"
#include "ControlsMenu.h"
#include "../../Game.h"
#include "../UIButton.h"
#include "../UIText.h"
#include "../UIRect.h"

PauseMenu::PauseMenu(class Game* game, const std::string& fontName)
        : UIScreen(game, fontName)
        , mResumeButton(nullptr)
        , mSettingsButton(nullptr)
        , mControlsButton(nullptr)
        , mMainMenuButton(nullptr)
        , mQuitButton(nullptr) {

    mIsTransparent = false;

    float centerX = Game::WINDOW_WIDTH / 2.0f;
    float centerY = Game::WINDOW_HEIGHT / 2.0f;

    const Vector3 textMain(0.91f, 0.94f, 0.91f);      // #E8EFE8
    const Vector3 textHover(0.66f, 0.80f, 0.70f);    // #A9CDB2
    const Vector3 buttonBg(0.11f, 0.14f, 0.12f);     // #1C241F
    const Vector4 darkOverlay(0.08f, 0.10f, 0.09f, 0.85f);
    const Vector4 transparent(0.0f, 0.0f, 0.0f, 0.0f);

    AddRect(Vector2(centerX, centerY), Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), 1.0f, 0.0f, 900);
    mRects[0]->SetColor(darkOverlay);

    UIText* title = AddText("PAUSE", Vector2(centerX, centerY - 200.0f), 1.1f, 0.0f, 60, 900, 910);
    title->SetTextColor(textHover);
    title->SetBackgroundColor(transparent);

    Vector2 resumePos(centerX, centerY - 60.0f);
    Vector2 settingsPos(centerX, centerY + 20.0f);
    Vector2 controlsPos(centerX, centerY + 100.0f);
    Vector2 mainMenuPos(centerX, centerY + 180.0f);
    Vector2 quitPos(centerX, centerY + 260.0f);

    mResumeButton = AddButton("Continue", [this]() {
        mGame->ResumeGame();
        Close();
    }, resumePos, 1.0f, 0.0f, 46, 900, 920);

    mSettingsButton = AddButton("Settings", [this]() {
        SetState(UIState::Paused);
        new SettingsMenu(mGame, "../Assets/Fonts/Alkhemikal.ttf");
    }, settingsPos, 1.0f, 0.0f, 46, 900, 921);

    mControlsButton = AddButton("Controls", [this]() {
        SetState(UIState::Paused);
        new ControlsMenu(mGame, "../Assets/Fonts/Alkhemikal.ttf");
    }, controlsPos, 1.0f, 0.0f, 46, 900, 922);

    mMainMenuButton = AddButton("Back to Menu", [this]() {
        mGame->ResumeGame();
        Close();
        mGame->SetScene(GameScene::MainMenu);
    }, mainMenuPos, 1.0f, 0.0f, 46, 900, 923);

    mQuitButton = AddButton("Leave", [this]() {
        mGame->Quit();
    }, quitPos, 1.0f, 0.0f, 46, 900, 924);

    for (auto button : mButtons) {
        button->SetTextColor(textMain);
        button->SetHoverColor(textHover);
        button->SetBackgroundColor(Vector4(buttonBg, 0.8f));
    }

    mSelectedButtonIndex = 0;
    ChangeSelection(0);
}

void PauseMenu::ChangeSelection(int newIndex) {
    if (mButtons.empty()) return;

    int clamped = (newIndex + static_cast<int>(mButtons.size())) % static_cast<int>(mButtons.size());
    for (size_t i = 0; i < mButtons.size(); ++i) {
        mButtons[i]->SetHighlighted(i == static_cast<size_t>(clamped));
    }
    mSelectedButtonIndex = clamped;
}

void PauseMenu::ActivateSelection() {
    if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())) {
        mButtons[mSelectedButtonIndex]->OnClick();
    }
}

void PauseMenu::HandleKeyPress(int key) {
    switch (key) {
        case SDLK_DOWN:
        case SDLK_s:
            ChangeSelection(mSelectedButtonIndex + 1);
            break;
        case SDLK_UP:
        case SDLK_w:
            ChangeSelection(mSelectedButtonIndex - 1);
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:
            ActivateSelection();
            break;
        case SDLK_ESCAPE:
        case SDLK_p:
            mResumeButton->OnClick();
            break;
    }
}
