#include "MainMenu.h"
#include "SettingsMenu.h"
#include "ControlsMenu.h"
#include "../../Game.h"
#include "../UIButton.h"
#include "../../Renderer/Renderer.h"
#include "../../Math.h"

MainMenu::MainMenu(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName) {
    float centerX = Game::WINDOW_WIDTH / 2.0f;
    float centerY = Game::WINDOW_HEIGHT / 2.0f;

    const Vector3 textMain(0.91f, 0.94f, 0.91f);      // #E8EFE8
    const Vector3 textHover(0.66f, 0.80f, 0.70f);    // #A9CDB2
    const Vector3 buttonBg(0.11f, 0.14f, 0.12f);     // #1C241F
    const Vector3 borderColor(0.30f, 0.37f, 0.32f);  // #4D5F52
    const Vector4 transparent(0.0f, 0.0f, 0.0f, 0.0f);

    AddImage("../Assets/UI/MenuBackground.jpeg", Vector2(centerX, centerY), 0.6f, 0.0f, 0);

    AddImage("../Assets/UI/NewLogo.png", Vector2(centerX, 280.0f), 1.0f, 0.0f, 10);

    Vector2 playButtonPos(centerX, centerY + 120.0f);
    Vector2 settingsButtonPos(centerX, centerY + 200.0f);
    Vector2 controlsButtonPos(centerX, centerY + 280.0f);
    Vector2 exitButtonPos(centerX, centerY + 360.0f);

    UIButton *playButton = AddButton("Begin Journey", [this]() {
        Close();
        mGame->SetScene(GameScene::Level1);
    }, playButtonPos, 1.0f, 0.0f, 48, 900, 200);

    playButton->SetTextColor(textMain);
    playButton->SetHoverColor(textHover);
    playButton->SetBackgroundColor(Vector4(buttonBg, 0.8f));

    UIButton *settingsButton = AddButton("Settings", [this]() {
        SetState(UIState::Paused);
        new SettingsMenu(mGame, "../Assets/Fonts/Alkhemikal.ttf");
    }, settingsButtonPos, 1.0f, 0.0f, 48, 900, 200);

    settingsButton->SetTextColor(textMain);
    settingsButton->SetHoverColor(textHover);
    settingsButton->SetBackgroundColor(Vector4(buttonBg, 0.8f));

    UIButton *controlsButton = AddButton("Controls", [this]() {
        SetState(UIState::Paused);
        new ControlsMenu(mGame, "../Assets/Fonts/Alkhemikal.ttf");
    }, controlsButtonPos, 1.0f, 0.0f, 48, 900, 200);

    controlsButton->SetTextColor(textMain);
    controlsButton->SetHoverColor(textHover);
    controlsButton->SetBackgroundColor(Vector4(buttonBg, 0.8f));

    UIButton *exitButton = AddButton("Quit", [this]() {
        mGame->Quit();
    }, exitButtonPos, 1.0f, 0.0f, 48, 900, 200);

    exitButton->SetTextColor(textMain);
    exitButton->SetHoverColor(textHover);
    exitButton->SetBackgroundColor(Vector4(buttonBg, 0.8f));
}

void MainMenu::HandleKeyPress(int key) {
    if (mButtons.empty()) return;

    int currentSelection = mSelectedButtonIndex;

    switch (key) {
        case SDLK_DOWN:
        case SDLK_s:
            mButtons[currentSelection]->SetHighlighted(false);
            mSelectedButtonIndex = (currentSelection + 1) % mButtons.size();
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
            break;

        case SDLK_UP:
        case SDLK_w:
            mButtons[currentSelection]->SetHighlighted(false);
            mSelectedButtonIndex = (currentSelection - 1 + mButtons.size()) % mButtons.size();
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
            break;

        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:
            if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < mButtons.size()) {
                mButtons[mSelectedButtonIndex]->OnClick();
            }
            break;
    }
}
