#include "YouWin.h"
#include "../../Game.h"
#include "../UIButton.h"
#include "../UIRect.h"
#include "../UIText.h"

YouWin::YouWin(class Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
    float width = Game::WINDOW_WIDTH;
    float height = Game::WINDOW_HEIGHT;
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;

    const Vector3 titleColor(0.95f, 0.93f, 0.80f);
    const Vector3 buttonText(0.92f, 0.85f, 0.78f);
    const Vector3 buttonHover(0.70f, 0.88f, 0.76f);
    const Vector4 buttonBg(0.10f, 0.12f, 0.20f, 0.82f);
    const Vector4 overlay(0.03f, 0.03f, 0.08f, 0.65f);
    const Vector4 transparent(0.0f, 0.0f, 0.0f, 0.0f);

    AddRect(Vector2(centerX, centerY), Vector2(width, height), 1.0f, 0.0f, 5)->SetColor(overlay);

    UIText* title = AddText("YOU WIN", Vector2(centerX, centerY - 140.0f), 2.5f, 0.0f, 60, 900, 20);
    title->SetBackgroundColor(transparent);
    title->SetTextColor(titleColor);

    Vector2 menuPos(centerX, centerY + 20.0f);
    Vector2 quitPos(centerX, centerY + 100.0f);

    AddButton("MAIN MENU", [this]() {
        mGame->SetMapPrefix("level1");
        mGame->SetScene(GameScene::MainMenu);
        Close();
    }, menuPos, 1.0f, 0.0f, 46, 900, 30);

    AddButton("QUIT", [this]() {
        mGame->Quit();
    }, quitPos, 1.0f, 0.0f, 46, 900, 31);

    for (auto button : mButtons) {
        button->SetTextColor(buttonText);
        button->SetHoverColor(buttonHover);
        button->SetBackgroundColor(buttonBg);
    }
}

void YouWin::HandleKeyPress(int key) {
    if (mButtons.empty()) return;

    if(key == SDLK_DOWN || key == SDLK_s) {
        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex++;
        mSelectedButtonIndex%=mButtons.size();
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        mGame->PlaySound(mGame->GetUIHoverSound());
    } else if(key == SDLK_UP || key == SDLK_w){
        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex--;
        mSelectedButtonIndex%=mButtons.size();
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        mGame->PlaySound(mGame->GetUIHoverSound());
    } else if (key==SDLK_RETURN){
        mGame->PlaySound(mGame->GetUIConfirmSound());
        mButtons[mSelectedButtonIndex]->OnClick();
    }
}
