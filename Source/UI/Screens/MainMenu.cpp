#include "MainMenu.h"
#include "SettingsMenu.h"
#include "../../Game.h"
#include "../UIButton.h"
#include "../../Renderer/Renderer.h"
#include "../../Math.h"

MainMenu::MainMenu(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName) {
    float centerX = Game::WINDOW_WIDTH / 2.0f;
    float centerY = Game::WINDOW_HEIGHT / 2.0f;

    const Vector3 white(1.0f, 1.0f, 1.0f);
    const Vector3 crimson(0.8f, 0.1f, 0.1f);
    const Vector3 gold(1.0f, 0.84f, 0.0f);
    const Vector4 darkOverlay(0.0f, 0.0f, 0.0f, 0.7f);
    const Vector4 transparent(0.0f, 0.0f, 0.0f, 0.0f);

    AddRect(Vector2(centerX, centerY), Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), 1.0f, 0.0f, 0);
    mRects[0]->SetColor(darkOverlay);

    AddImage("../Assets/UI/Logo.png", Vector2(centerX, 250.0f), 0.25f, 0.0f, 10);

    Vector2 playButtonPos(centerX, centerY + 100.0f);
    Vector2 settingsButtonPos(centerX, centerY + 180.0f);
    Vector2 exitButtonPos(centerX, centerY + 260.0f);

    UIButton *playButton = AddButton("[ Begin Journey ]", [this]() {
        Close();
        mGame->SetScene(GameScene::Level1);
    }, playButtonPos, 1.0f, 0.0f, 42, 900, 200);

    playButton->SetTextColor(white);
    playButton->SetHoverColor(crimson);
    playButton->SetBackgroundColor(transparent);

    UIButton *settingsButton = AddButton("[ Settings ]", [this]() {
        SetState(UIState::Paused);
        new SettingsMenu(mGame, "../Assets/Fonts/Alkhemikal.ttf");
    }, settingsButtonPos, 1.0f, 0.0f, 42, 900, 200);

    settingsButton->SetTextColor(white);
    settingsButton->SetHoverColor(crimson);
    settingsButton->SetBackgroundColor(transparent);

    UIButton *exitButton = AddButton("[ Leave ]", [this]() {
        mGame->Quit();
    }, exitButtonPos, 1.0f, 0.0f, 42, 900, 200);

    exitButton->SetTextColor(white);
    exitButton->SetHoverColor(crimson);
    exitButton->SetBackgroundColor(transparent);
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